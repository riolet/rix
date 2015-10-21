
if [ -f snazzle ]
	then	
		echo Clearing: snazzle -- preventing unexpected test results
		rm snazzle
fi

if [ -f lex.yy.c ]
	then	
		echo Clearing: lex.yy.c -- flex output
		rm lex.yy.c
fi

if [ -f snazzle.tab.c ]
	then	
		echo Clearing: snazzle.tab.c -- bison output
		rm snazzle.tab.c
fi

if [ -f snazzle.tab.h ]
	then	
		echo Clearing: snazzle.tab.h -- bison output
		rm snazzle.tab.h
fi
