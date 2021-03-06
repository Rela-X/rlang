include config.mk

vpath %.h src/
vpath %.c src/
vpath %.l src/
vpath %.y src/

CFLAGS  += -D_POSIX_C_SOURCE=200809L

CPPFLAGS     += -I ./
CPPFLAGS     += -I src/

LDLIBS  += -lm -lrelafix

TREE_WALKER_OBJ = scope_annotator.o symbol_annotator.o type_annotator.o type_validator.o executor.o tree_cleaner.o
PARSER_OBJ = rlang.yy.o rlang.tab.o
OBJ = $(PARSER_OBJ) $(TREE_WALKER_OBJ) main.o ast.o builtins.o memory.o print.o scope.o symbol.o value.o

.PHONY : all clean
.PHONY : test

TARGET = rlang

# disable implicit rules
.SUFFIXES:

all: $(TARGET)

$(TARGET) : $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(LDFLAGS) $^ $(LDLIBS)

clean:
	rm -f $(OBJ) $(TARGET)
	rm -f *.tab.c *.tab.h
	rm -f *.yy.c

main.o : main.c 
	$(CC) $(CFLAGS) -c $(CPPFLAGS) -o $@ $<

%.yy.o : %.yy.c
	$(CC) $(CFLAGS) -c $(CPPFLAGS) -o $@ $<

%.o : %.c %.h
	$(CC) $(CFLAGS) -c $(CPPFLAGS) -o $@ $<

$(TREE_WALKER_OBJ) : %.o : %.c
	$(CC) $(CFLAGS) -c $(CPPFLAGS) -o $@ $<

ast.h print.h : rlang.tab.h

%.tab.c %.tab.h : %.y
	$(YACC) $(YFLAGS) -b $* $<

%.yy.c : %.l %.tab.h
	$(LEX) -o $@ $<

# test
test : rlang
	LD_LIBRARY_PATH="$$(test/get_ld_library_path $(LDFLAGS))" ./rlang < test/unittests.rlang
