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

#include "rlang.h"

#define YYERROR_VERBOSE 1
#define YYDEBUG 0

extern int yylex();

void
yyerror(char *yymsg) {
	printf("my_yyerror: %s\n", yymsg);
}

%}

%locations

%union {
        bool    bval;
        int     ival;
        double  fval;
        char    *str;
        /*
        rf_Domain *domain;
        rf_Relation *relation;
        */
}

%destructor { } <*>
%destructor { free($$); } <str>
/*
%destructor { rf_set_free($$); } <domain>
%destructor { rf_relation_free($$); } <relation>
*/
%destructor { } <>

%token <bval>   BOOLEAN
%token <ival>   INTEGER
%token <fval>   FLOAT
%token <str>    IDENTIFIER

%token ASSIGN
%token IF
%token ELSE
%token WHILE
%token DO

%token LBRACE
%token RBRACE
%token LPAREN
%token RPAREN

%token SEMICOLON

%right NOT
%nonassoc EQ NEQ
%left AND
%left IOR XOR

%nonassoc LT LE GE GT

%left MOD
%left ADD SUB
%left MUL DIV
%right POW
%left INC DEC

%type <bval>    boolexpr
%type <ival>    arithmexpr

%start program

%%

program         : statements
                ;

statements      : statement statements
                | statement
                ;

statement       : assignstmt SEMICOLON
                | callstmt SEMICOLON
                | ifstmt 
                | whilestmt 
                | stmtblock
                | expr SEMICOLON
                ;

assignstmt      : IDENTIFIER IDENTIFIER ASSIGN expr
                | IDENTIFIER ASSIGN expr
                ;

callstmt        : IDENTIFIER LPAREN params RPAREN
                ;

ifstmt          : IF boolexpr statement ELSE statement
                | IF boolexpr statement
                ;

whilestmt       : WHILE boolexpr statement
                | DO statement WHILE boolexpr
                ;

stmtblock       : LBRACE statements RBRACE
                ;

expr            : boolexpr 
                | arithmexpr
                ;

boolexpr	: LPAREN boolexpr RPAREN        { $$ = $2; }
                | NOT boolexpr                  { $$ = reduce_not($2,&@2); }
                | boolexpr EQ boolexpr          { $$ = reduce_eq($1,$3,&@3); }
/*                | boolexpr NEQ boolexpr         { $$ = reduce_($1,$3); } */
                | boolexpr AND boolexpr         { $$ = reduce_and($1,$3,&@3); }
                | boolexpr IOR boolexpr         { $$ = reduce_ior($1,$3,&@3); }
                | boolexpr XOR boolexpr         { $$ = reduce_xor($1,$3,&@3); } // TODO
                | arithmexpr EQ arithmexpr      { $$ = reduce_eq2($1,$3,&@3); }
/*                | arithmexpr NEQ arithmexpr     { $$ = reduce_($1,$3,&@3); } */
                | arithmexpr LT arithmexpr      { $$ = reduce_lt($1,$3,&@3); }
                | arithmexpr LE arithmexpr      { $$ = reduce_le($1,$3,&@3); }
                | arithmexpr GE arithmexpr      { $$ = reduce_ge($1,$3,&@3); }
                | arithmexpr GT arithmexpr      { $$ = reduce_gt($1,$3,&@3); }
                | IDENTIFIER
                | BOOLEAN                       { $$ = $1; }
                ;

arithmexpr	: LPAREN arithmexpr RPAREN      { $$ = $2; }
/*                | NEG arithmexpr                { $$ = reduce_neg($2); } */
                | arithmexpr ADD arithmexpr     { $$ = reduce_add($1,$3,&@3); }
                | arithmexpr SUB arithmexpr     { $$ = reduce_sub($1,$3,&@3); }
                | arithmexpr MUL arithmexpr     { $$ = reduce_mul($1,$3,&@3); }
                | arithmexpr DIV arithmexpr     { $$ = reduce_div($1,$3,&@3); }
                | arithmexpr POW arithmexpr     { $$ = reduce_pow($1,$3,&@3); }
                | arithmexpr MOD arithmexpr     { $$ = reduce_mod($1,$3,&@3); }
                | INC IDENTIFIER
                | DEC IDENTIFIER
                | IDENTIFIER INC
                | IDENTIFIER DEC
                | IDENTIFIER
                | INTEGER                       { $$ = $1; }
                | FLOAT                         { $$ = $1; }
                ;

relexpr         :
                ;

params          :
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

