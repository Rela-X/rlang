CC      = clang
CFLAGS  = -std=c11 -Os -Wall -pedantic
MAKE    = make
LEX     = flex
YACC    = bison --yacc
YFLAGS  = -d -t

INC     = -I ../relafix/inc/
LIBPATH = -L ../relafix/
