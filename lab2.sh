#!/bin/bash

for inputf in $(ls CircuitRouter-SeqSolver/inputs/*.txt)
do
    absname=$(echo $inputf | cut -d/ -f3)
    nlines=$(wc -l < $inputf)
    nconnections=$(grep -c p < $inputf)
    echo "${absname}: ${nlines} ${nconnections}"
done
