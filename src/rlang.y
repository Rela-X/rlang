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

extern int yylex();

void
yyerror(const char *yymsg) {
	printf("my_yyerror: %s\n", yymsg);
}

Ast *ast;

%}

%locations

%union {
        Ast *node;
        Token *token;
}

%destructor { } <*>
%destructor { ast_free($$); } <node>
%destructor { token_free($$); } <token>

%type  <node>  program statements statement declarestmt ifstmt whilestmt expr assign_expr
%type  <token> boolean_op arithmetic_comp arithmetic_op

%token <token> ASSIGN
%token <token> IF ELSE
%token <token> WHILE DO

%token <token> LBRACE RBRACE LPAREN RPAREN
%token <token> SEMICOLON

%token <token> NOT EQ AND IOR XOR       // boolean comparators
%token <token> LT LE GE GT              // arithmetic comparators
%token <token> ADD SUB MUL DIV POW MOD  // arithmetic operators

%token <token> BOOLEAN INTEGER FLOAT STRING IDENTIFIER

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

program         : statements                    { ast = $$ = ast_new(NULL); ast_append_child($$, $1); }
                ;

statements      : statement statements          { ast_append_child($1, $2); }
                | statement
                ;

statement       : LBRACE statements RBRACE      { $$ = $2; }
                | declarestmt SEMICOLON
                | ifstmt 
                | whilestmt 
                | expr SEMICOLON
                ;

declarestmt     : IDENTIFIER IDENTIFIER ASSIGN expr     { $$ = ast_new($3); ast_append_child_all($$, ast_new($1), ast_new($2), $4); }
                | IDENTIFIER IDENTIFIER
                ;

ifstmt          : IF expr statement ELSE statement      { $$ = ast_new($1); ast_append_child_all($$, $2, $3, $5); }
                | IF expr statement                     { $$ = ast_new($1); ast_append_child_all($$, $2, $3); }
                ;

whilestmt       : WHILE expr statement                  { $$ = ast_new($1); ast_append_child_all($$, $2, $3); }
                ;

expr            : LPAREN expr RPAREN            { $$ = $2; }
                | assign_expr
                | call_expr
                | NOT expr                      { $$ = ast_new($1); ast_append_child($$, $2); }
                | SUB expr %prec NEG            { $$ = ast_new($1); ast_append_child($$, $2); }
                | expr boolean_op expr          { $$ = ast_new($2); ast_append_child_all($$, $1, $3); }
                | expr arithmetic_comp expr     { $$ = ast_new($2); ast_append_child_all($$, $1, $3); }
                | expr arithmetic_op expr       { $$ = ast_new($2); ast_append_child_all($$, $1, $3); }
                | IDENTIFIER                    { $$ = ast_new($1); }
                | BOOLEAN                       { $$ = ast_new($1); }
                | INTEGER                       { $$ = ast_new($1); }
                | FLOAT                         { $$ = ast_new($1); }
                | STRING                        { $$ = ast_new($1); }
/*
                | INC IDENTIFIER
                | DEC IDENTIFIER
                | IDENTIFIER INC
                | IDENTIFIER DEC
*/
                ;

assign_expr     : IDENTIFIER ASSIGN expr        { $$ = ast_new($2); ast_append_child_all($$, ast_new($1), $3); }
                ;

call_expr       : IDENTIFIER LPAREN call_args RPAREN
                ;

call_args       : /* empty */
                | expr "," call_args
                | expr
                ;

boolean_op      : EQ | AND | IOR | XOR
                ;
arithmetic_comp : LT | LE | GE | GT
                ;
arithmetic_op   : ADD | SUB | MUL | DIV | POW | MOD
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

