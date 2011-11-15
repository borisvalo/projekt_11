CFLAGS=-std=c99 -Wall -pedantic -g
BIN=projekt
CC=gcc
RM=rm -f


all: scaner.o parser.o str.o zasobnik.o bvs.o
	$(CC) $(CFLAGS) -o $(BIN) parser.o scaner.o str.o zasobnik.o bvs.o

str.o: str.c str.h
	$(CC) $(CFLAGS) -c -o str.o str.c

parser.o: parser.c parser.h scaner.h zasobnik.h str.h
	$(CC) $(CFLAGS) -c -o parser.o parser.c 

scaner.o: scaner.c scaner.h obecne.h
	$(CC) $(CFLAGS) -c -o scaner.o scaner.c
	
zasobnik.o: zasobnik.c zasobnik.h parser.h bvs.h
	$(CC) $(CFLAGS) -c -o zasobnik.o zasobnik.c

bvs.o: bvs.c bvs.h
	$(CC) $(CFLAGS) -c -o bvs.o bvs.c

clean:
	$(RM) *.o $(BIN)
