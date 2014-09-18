all:
	gcc -ggdb -o parser parser.c
	./parser example.rit
	gcc -ggdb -o example out.c -lm
clean:
	rm parser
