if [ -f ritchie.y ] 
	then
		echo Compiling: ritchie.y
		bison -d ritchie.y
	else 
		echo ritchie.y not found
fi

if [ -f ritchie.l ]
	then	
		echo Compiling: ritchie.l
		flex ritchie.l
	else 
		echo ritchie.l not found
fi
