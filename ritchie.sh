#!/bin/bash
RITCHIE_HOME_=${RITCHIE_HOME:-.}
RITCHIE_FILE=${1}
${RITCHIE_HOME}/ritc -q -o ${RITCHIE_FILE} ${RITCHIE_FILE}
if [ $? -eq 0 ]; then
	chmod +x ./${RITCHIE_FILE}.sh
	./${RITCHIE_FILE}.sh
fi
shift
./${RITCHIE_FILE}.out $@
rm ${RITCHIE_FILE}.out ${RITCHIE_FILE}.c ${RITCHIE_FILE}.h ${RITCHIE_FILE}.sh
