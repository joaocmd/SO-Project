#!/bin/bash

numthreads=$1
inputfile=$2
#TODO order in \- matters???
inputfilename=$(echo $2 | grep -o "[0-9A-Za-z_\.\-]*txt")
outputfile="results/${inputfilename}.speedups.csv"

echo "#threads,coarse_time,fine_time,speedup" > $outputfile

for i in $(seq 1 $1)
do
    CircuitRouter-ParSolverCoarse/CircuitRouter-ParSolver -t $i $2
    coarsetime=$(cat "$2.res" | grep "Elapsed time" | tr -s ' ' | cut -d ' ' -f 4)
    CircuitRouter-ParSolverFine/CircuitRouter-ParSolver -t $i $2
    finetime=$(cat "$2.res" | grep "Elapsed time" | tr -s ' ' | cut -d ' ' -f 4)
    speedup=$(echo "scale=6; ${coarsetime}/${finetime}" | bc)
    echo "$i,$coarsetime,$finetime,$speedup" >> $outputfile
done
cat $outputfile

