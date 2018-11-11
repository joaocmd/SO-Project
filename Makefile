SEQSOLVER=CircuitRouter-SeqSolver/CircuitRouter-SeqSolver.c
PARSOLVER=CircuitRouter-ParSolver/CircuitRouter-ParSolver.c

all: forcemake CircuitRouter-SeqSolver CircuitRouter-ParSolver

forcemake:
	touch $(SEQSOLVER)
	touch $(PARSOLVER)

CircuitRouter-SeqSolver: $(SEQSOLVER)
	cd CircuitRouter-SeqSolver && $(MAKE)

CircuitRouter-ParSolver: $(PARSOLVER)
	cd CircuitRouter-ParSolver && $(MAKE)

clean:
	cd CircuitRouter-SeqSolver && make clean
	cd CircuitRouter-ParSolver && make clean
    
