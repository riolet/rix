#!/bin/bash
RITCHIE_HOME_=${RITCHIE_HOME:-.}
RITCHIE_FILE=${1}
${RITCHIE_HOME_}/ritc -o ${RITCHIE_FILE} ${RITCHIE_FILE} &> /dev/null
gcc ${RITCHIE_FILE}.c  ${RITCHIE_HOME}/bstrlib.c  ${RITCHIE_HOME}/errors.c -E
gcc -o ${RITCHIE_FILE}.out ${RITCHIE_FILE}.c  ${RITCHIE_HOME}/bstrlib.c  ${RITCHIE_HOME}/errors.c -lm -I ${RITCHIE_HOME_}
