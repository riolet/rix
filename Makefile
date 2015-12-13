all: ritc

ritchie.tab.c ritchie.tab.h:	ritchie.y
	bison -d ritchie.y

lex.yy.c: ritchie.l ritchie.tab.h
	flex ritchie.l

ritc: lex.yy.c ritchie.tab.c ritchie.tab.h
	gcc preproc.c ObjectTree.c ritchie.tab.c lex.yy.c ritc.c errors.c crsl.c -lfl -o ritc

clean:
	rm ritc ritchie.tab.c lex.yy.c ritchie.tab.h
