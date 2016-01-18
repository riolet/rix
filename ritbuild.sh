#!/bin/bash
RIX_HOME_=${RIX_HOME:-.}
RIX_FILE=${1}
${RIX_HOME}/rixc -o ${RIX_FILE} ${RIX_FILE}
if [ $? -eq 0 ]; then
	chmod +x ./${RIX_FILE}.sh
	./${RIX_FILE}.sh
fi

