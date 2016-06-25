#!/bin/bash
RIX_HOME_=${RIX_HOME:-.}
RIX_FILE=${1}
${RIX_HOME}/rixc -W -q -o ${RIX_FILE} ${RIX_FILE}
if [ $? -eq 0 ]; then
	chmod +x ./${RIX_FILE}.sh
	./${RIX_FILE}.sh
fi
shift
#./${RIX_FILE}.out $@
#rm ${RIX_FILE}.out ${RIX_FILE}.c ${RIX_FILE}.h ${RIX_FILE}.sh ${RIX_FILE}.log
