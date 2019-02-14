# bin/bash

ESTIMATE_LINE="    Estimated Time To Completion"

RESULT_LINE="[["

FS=":"
SP=" "
CM=","
Bk="]"

flag="0"

estimateTime=0
resultTime=0
CurrentLineHalf=""
CurrentLineRest=""

while IFS='' read -r line || [[ -n "$line" ]];

do
	
	CurrentLineHalf= $($line|cut -d$FS -f1)
	CurrentLineRest= $($line|cut -d$FS -f2)
	echo "CurrentLineHalf $CurrentLineHalf"
	if [ "$CurrentLineHalf" == "$ESTIMATE_LINE" ];
	then
		estimateTime= $($CurrentLineRest|cut -d$SP -f2)
		flag= 0
	else
		CurrentLineHalf= $($line|cut -d$BK -f1)
		if ["$CurrentLineHalf" == "$RESULT_LINE"]
		then
			timeStart= $($line|cut -d$CM -f5)
			timeEnd= $($line|cut -d$CM -f4)
			resultTime= 'expr $timeStart - $timeEnd'	
		fi
done <build-apache-1.5.1_log
