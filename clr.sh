
if [ -f ritc ]
	then	
		echo Clearing: ritc --           preventing unexpected test results
		rm ritc
fi

if [ -f lex.yy.c ]
	then	
		echo Clearing: lex.yy.c --       flex output
		rm lex.yy.c
fi

if [ -f ritchie.tab.c ]
	then	
		echo Clearing: ritchie.tab.c --  bison output
		rm ritchie.tab.c
fi

if [ -f ritchie.tab.h ]
	then	
		echo Clearing: ritchie.tab.h --  bison output
		rm ritchie.tab.h
fi

if [ -f out.c ]
	then
		rm out.c
fi

if [ -f out.h ]
	then
		rm out.h
fi
