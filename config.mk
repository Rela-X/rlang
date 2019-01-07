MAKE = make

CC       = clang
CPPFLAGS = -I ./ -I ../relafix/
CFLAGS   = -std=c11 -Os -Wall -pedantic
LDFLAGS  = -L ../relafix/

LEX    = flex
YACC   = bison --yacc
YFLAGS = -d -t
