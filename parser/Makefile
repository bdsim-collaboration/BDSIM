CC=g++
#CC=gcc32
OBJ=gmad.o lex.yy.o parser.tab.o 
LIB=-lfl

#LEX=./flex-2.5.4/flex
LEX=flex
LOPT=-i

#YACC=./bison-2.3/src/bison
YACC=bison
YOPT=-d -v

#CPPFLAGS=-DDEBUG

OS        := $(shell uname)

ifeq ($(OS),Linux)
LIBEXT=so
endif
ifeq ($(OS),Darwin)
LIBEXT=dylib
endif
LIBNAME=libgmad.$(LIBEXT)

CPPFLAGS += -Wno-write-strings

ifeq ($(OS),Darwin)
LDFLAGS=-O2 -dynamiclib -undefined error
endif

ifeq ($(OS),Linux)
LDFLAGS=-W1 -shared -g 
endif

#parsing library

$(LIBNAME) : $(OBJ)
	$(CC) $(LDFLAGS) -o $(LIBNAME) $(OBJ) $(LIB)


# gmad parser

parser : parser.cc gmad.h getEnv.h $(LIBNAME)
	$(CC) parser.cc -g -o parser -L. -lgmad -lgetEnv


gmad.o :gmad.cc gmad.h
	$(CC) -Wall -fPIC -g -c gmad.cc

getEnv.o :getEnv.cc getEnv.h
	$(CC) -Wall -fPIC -g -c getEnv.cc

lex.yy.o : lex.yy.c parser.tab.c parser.tab.h sym_table.h
	$(CC) -Wall -fPIC -g -c  lex.yy.c

parser.tab.o : parser.tab.h parser.tab.c sym_table.h 
	$(CC) -Wall -fPIC -g -c $(INCL)  parser.tab.c

lex.yy.c : parser.l parser.tab.h parser.y parser.h
	$(LEX) $(LOPT) parser.l

parser.tab.h parser.tab.c : parser.y parser.h
	$(YACC) $(YOPT) parser.y

clean :
	rm -f parser $(LIBNAME) $(OBJ) lex.yy.c parser.tab.c parser.tab.h *~

cleandist :
	@echo "Cleaning up gmad parser temporary files..."
	@rm -f $(OBJ) lex.yy.c parser.tab.c parser.tab.h parser.output *~
