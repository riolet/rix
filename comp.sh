if [ -f rix.y ] 
	then
		echo Compiling: rix.y
		bison -d rix.y
	else 
		echo rix.y not found
fi

if [ -f rix.l ]
	then	
		echo Compiling: rix.l
		flex rix.l
	else 
		echo rix.l not found
fi
