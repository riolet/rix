echo Make sure you run with sudo because this will need to set permissions.
if [ -f ./ritchie.lang ] 
	then
		echo Inserting ritchie.lang into /usr/share/gtksourceview-3.0/
		if [ -f /usr/share/gtksourceview-3.0/language-specs/ritchie.lang ]
			then
				sudo rm -rf /usr/share/gtksourceview-3.0/language-specs/ritchie.lang
		fi
		sudo cp ./ritchie.lang /usr/share/gtksourceview-3.0/language-specs/
		sudo chmod 0644 /usr/share/gtksourceview-3.0/language-specs/ritchie.lang
		echo Success!
	else 
		echo no ritchie.lang file found
fi
