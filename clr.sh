
if [ -f rixc ]
	then	
		echo Clearing: rixc --           preventing unexpected test results
		rm rixc
fi

if [ -f lex.yy.c ]
	then	
		echo Clearing: lex.yy.c --       flex output
		rm lex.yy.c
fi

if [ -f rix.tab.c ]
	then	
		echo Clearing: rix.tab.c --  bison output
		rm rix.tab.c
fi

if [ -f rix.tab.h ]
	then	
		echo Clearing: rix.tab.h --  bison output
		rm rix.tab.h
fi

if [ -f out.c ]
	then
		rm out.c
fi

if [ -f out.h ]
	then
		rm out.h
fi
