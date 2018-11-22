# Makefile
# João David, 89471
# Gonçalo Almeida, 89448

CFLAGS = -Wall -std=gnu99 -I../lib

all: CircuitRouter-AdvShell

CircuitRouter-AdvShell: CircuitRouter-AdvShell.o
	gcc $(CFLAGS) ../lib/vector.o ../lib/commandlinereader.o finprocess.o  CircuitRouter-AdvShell.o -o CircuitRouter-AdvShell -lm

CircuitRouter-AdvShell.o: CircuitRouter-AdvShell.c finprocess.o ../lib/vector.o ../lib/types.h ../lib/commandlinereader.o
	gcc $(CFLAGS) -c CircuitRouter-AdvShell.c  -o CircuitRouter-AdvShell.o

../lib/vector.o: ../lib/vector.c ../lib/vector.h
	gcc $(CFLAGS) -c ../lib/vector.c -o ../lib/vector.o

../lib/commandlinereader.o: ../lib/commandlinereader.c ../lib/commandlinereader.h
	gcc $(CFLAGS) -c ../lib/commandlinereader.c -o ../lib/commandlinereader.o

finprocess.o: finprocess.c finprocess.h
	gcc $(CFLAGS) -c finprocess.c -o finprocess.o

clean:
	@echo Cleaning...
	rm -f CircuitRouter-AdvShell.o finprocess.o ../lib/commandlinereader.o ../lib/vector.o CircuitRouter-AdvShell
