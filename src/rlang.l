%option noyywrap
%option bison-bridge

%{
#include <stdbool.h>

#include "ast.h"
#include "rlang.tab.h"

#define TOKEN(t) yylval->token = token_new(t, yytext); return t

// http://flex.sourceforge.net/manual/Common-Patterns.html#Common-Patterns
%}

c99_decimal	0|(([[:digit:]]{-}[0])[[:digit:]]*)
c99_hex		0[xX][[:xdigit:]]+
c99_octal	0[oO][01234567]+
c99_integer	({c99_decimal}|{c99_hex}|{c99_octal})

dseq		([[:digit:]]+)
dseq_opt	([[:digit:]]*)
frac		(({dseq_opt}"."{dseq})|{dseq}".")
exp		([eE][+-]?{dseq})
exp_opt		({exp}?)
fsuff		[flFL]
fsuff_opt	({fsuff}?)
hpref		(0[xX])
hdseq		([[:xdigit:]]+)
hdseq_opt	([[:xdigit:]]*)
hfrac		(({hdseq_opt}"."{hdseq})|({hdseq}"."))
bexp		([pP][+-]?{dseq})
dfc		(({frac}{exp_opt}{fsuff_opt})|({dseq}{exp}{fsuff_opt}))
hfc		(({hpref}{hfrac}{bexp}{fsuff_opt})|({hpref}{hdseq}{bexp}{fsuff_opt}))
c99_floating_point	({dfc}|{hfc})

ucn  		((\\u([[:xdigit:]]{4}))|(\\U([[:xdigit:]]{8})))
nondigit	[_[:alpha:]]
c99_id		([_[:alpha:]]|{ucn})([_[:alnum:]]|{ucn})*

%x COMMENT
%x BLOCKCOMMENT

%%

[ \t\r\n]+	{ }

":="		{ TOKEN(ASSIGN); }
"if"		{ TOKEN(IF); }
"else"		{ TOKEN(ELSE); }
"while"		{ TOKEN(WHILE); }
"do"		{ TOKEN(DO); }

"!"		{ TOKEN(NOT); }
"=="		{ TOKEN(EQ); }
"!="		{ TOKEN(NEQ); }
"&&"		{ TOKEN(AND); }
"||"		{ TOKEN(IOR); }
"^^"		{ TOKEN(XOR); } // TODO !=

"<"		{ TOKEN(LT); }
"<="		{ TOKEN(LE); }
">="		{ TOKEN(LE); }
">"		{ TOKEN(LT); }

"++"		{ TOKEN(INC); }
"--"		{ TOKEN(DEC); }

"+"		{ TOKEN(ADD); }
"-"		{ TOKEN(SUB); }
"*"		{ TOKEN(MUL); }
"/"		{ TOKEN(DIV); }

"**"		{ TOKEN(POW); }
"%"		{ TOKEN(MOD); }

"{"		{ TOKEN(LBRACE); }
"}"		{ TOKEN(RBRACE); }
"("		{ TOKEN(LPAREN); }
")"		{ TOKEN(RPAREN); }

";"		{ TOKEN(SEMICOLON); }

"true"|"false"		{ TOKEN(BOOLEAN); }
{c99_integer}		{ TOKEN(INTEGER); }
{c99_floating_point}	{ TOKEN(FLOAT); }
{c99_id}		{ TOKEN(IDENTIFIER); }

.		{ printf("UNHANDLED TOKEN \"%s\"\n", yytext); }

"//"		{ BEGIN(COMMENT); }
<COMMENT>\n	{ BEGIN(INITIAL); }
<COMMENT>.	{ }
"/*"			{ BEGIN(BLOCKCOMMENT); }
<BLOCKCOMMENT>"*/"	{ BEGIN(INITIAL); }
<BLOCKCOMMENT>.		{ }

%%

