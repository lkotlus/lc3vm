lc3vm: build/main.o
	gcc -g -o lc3vm build/main.o

build/main.o: main.c
	gcc -c main.c -o build/main.o
