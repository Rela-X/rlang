%option bison-bridge
%option bison-locations
%option yylineno
%option noyywrap
%option stack

%{
#include <stdbool.h>

#include "ast.h"
#include "rlang.tab.h"

static inline
void yylloc_update(YYLTYPE *yylloc) {
	yylloc->first_line   = yylloc->last_line;
	yylloc->first_column = yylloc->last_column;
	yylloc->last_line    = yylineno;
	yylloc->last_column  = (yylloc->first_line == yylloc->last_line) ? yylloc->last_column + yyleng : 1;
}

// YY_USER_ACTION requires ';'
#define YY_USER_ACTION do { yylloc_update(yylloc); } while(0);

#define TOKEN(t) do { return t; } while(0)
#define VALUE(v) do { yylval->value = strndup(yytext, yyleng); return v; } while(0)

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

%x STR
%x RTABLE

%x LINECOMMENT
%x BLOCKCOMMENT
%%

[[:space:]]+	{ }

":="		{ TOKEN(ASSIGN); }
"="		{ TOKEN(ASSIGN); }
"if"		{ TOKEN(IF); }
"else"		{ TOKEN(ELSE); }
"while"		{ TOKEN(WHILE); }
"do"		{ TOKEN(DO); }
"return"	{ TOKEN(RETURN); }

"!"		{ TOKEN(NOT); }
"=="		{ TOKEN(EQ); }
"!="		{ TOKEN(NEQ); }
"&&"		{ TOKEN(AND); }
"||"		{ TOKEN(IOR); }
"^^"		{ TOKEN(XOR); } // TODO !=

"<"		{ TOKEN(LT); }
"<="		{ TOKEN(LE); }
">="		{ TOKEN(GE); }
">"		{ TOKEN(GT); }

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

":"		{ TOKEN(COLON); }
";"		{ TOKEN(SEMICOLON); }
","		{ TOKEN(COMMA); }

"true"			{ VALUE(BOOLEAN); }
"false"			{ VALUE(BOOLEAN); }
{c99_integer}		{ VALUE(INTEGER); }
{c99_floating_point}	{ VALUE(FLOAT); }
{c99_id}		{ VALUE(IDENTIFIER); }

\"\"			{ yylval->value = strdup(""); return STRING; }
<INITIAL>["]		{ yy_push_state(STR); }
<STR>{
	["]		{ yy_pop_state(); }
	[^"]*		{ VALUE(STRING); }
}

<INITIAL>"//"		{ yy_push_state(LINECOMMENT); }
<LINECOMMENT>{
	\n		{ yy_pop_state(); }
	.*		{ }
}

<*>"/*"			{ yy_push_state(BLOCKCOMMENT); }
<BLOCKCOMMENT>{
	"*/"		{ yy_pop_state(); }
	[^*\n]+		{ }
	"*"[^/]		{ }
	\n		{ }
}

<INITIAL>"["		{ yy_push_state(RTABLE); TOKEN(LBRACKET); }
<RTABLE>{
	"]"		{ yy_pop_state(); TOKEN(RBRACKET); }
	"0"		{ VALUE(BOOLEAN); }
	"1"		{ VALUE(BOOLEAN); }
	"|"		{ TOKEN(VBAR); }
	[[:space:]]*	{ }
}

.	{ printf("UNHANDLED TOKEN \"%s\"\n", yytext); }

%%

