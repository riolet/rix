all:
	gcc -ggdb -o ritc ritc.c errors.c
clean:
	rm ritc
