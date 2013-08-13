/**
LL(k) Recursive Descent Parser (wegen INC und DEC Operatoren)
Homogeneous AST (weil: C)

Arithmetik:
        ADD     +
        INC     ++ (pre- und post)
        SUB     -
        DEC     -- (pre- und post)
        MUL     *
        POW     **
        DIV     /
        MOD     %

Variablen Deklaration und Zuweisung für die Typen bool, int, float, String
        Deklaration     Typ Name [:= Initialwert]
        Zuweisung       :=

Kommandos zur Definition und Verarbeitung von Mengen (Domänen) und binären homogenen und heterogenen Relationen mithilfe von RelaFix
        Datentyp        "R"
        Set heyting := { 0 a 1 }
        R myRel := heyting x heyting -> heyting : { 1 1 1 || 0 1 1 || 0 a 1 }

Logische Tests und Operatoren:
        EQUAL           ==
        NOT             !
        AND             &&
        OR              ||
        IMPLIES 
        XOR             ^^
        LESS            <
        LESSEQUAL       <=
        GREATER         >
        GREATEREQUAL    >=

Kontrollstrukturen:
        while Test Statement
        if Test Statement [else Statement]

Definition und Aufruf von Prozeduren
        Typ Name(Parameter, … ) { …; return […]; }

Befehle zur Ausgabe von Werten in verschiedenen Darstellungsformaten

*/

%{
#include <stdio.h>

#include "ast.h"

#define YYERROR_VERBOSE 1
#define YYDEBUG 1

#include "rlang.tab.h"

int yylex(YYSTYPE *, YYLTYPE *);

void
yyerror(YYLTYPE *llocp, Ast **ast, const char *yymsg) {
	printf("my_yyerror: %s\n", yymsg);
}

%}

%locations
%define api.pure
%parse-param {Ast **ast}

%union {
        Ast *node;
        char *value;
}

%printer { print_node(yyoutput, $$); } <node>
%printer { fprintf(yyoutput, "%s", $$); } <value>

%destructor { } <*>
%destructor { free($$); } <value>
%destructor { ast_free($$); } <node>

%type  <node>  program statements statement
%type  <node>  block declaration if while
%type  <node>  vardecl function function_args
%type  <node>  expr assign_expr call_expr call_args
%type  <node>  boolean_op arithmetic_comp arithmetic_op
%type  <node>  identifier

%token <value> BOOLEAN INTEGER FLOAT STRING IDENTIFIER

%token ASSIGN
%token IF ELSE
%token WHILE DO
%token RETURN BREAK CONTINUE

%token LBRACE RBRACE LPAREN RPAREN
%token SEMICOLON COMMA

%token NOT EQ AND IOR XOR       // boolean comparators
%token LT LE GE GT              // arithmetic comparators
%token ADD SUB MUL DIV POW MOD  // arithmetic operators

%right NOT
%nonassoc EQ NEQ
%left AND
%left IOR XOR

%nonassoc LT LE GE GT

%left MOD
%left ADD SUB
%left MUL DIV
%right POW
%right NEG
%left INC DEC

%start program

%%

program         : statements                    { $$ = $1; *ast = ast_clone($$); }
                ;

statements      : statement                     { $$ = ast_new(N_BLOCK); ast_append_child($$, $1); }
                | statements statement          { ast_append_child($1, $2); }
                ;

statement       : block
                | declaration
                | if
                | while
                | expr SEMICOLON
                | RETURN expr SEMICOLON         { $$ = ast_new(N_RETURN); ast_append_child($$, $2); }
                ;

block           : LBRACE statements RBRACE      { $$ = $2; }
                ;

declaration     : vardecl ASSIGN expr SEMICOLON         { $1->next = ast_new(N_ASSIGNMENT); ast_append_child_all($1->next, ast_clone($1->child->next), $3); }
                | vardecl SEMICOLON
                | function
                ;

if              : IF expr statement ELSE statement      { $$ = ast_new(N_IF); ast_append_child_all($$, $2, $3, $5); }
                | IF expr statement                     { $$ = ast_new(N_IF); ast_append_child_all($$, $2, $3); }
                ;

while           : WHILE expr statement                  { $$ = ast_new(N_WHILE); ast_append_child_all($$, $2, $3); }
                ;

function        : identifier identifier LPAREN function_args RPAREN block { $$ = ast_new(N_FUNCTION); ast_append_child_all($$, $1, $2, $4, $6); }
                ;

function_args   : /* empty */                   { $$ = ast_new(N_FUNCTIONARGS); }
                | vardecl                       { $$ = ast_new(N_FUNCTIONARGS); ast_append_child($$, $1); }
                | function_args COMMA vardecl   { ast_append_child($1, $3); }
                ;

vardecl         : identifier identifier                 { $$ = ast_new(N_DECLARATION); ast_append_child_all($$, $1, $2); }
                ;

expr            : LPAREN expr RPAREN            { $$ = $2; }
                | call_expr
                | assign_expr
                | identifier
                | NOT expr                      { $$ = ast_new(N_NOT); ast_append_child($$, $2); }
                | SUB expr %prec NEG            { $$ = ast_new(N_NEG); ast_append_child($$, $2); }
                | expr boolean_op expr          { $$ = $2; ast_append_child_all($$, $1, $3); }
                | expr arithmetic_comp expr     { $$ = $2; ast_append_child_all($$, $1, $3); }
                | expr arithmetic_op expr       { $$ = $2; ast_append_child_all($$, $1, $3); }
                | BOOLEAN                       { $$ = ast_new(N_BOOLEAN); $$->value = $1; }
                | INTEGER                       { $$ = ast_new(N_INTEGER); $$->value = $1; }
                | FLOAT                         { $$ = ast_new(N_FLOAT); $$->value = $1; }
                | STRING                        { $$ = ast_new(N_STRING); $$->value = $1; }
/*
                | INC identifier
                | DEC identifier
                | identifier INC
                | identifier DEC
*/
                ;

call_expr       : identifier LPAREN call_args RPAREN    { $$ = ast_new(N_CALL); ast_append_child_all($$, $1, $3); }
                ;

call_args       : /* empty */                           { $$ = ast_new(N_CALLARGS); }
                | expr                                  { $$ = ast_new(N_CALLARGS); ast_append_child($$, $1); }
                | call_args COMMA expr                  { ast_append_child($1, $3); }
                ;

assign_expr     : identifier ASSIGN expr                { $$ = ast_new(N_ASSIGNMENT); ast_append_child_all($$, $1, $3); }
                ;

identifier      : IDENTIFIER                    { $$ = ast_new(N_IDENTIFIER); $$->value = $1; }
                ;

boolean_op      : EQ    { $$ = ast_new(N_EQ); }
                | NEQ   { $$ = ast_new(N_NEQ); }
                | AND   { $$ = ast_new(N_AND); }
                | IOR   { $$ = ast_new(N_IOR); }
                | XOR   { $$ = ast_new(N_XOR); }
                ;
arithmetic_comp : LT    { $$ = ast_new(N_LT); }
                | LE    { $$ = ast_new(N_LE); }
                | GE    { $$ = ast_new(N_GE); }
                | GT    { $$ = ast_new(N_GT); }
                ;
arithmetic_op   : ADD   { $$ = ast_new(N_ADD); }
                | SUB   { $$ = ast_new(N_SUB); }
                | MUL   { $$ = ast_new(N_MUL); }
                | DIV   { $$ = ast_new(N_DIV); }
                | POW   { $$ = ast_new(N_POW); }
                | MOD   { $$ = ast_new(N_MOD); }
                ;

/* -- */

//relation        : domain 'x' domain '=' table            // { printf("Relation(%s, %s, %s)\n", $1, $3, $5); }
//                ;
//domain          : '{' domainElements '}'                 // { printf("Domain(%s)\n", $2); }
//                ;
//domainElements  : /* empty */
//                | domainElement domainElements
//                ;
//domainElement   : domain /* domains can be nested */
//                | identifier   /* [a-z] */
//                ;
//table           : '{' tableRows '}'                      // { printf("Table(%s)\n", $2); }
//                ;
//tableRows       : tableRow
//                | tableRow '|' '|' tableRows
//                ;
//tableRow        : binBool /* [01 ]* (can be empty)*/
//                ;
//
//binBool         :;

%%

