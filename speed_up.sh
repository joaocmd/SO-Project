#!/bin/bash

CircuitRouter-SeqSolver/CircuitRouter-SeqSolver -t $1 $2
seqTime=cat "$2.res" | grep "Elapsed time" | tr -s' ' | cut -d ' ' -f 4

CircuitRouter-ParSolver/CircuitRouter-ParSolver -t $1 $2
parTime=cat "$2.res" | grep "Elapsed time" | tr -s' ' | cut -d ' ' -f 4

speedUp=$(echo "scale=6; ${seqTime}/${parTime}" | bc)
echo Speedup = $speedUp
