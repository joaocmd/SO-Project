#!/bin/bash

numthreads=$1
inputfile=$2
inputfilename=$(echo $2 | grep -o "[0-9A-Za-z_\.\-]*txt")
outputfile="results/${inputfilename}.speedups.csv"

#print header
echo "#threads,exec_time,speedup" > $outputfile

#test sequential version
CircuitRouter-SeqSolver/CircuitRouter-SeqSolver $2
seqtime=$(cat "$2.res" | grep "Elapsed time" | tr -s ' ' | cut -d ' ' -f 4)
echo "1S,$seqtime,1" >> $outputfile

#test parallel version for 1..n threads
for i in $(seq 1 $1)
do
    CircuitRouter-ParSolver/CircuitRouter-ParSolver -t $i $2
    partime=$(cat "$2.res" | grep "Elapsed time" | tr -s ' ' | cut -d ' ' -f 4)
    speedup=$(echo "scale=6; ${seqtime}/${partime}" | bc)
    echo "$i,$partime,$speedup" >> $outputfile
done

#replicate output to stdout
cat $outputfile

