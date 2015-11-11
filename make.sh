bash clr.sh
echo Clear complete.
bash comp.sh
echo -----------------------------------------------------------------
echo beginning compilation of ritchie output file
echo -----------------------------------------------------------------
#gcc -W -Wall -pedantic -std=c99 ObjectTree.c ritchie.tab.c lex.yy.c ritc.c errors.c -lfl -o ritc
gcc ObjectTree.c ritchie.tab.c lex.yy.c ritc.c errors.c -lfl -o ritc

