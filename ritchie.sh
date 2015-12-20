#!/bin/bash
RITCHIE_DIR=${RITCHIE_HOME:-.}
RITCHIE_FILE=${1}
echo $RITCHIE_FILE
#${RITCHIE_DIR}/ritbuild.sh -o ${RITCHIE_FILE} ${RITCHIE_FILE} &> /dev/null
${RITCHIE_DIR}/ritbuild.sh ${RITCHIE_FILE}
shift
./${RITCHIE_FILE}.out $@
rm ${RITCHIE_FILE}.out ${RITCHIE_FILE}.c ${RITCHIE_FILE}.h ${RITCHIE_FILE}.sh
