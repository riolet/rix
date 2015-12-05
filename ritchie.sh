#!/bin/bash
./ritc -o ${1} ${1} &> /dev/null
gcc -o ${1}.out ${1}.c -lm
./${1}.out
rm ${1}.out ${1}.c ${1}.h
