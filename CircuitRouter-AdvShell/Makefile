# Makefile
# João David, 89471
# Gonçalo Almeida, 89448

CFLAGS = -Wall -std=gnu99 -I../lib -g

all: CircuitRouter-AdvShell

CircuitRouter-AdvShell: CircuitRouter-AdvShell.o
	gcc $(CFLAGS) ../lib/vector.o commandparser.o chprocess.o  CircuitRouter-AdvShell.o -o CircuitRouter-AdvShell -lm

CircuitRouter-AdvShell.o: CircuitRouter-AdvShell.c chprocess.o ../lib/vector.o ../lib/types.h commandparser.o
	gcc $(CFLAGS) -c CircuitRouter-AdvShell.c  -o CircuitRouter-AdvShell.o

../lib/vector.o: ../lib/vector.c ../lib/vector.h
	gcc $(CFLAGS) -c ../lib/vector.c -o ../lib/vector.o

commandparser.o: commandparser.c commandparser.h
	gcc $(CFLAGS) -c commandparser.c -o commandparser.o

chprocess.o: chprocess.c chprocess.h
	gcc $(CFLAGS) -c chprocess.c -o chprocess.o

clean:
	@echo Cleaning...
	rm -f CircuitRouter-AdvShell.o chprocess.o commandparser.o ../lib/vector.o CircuitRouter-AdvShell
