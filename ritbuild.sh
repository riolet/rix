#!/bin/bash
RITCHIE_HOME_=${RITCHIE_HOME:-.}
RITCHIE_FILE=${1}
echo  "Ritchie File" $RITCHIE_FILE  
${RITCHIE_HOME}/ritc -o ${RITCHIE_FILE} ${RITCHIE_FILE}
chmod +x ./${RITCHIE_FILE}.sh
./${RITCHIE_FILE}.sh
