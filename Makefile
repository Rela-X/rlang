include config.mk

vpath %.h inc/
vpath %.c src/
vpath %.l src/
vpath %.y src/

INC     += -I ./
INC     += -I inc/

OBJ = main.o ast.o rlang.yy.o rlang.tab.o

.PHONY : all clean
.PHONY : test

TARGET = rlang

# disable implicit rules
.SUFFIXES:

all: $(TARGET)

$(TARGET) : $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(LIBPATH) $(LIB) $^
	
clean:
	rm -f $(OBJ) $(TARGET)
	rm -f *.tab.c *.tab.h
	rm -f *.yy.c

main.o : main.c 
	$(CC) $(CFLAGS) -c $(INC) -o $@ $<
	
%.yy.o : %.yy.c
	$(CC) $(CFLAGS) -c $(INC) -o $@ $<

%.o : %.c %.h
	$(CC) $(CFLAGS) -c $(INC) -o $@ $<
                
%.tab.c %.tab.h : %.y
	$(YACC) $(YFLAGS) -b $* $<

%.yy.c : %.l %.tab.h
	$(LEX) -o $@ $<

# test
test : rlang
	valgrind --quiet ./$< < test.rlang
