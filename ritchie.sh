#!/bin/bash
RITCHIE_DIR=${RITCHIE_HOME:-.}
RITCHIE_FILE=${1}
${RITCHIE_DIR}/debug.sh -o ${RITCHIE_FILE} ${RITCHIE_FILE} &> /dev/null
chmod +x ./${RITCHIE_FILE}.sh
./${RITCHIE_FILE}.sh
shift
./${RITCHIE_FILE}.out $@
rm ${RITCHIE_FILE}.out ${RITCHIE_FILE}.c ${RITCHIE_FILE}.h ${RITCHIE_FILE}.sh
