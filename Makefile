all:
	gcc -ggdb -o ritc ritc.c
	./ritc -o example example.rit
	gcc -ggdb -o example example.c -lm
clean:
	rm parser
