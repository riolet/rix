all: rixc

rix.tab.c rix.tab.h:	rix.y
	bison -d rix.y

lex.yy.c: rix.l rix.tab.h
	flex rix.l

rixc.rix.c rixc.rix.h: lex.yy.c rix.tab.c rix.tab.h
	${RIX_HOME}/rixc -L -q -o rixc.rix rixc.rix

rixc: rixc.rix.c rixc.rix.h
	gcc -ggdb preproc.c ObjectTree.c rix.tab.c lex.yy.c rixc.c crsl.c -lfl -o rixc -Wno-implicit-function-declaration -I ${RIX_HOME}  rixc.rix.c rixc.rix_ext.c ${RIX_HOME}/rsl/rsl.c ${RIX_HOME}/errors.c ${RIX_HOME}/"rsl/RSL_Array.c" ${RIX_HOME}/"rsl/RSL_String.c" ${RIX_HOME}/"rsl/RSL_Stream.c"  ${RIX_HOME}/"ext/List/List.c" ${RIX_HOME}/"ext/List/cliblist.c" ${RIX_HOME}/"ext/List/list_node.c" ${RIX_HOME}/"ext/List/list_iterator.c"  -lm

clean:
	rm rixc rix.tab.c lex.yy.c rix.tab.h rixc.rix.c rixc.rix.h
