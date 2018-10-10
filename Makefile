# Makefile
# João David, 89471
# Gonçalo Almeida, 89448

CFLAGS=-Wall -std=gnu99 -Icustomlib/ -Ilib/

all: CircuitRouter-SimpleShell CircuitRouter-SeqSolver

CircuitRouter-SimpleShell: CircuitRouter-SimpleShell.o
	gcc $(CFLAGS) lib/vector.o lib/commandlinereader.o customlib/finprocess.o  CircuitRouter-SimpleShell.o -o CircuitRouter-SimpleShell -lm
	touch CircuitRouter-SeqSolver/CircuitRouter-SeqSolver.c

CircuitRouter-SeqSolver: CircuitRouter-SeqSolver/CircuitRouter-SeqSolver.c
	cd CircuitRouter-SeqSolver && $(MAKE)

CircuitRouter-SimpleShell.o: CircuitRouter-SimpleShell.c customlib/finprocess.o lib/vector.o lib/types.h lib/commandlinereader.o
	gcc $(CFLAGS) -c CircuitRouter-SimpleShell.c  -o CircuitRouter-SimpleShell.o

lib/vector.o: lib/vector.c lib/vector.h
	gcc $(CFLAGS) -c lib/vector.c -o lib/vector.o

lib/commandlinereader.o: lib/commandlinereader.c lib/commandlinereader.h
	gcc $(CFLAGS) -c lib/commandlinereader.c -o lib/commandlinereader.o

customlib/finprocess.o: customlib/finprocess.c customlib/finprocess.h
	gcc $(CFLAGS) -c customlib/finprocess.c -o customlib/finprocess.o

clean:
	@echo Cleaning...
	cd CircuitRouter-SeqSolver && $(MAKE) clean 
	rm -f CircuitRouter-SimpleShell.o customlib/finprocess.o lib/commandlinereader.o lib/vector.o CircuitRouter-SimpleShell
