CFLAGS= -g -Wall -pedantic

CircuitRouter-SeqSolver: CircuitRouter-SeqSolver.c lib/list.h maze.h router.h lib/timer.h lib/types.h
	gcc $(CFLAGS) -lm *.c lib/*.c -o CircuitRouter-SeqSolver

coordinate.o: coordinate.c coordinate.h
	gcc $(CFLAGS) -c coordinate.c -o coordinate.o

grid.o: grid.c grid.h coordinate.h lib/types.h lib/vector.h
	gcc $(CFLAGS) -c grid.c -o grid.o

maze.o: maze.c maze.h coordinate.h grid.h lib/list.h lib/pair.h lib/queue.h lib/types.h lib/vector.h
	gcc $(CFLAGS) -c maze.c -o maze.o

router.o: router.c router.h coordinate.h grid.h lib/queue.h lib/vector.h
	gcc $(CFLAGS) -c router.c -o router.o

lib/list.o: lib/list.c lib/list.h lib/types.h
	gcc $(CFLAGS) -c lib/list.o

lib/pair.o: lib/pair.c lib/pair.h lib/memory.h
	gcc $(CFLAGS) -c lib/list.o

lib/queue.o: lib/queue.c lib/queue.h lib/types.h
	gcc $(CFLAGS) -c lib/queue.o

lib/vector.o: lib/vector.c lib/vector.h lib/types.h lib/utility.h
	gcc $(CFLAGS) -c lib/vector.o

clean:
	rm -f *.o lib/*.o CircuitRouter-SeqSolver
