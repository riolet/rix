# Makefile example -- scanner and parser.
# Creates "myprogram" from "scan.l", "parse.y", and "myprogram.c"
#
LEX        = flex
YACC       = bison -y
YFLAGS     = -d

all: ritc

ritchie.tab.c ritchie.tab.h:	ritchie.y
	bison -d ritchie.y

lex.yy.c: ritchie.l ritchie.tab.h
	flex ritchie.l

ritc: lex.yy.c ritchie.tab.c ritchie.tab.h
	gcc preproc.c ObjectTree.c ritchie.tab.c lex.yy.c ritc.c errors.c -lfl -o ritc

clean:
	rm ritc ritchie.tab.c lex.yy.c ritchie.tab.h
