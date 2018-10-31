#!/bin/bash

numthreads=$1

for file in $(ls inputs/*.txt)
do
    echo $file
    ./doTest.sh $1 $file
done

