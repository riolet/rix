if [ -f snazzle.y ] 
	then
		echo Compiling: snazzle.y
		bison -d snazzle.y
	else 
		echo snazzle.y not found
fi

if [ -f snazzle.l ]
	then	
		echo Compiling: snazzle.l
		flex snazzle.l
	else 
		echo snazzle.l not found
fi
