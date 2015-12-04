# Makefile example -- scanner and parser.
# Creates "myprogram" from "scan.l", "parse.y", and "myprogram.c"
#
LEX        = flex
YACC       = bison -y
YFLAGS     = -d
CFLAGS     = -lfl
objects    = scan.o parse.o ritc.o preproc.o ObjectTree.o ritchie.tab.o errors.o

ritc: $(objects) $(CFLAGS)
scan.o: ritchie.l
parse.o: ritchie.y
preproc.o: preproc.c
ObjectTree.o: ObjectTree.c
ritchie.tab.o: ritchie.tab.c
errors.o: errors.c
ritc.o: ritc.c

clean:
	rm -f *.o
