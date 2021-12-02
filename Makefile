all: rixc

rix.tab.c rix.tab.h:	rix.y
	bison -d rix.y

lex.yy.c: rix.l rix.tab.h
	flex rix.l

rixc: lex.yy.c rix.tab.c rix.tab.h
	gcc -ggdb preproc.c ObjectTree.c rix.tab.c lex.yy.c rixc.c errors.c crsl.c ext/List/cliblist.c ext/List/list_node.c ext/List/list_iterator.c -lfl -o rixc

clean:
	rm rixc rix.tab.c lex.yy.c rix.tab.h
