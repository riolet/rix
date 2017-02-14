RIX_HOME=$(dirname `pwd`)
echo $RIX_HOME
for i in *.expected
do 
	IFS='.' read -ra PARTS <<< $i
	echo "Building ${PARTS[0]}.rix"
	${RIX_HOME}/rixbuild.sh ${PARTS[0]}.rix
	if [ -f ${PARTS[0]}.arg ]
	then
		echo "cat ${PARTS[0]}.arg |xargs ./${PARTS[0]}.rix.out > ${PARTS[0]}.actual"
		cat ${PARTS[0]}.arg |xargs ./${PARTS[0]}.rix.out > ${PARTS[0]}.actual
		valgrind --log-file=${PARTS[0]}.val ./${PARTS[0]}.rix.out > /dev/null
	else
		valgrind --log-file=${PARTS[0]}.val ./${PARTS[0]}.rix.out > ${PARTS[0]}.actual
	fi
	
	if grep -q "All heap blocks were freed -- no leaks are possible" ${PARTS[0]}.val
	then
		echo "RAII worked correctly"
	else
		echo "RAII issues"
		exit 1
	fi	
	
	if cmp -s ${PARTS[0]}.expected ${PARTS[0]}.actual
	then
		echo "${PARTS[0]}.expected ${PARTS[0]}.actual match"
	else
		echo "${PARTS[0]}.expected ${PARTS[0]}.actual don't match"
		exit 1
	fi	
done
