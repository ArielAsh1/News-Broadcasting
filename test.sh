#!/bin/bash

for ((i=1; i<=1000; i++))
do
    echo "Iteration $i"
    ./ex3.out ./conf.txt > output.txt
    p1=$(cat output.txt | grep 'Producer 1' | wc -l)
    if [[ $p1 != 20 ]]
    then
        echo "failed for p1 - received $p1/20"
        cat ./output.txt | grep 'Producer 1'
        exit 1
    fi
    p2=$(cat output.txt | grep 'Producer 2' | wc -l)
    if [[ $p2 != 10 ]]
    then
        echo "failed for p2 - received $p2/10"
        cat ./output.txt | grep 'Producer 2'
        exit 1
    fi
    p3=$(cat output.txt | grep 'Producer 3' | wc -l)
    if [[ $p3 != 15 ]]
    then
        echo "failed for p3 - received $p3/15"
        cat ./output.txt | grep 'Producer 3'
        exit 1
    fi
done
