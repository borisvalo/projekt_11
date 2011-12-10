CFLAGS=-std=c99 -Wall -pedantic -g
BIN=projekt
CC=gcc
RM=rm -f


all:  seznam.o
	$(CC) $(CFLAGS) -o $(BIN) seznam.o

seznam.o: seznam.c interpret.h scaner.h parser.h
	$(CC) $(CFLAGS) -c -o seznam.o seznam.c
	
clean:
	$(RM) *.o $(BIN)
