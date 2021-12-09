all: rixc

rix.tab.c rix.tab.h:	rix.y
	bison -d rix.y

lex.yy.c: rix.l rix.tab.h
	flex rix.l

rixc.rix.c rixc.rix.h: lex.yy.c rix.tab.c rix.tab.h
	${RIX_HOME}/rixc -q -o rixc.rix rixc.rix

rixc: rixc.rix.c rixc.rix.h
	sh ./rixc.rix.sh

clean:
	rm rixc rix.tab.c lex.yy.c rix.tab.h rixc.rix.c rixc.rix.h rixc.rix.c rixc.rix.h rixc.rix.sh rixc.rix.log
