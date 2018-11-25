all:
	(cd CircuitRouter-SeqSolver && make $(MFLAGS) all)
	(cd CircuitRouter-AdvShell  && make $(MFLAGS) all)

clean:
	(cd CircuitRouter-SeqSolver && make $(MFLAGS) clean)
	(cd CircuitRouter-AdvShell  && make $(MFLAGS) clean)
	(cd inputs && rm *.res*)
    
