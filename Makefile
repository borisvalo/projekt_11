CFLAGS=-std=c99 -Wall -pedantic -g
BIN=projekt
CC=gcc
RM=rm -f


all: scaner.o parser.o str.o zasobnik.o bvs.o seznam.o vfce.o ial.o interpret.o main.o
	$(CC) $(CFLAGS) -o $(BIN) main.o parser.o scaner.o seznam.o zasobnik.o vfce.o ial.o interpret.o  str.o bvs.o -lm

parser.o: parser.c parser.h scaner.h zasobnik.h str.h interpret.h bvs.h
	$(CC) $(CFLAGS) -c -o parser.o parser.c 
	
vfce.o: vfce.c vfce.h
	$(CC) $(CFLAGS) -c -o vfce.o vfce.c
	
ial.o: ial.c ial.h
	$(CC) $(CFLAGS) -c -o ial.o ial.c
	
interpret.o: interpret.c interpret.h str.h bvs.h scaner.h parser.h ial.h
	$(CC) $(CFLAGS) -c -o interpret.o interpret.c

str.o: str.c str.h
	$(CC) $(CFLAGS) -c -o str.o str.c	

scaner.o: scaner.c scaner.h obecne.h
	$(CC) $(CFLAGS) -c -o scaner.o scaner.c
	
zasobnik.o: zasobnik.c zasobnik.h parser.h bvs.h
	$(CC) $(CFLAGS) -c -o zasobnik.o zasobnik.c

bvs.o: bvs.c interpret.h
	$(CC) $(CFLAGS) -c -o bvs.o bvs.c
	
seznam.o: seznam.c interpret.h bvs.h scaner.h parser.h
		$(CC) $(CFLAGS) -c -o seznam.o seznam.c

main.o: main.c parser.h scaner.h zasobnik.h str.h interpret.h bvs.h
	$(CC) $(CFLAGS) -c -o main.o main.c
	
clean:
	$(RM) *.o $(BIN)
