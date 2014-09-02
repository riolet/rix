all:
	gcc -ggdb -o parser parser.c
	./parser helloworld.rit
	gcc -ggdb -o out out.c
clean:
	rm parser
