%option noyywrap

%{
#include <stdbool.h>
#include <string.h>
#include "rlang.tab.h"

// http://flex.sourceforge.net/manual/Common-Patterns.html#Common-Patterns
%}

DIGIT	[0-9]
ID	[_a-zA-Z][_a-zA-Z0-9]*

%%

[ \t\r\n]+	{ }

":="            { return ASSIGN; }
"if"            { return IF; }
"else"          { return ELSE; }
"while"         { return WHILE; }
"do"            { return DO; }

"!"		{ return NOT; }
"=="		{ return EQ; }
"!="		{ return NEQ; }
"&&"		{ return AND; }
"||"		{ return IOR; }
"^^"		{ return XOR; } // TODO !=

"<"		{ return LT; }
"<="		{ return LE; }
">="		{ return LE; }
">"		{ return LT; }

"++"		{ return INC; }
"--"		{ return DEC; }

"+"		{ return ADD; }
"-"		{ return SUB; }
"*"		{ return MUL; }
"/"		{ return DIV; }

"**"		{ return POW; }
"%"		{ return MOD; }

"{"		{ return LBRACE; }
"}"		{ return RBRACE; }
"("		{ return LPAREN; }
")"		{ return RPAREN; }

";"		{ return SEMICOLON; }

"true"		{ yylval.bval = true; return BOOLEAN; }
"false"		{ yylval.bval = false; return BOOLEAN; }

{DIGIT}+                        { yylval.ival = atoi(yytext); return INTEGER; }
{DIGIT}+"."{DIGIT}*             { yylval.fval = atof(yytext); return FLOAT; }
{DIGIT}+[eE]["+""-"]?{DIGIT}*   { yylval.fval = atof(yytext); return FLOAT; }

{ID}		{ yylval.str = strdup(yytext); return IDENTIFIER; }

.		{ }

%%

