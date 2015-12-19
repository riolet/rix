#!/bin/bash
RITCHIE_HOME_=${RITCHIE_HOME:-.}
RITCHIE_FILE=${1}
${RITCHIE_HOME}/ritc -o ${RITCHIE_FILE} ${RITCHIE_FILE} 
echo "gcc ${RITCHIE_FILE}.c ${RITCHIE_HOME}/errors.c -E  -lm -I ${RITCHIE_HOME}"
gcc ${RITCHIE_FILE}.c ${RITCHIE_HOME}/errors.c -E  -lm -I ${RITCHIE_HOME}
echo "gcc -o ${RITCHIE_FILE}.out ${RITCHIE_FILE}.c  ${RITCHIE_HOME}/errors.c -lm -I ${RITCHIE_HOME}"
gcc -ggdb -o ${RITCHIE_FILE}.out ${RITCHIE_FILE}.c  ${RITCHIE_HOME}/errors.c -lm -I ${RITCHIE_HOME}

