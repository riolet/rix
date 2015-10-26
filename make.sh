bash clr.sh
echo Clear complete.
bash comp.sh
echo -----------------------------------------------------------------
echo beginning compilation of ritchie output file
echo -----------------------------------------------------------------
gcc ritchie.tab.c lex.yy.c ritc.c errors.c -lfl -o ritc

