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
/*
%printer { fprintf (yyoutput, "'%c'", $$); } <character>
%printer { fprintf (yyoutput, "&%p", $$); } <*>
%printer { fprintf (yyoutput, "\"%s\"", $$); } STRING1 string1
%printer { fprintf (yyoutput, "<>"); } <>
*/

%destructor { } <*>
%destructor { free($$); } <value>
%destructor { ast_free($$); } <node>

%type  <node>  program statements statement declarestmt ifstmt whilestmt expr assign_expr
%type  <node>  identifier
%type  <node>  boolean_op arithmetic_comp arithmetic_op

%token <value> BOOLEAN INTEGER FLOAT STRING IDENTIFIER

%token ASSIGN
%token IF ELSE
%token WHILE DO

%token LBRACE RBRACE LPAREN RPAREN
%token SEMICOLON

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

program         : statements                    { $$ = $1; *ast = ast_copy($$); }
                ;

statements      : statements statement          { ast_append_child($1, $2); }
                | statement                     { $$ = ast_new(N_BLOCK); ast_append_child($$, $1); }
                ;

statement       : LBRACE statements RBRACE      { $$ = $2; }
                | declarestmt SEMICOLON
                | ifstmt 
                | whilestmt 
                | expr SEMICOLON
                ;

declarestmt     : identifier identifier ASSIGN expr     { $$ = ast_new(N_DECLARESTMT); ast_append_child_all($$, $1, $2, $4); }
                | identifier identifier                 { $$ = ast_new(N_DECLARESTMT); ast_append_child_all($$, $1, $2); }
                ;

ifstmt          : IF expr statement ELSE statement      { $$ = ast_new(N_IFSTMT); ast_append_child_all($$, $2, $3, $5); }
                | IF expr statement                     { $$ = ast_new(N_IFSTMT); ast_append_child_all($$, $2, $3); }
                ;

whilestmt       : WHILE expr statement                  { $$ = ast_new(N_WHILESTMT); ast_append_child_all($$, $2, $3); }
                ;

expr            : LPAREN expr RPAREN            { $$ = $2; }
                | assign_expr
                | call_expr
                | NOT expr                      { $$ = ast_new(N_NOT); ast_append_child($$, $2); }
                | SUB expr %prec NEG            { $$ = ast_new(N_NEG); ast_append_child($$, $2); }
                | expr boolean_op expr          { $$ = $2; ast_append_child_all($$, $1, $3); }
                | expr arithmetic_comp expr     { $$ = $2; ast_append_child_all($$, $1, $3); }
                | expr arithmetic_op expr       { $$ = $2; ast_append_child_all($$, $1, $3); }
                | identifier
                | BOOLEAN                       { $$ = ast_new(N_BOOLEAN); $$->value = $1; } // TODO Rule unused
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

assign_expr     : identifier ASSIGN expr        { $$ = ast_new(N_ASSIGNMENT); ast_append_child_all($$, $1, $3); }
                ;

call_expr       : identifier LPAREN call_args RPAREN
                ;

call_args       : /* empty */
                | expr "," call_args
                | expr
                ;

boolean_op      : EQ    { $$ = ast_new(N_EQ); }
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

identifier      : IDENTIFIER                    { $$ = ast_new(N_IDENTIFIER); $$->value = $1; }

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

