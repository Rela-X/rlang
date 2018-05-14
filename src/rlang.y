%{
struct _ast;

#include "rlang.tab.h"

#include <stdio.h>

#include "ast.h"
#include "print.h"

#define YYERROR_VERBOSE 1
#define YYDEBUG 1

int yylex(YYSTYPE *, YYLTYPE *);

void
yyerror(YYLTYPE *yyllocp, struct _ast **ast, const char *yymsg) {
	err_at(yyllocp, yymsg);
}

%}

%locations
%define api.pure
%parse-param {struct _ast **ast}

%union {
        struct _ast *node;
        char *value;
}

%printer { dprint_node(fileno(yyoutput), $$); } <node>
%printer { dprintf(fileno(yyoutput), "%s", $$); } <value>

%destructor { free($$); } <value>
%destructor { ast_free($$); } <node>

%type  <node>  program statements statement
%type  <node>  block declaration if while
%type  <node>  vardecl function function_args
%type  <node>  expr assign_expr call_expr call_args
%type  <node>  boolean_op arithmetic_comp arithmetic_op
%type  <node>  identifier value
%type  <node>  set set_elements set_element
%type  <node>  relation rtable rtable_rows rtable_row

%token <value> IDENTIFIER
%token <value> BOOLEAN INTEGER FLOAT STRING

%token ASSIGN
%token IF ELSE
%token WHILE DO
%token RETURN BREAK CONTINUE

%token LBRACE RBRACE LPAREN RPAREN LBRACKET RBRACKET
%token COLON SEMICOLON COMMA VBAR

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

statements      : statement                     { $$ = ast_new(N_BLOCK); $$->location = @$; ast_append_child($$, $1); }
                | statements statement          { ast_append_child($1, $2); }
                ;

statement       : block
                | declaration
                | if
                | while
                | expr SEMICOLON
                | RETURN expr SEMICOLON         { $$ = ast_new(N_RETURN); $$->location = @$; ast_append_child($$, $2); }
                ;

block           : LBRACE statements RBRACE      { $$ = $2; }
                ;

declaration     : vardecl ASSIGN expr SEMICOLON         { $1->next = ast_new(N_ASSIGNMENT); $$->location = @$; ast_append_child_all($1->next, ast_clone($1->child->next), $3); }
                | vardecl SEMICOLON
                | function
                ;

if              : IF expr statement ELSE statement      { $$ = ast_new(N_IF); $$->location = @$; ast_append_child_all($$, $2, $3, $5); }
                | IF expr statement                     { $$ = ast_new(N_IF); $$->location = @$; ast_append_child_all($$, $2, $3); }
                ;

while           : WHILE expr statement                  { $$ = ast_new(N_WHILE); $$->location = @$; ast_append_child_all($$, $2, $3); }
                ;

function        : identifier identifier LPAREN function_args RPAREN block { $$ = ast_new(N_FUNCTION); $$->location = @$; ast_append_child_all($$, $1, $2, $4, $6); }
                ;

function_args   : /* empty */                   { $$ = ast_new(N_FUNCTIONARGS); $$->location = @$; }
                | vardecl                       { $$ = ast_new(N_FUNCTIONARGS); $$->location = @$; ast_append_child($$, $1); }
                | function_args COMMA vardecl   { ast_append_child($1, $3); }
                ;

vardecl         : identifier identifier                 { $$ = ast_new(N_DECLARATION); $$->location = @$; ast_append_child_all($$, $1, $2); }
                ;

expr            : LPAREN expr RPAREN            { $$ = $2; }
                | call_expr
                | assign_expr
                | identifier
                | NOT expr                      { $$ = ast_new(N_NOT); $$->location = @$; ast_append_child($$, $2); }
                | SUB expr %prec NEG            { $$ = ast_new(N_NEG); $$->location = @$; ast_append_child($$, $2); }
                | expr boolean_op expr          { $$ = $2; ast_append_child_all($$, $1, $3); }
                | expr arithmetic_comp expr     { $$ = $2; ast_append_child_all($$, $1, $3); }
                | expr arithmetic_op expr       { $$ = $2; ast_append_child_all($$, $1, $3); }
                | value
                | set
                | relation
                ;

call_expr       : identifier LPAREN call_args RPAREN    { $$ = ast_new(N_CALL); $$->location = @$; ast_append_child_all($$, $1, $3); }
                ;

call_args       : /* empty */                           { $$ = ast_new(N_CALLARGS); $$->location = @$; }
                | expr                                  { $$ = ast_new(N_CALLARGS); $$->location = @$; ast_append_child($$, $1); }
                | call_args COMMA expr                  { ast_append_child($1, $3); }
                ;

assign_expr     : identifier ASSIGN expr                { $$ = ast_new(N_ASSIGNMENT); $$->location = @$; ast_append_child_all($$, $1, $3); }
                ;

identifier      : IDENTIFIER                    { $$ = ast_new(N_IDENTIFIER); $$->location = @$; $$->value = $1; }
                ;

boolean_op      : EQ    { $$ = ast_new(N_EQ); $$->location = @$; }
                | NEQ   { $$ = ast_new(N_NEQ); $$->location = @$; }
                | AND   { $$ = ast_new(N_AND); $$->location = @$; }
                | IOR   { $$ = ast_new(N_IOR); $$->location = @$; }
                | XOR   { $$ = ast_new(N_XOR); $$->location = @$; }
                ;
arithmetic_comp : LT    { $$ = ast_new(N_LT); $$->location = @$; }
                | LE    { $$ = ast_new(N_LE); $$->location = @$; }
                | GE    { $$ = ast_new(N_GE); $$->location = @$; }
                | GT    { $$ = ast_new(N_GT); $$->location = @$; }
                ;
arithmetic_op   : ADD   { $$ = ast_new(N_ADD); $$->location = @$; }
                | SUB   { $$ = ast_new(N_SUB); $$->location = @$; }
                | MUL   { $$ = ast_new(N_MUL); $$->location = @$; }
                | DIV   { $$ = ast_new(N_DIV); $$->location = @$; }
                | POW   { $$ = ast_new(N_POW); $$->location = @$; }
                | MOD   { $$ = ast_new(N_MOD); $$->location = @$; }
                ;

value           : BOOLEAN                       { $$ = ast_new(N_BOOLEAN); $$->location = @$; $$->value = $1; }
                | INTEGER                       { $$ = ast_new(N_INTEGER); $$->location = @$; $$->value = $1; }
                | FLOAT                         { $$ = ast_new(N_FLOAT); $$->location = @$; $$->value = $1; }
                | STRING                        { $$ = ast_new(N_STRING); $$->location = @$; $$->value = $1; }
                ;

relation        : expr IDENTIFIER expr COLON rtable       { if($2[0] != 'x' || $2[1] != '\0') YYABORT; free($2); $$ = ast_new(N_R); $$->location = @$; ast_append_child_all($$, $1, $3, $5); }
                ;

set             : LBRACE set_elements RBRACE    { $$ = $2; }
                ;

set_elements    : /* empty */                   { $$ = ast_new(N_SET); $$->location = @$; }
                | set_elements set_element      { ast_append_child($1, $2); }
                ;

set_element     : value                         { $1->class = N_STRING; }
                | identifier                    { $1->class = N_STRING; }
                | set
                ;

rtable          : LBRACKET rtable_rows RBRACKET         { $$ = $2; }
                ;

rtable_rows     : rtable_row                            { $$ = ast_new(N_RTABLE); $$->location = @$; ast_append_child($$, $1); }
                | rtable_rows VBAR VBAR rtable_row      { ast_append_child($1, $4); }
                ;

rtable_row      : value                                 { $$ = ast_new(N_RTABLEROW); $$->location = @$; ast_append_child($$, $1); }
                | rtable_row value                      { ast_append_child($1, $2); }
                ;

%%
