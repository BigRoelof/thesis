#!/bin/bash

cd build && make && cd ..

for file in cnf/*.cnf; 
do 
    filename=$(basename -- "$file")
    filename="${filename%.*}"
    echo "$filename"
    ./build/SATsolver "$file"
    echo ""
done