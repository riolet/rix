echo Make sure you run with sudo because this will need to set permissions.
if [ -f ./rix.lang ] 
	then
		echo Inserting rix.lang into /usr/share/gtksourceview-3.0/
		if [ -f /usr/share/gtksourceview-3.0/language-specs/rix.lang ]
			then
				sudo rm -rf /usr/share/gtksourceview-3.0/language-specs/rix.lang
		fi
		sudo cp ./rix.lang /usr/share/gtksourceview-3.0/language-specs/
		sudo chmod 0644 /usr/share/gtksourceview-3.0/language-specs/rix.lang
		echo Success!
	else 
		echo no rix.lang file found
fi
