echo Make sure you run with sudo because this will need to set permissions.
if [ -f ritchie.lang ] 
	then
		echo Inserting ritchie.lang into /usr/share/gtksourceview-3.0/
		if [ -f /usr/share/gtksourceview-3.0/language-specs/ritchie.lang ]
			then
				sudo rm -rf /usr/share/gtksourceview-3.0/language-specs/ritchie.lang
		fi
		sudo cp /home/dimitry/Desktop/ritchie/highlighting/ritchie.lang /usr/share/gtksourceview-3.0/language-specs/
		echo Success!
	else 
		echo no ritchie.lang file found
fi
