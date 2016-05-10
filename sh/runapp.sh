#!/bin/sh

while [ 1 ]
do
	count=`ps -ef |grep ./gather_V0 |grep -v grep |wc -l`

	if [ $count -lt 1 ]
    then
		cd /app
		./gather_V0&
		echo "********************************************"
		echo "              program restart!"
		echo "********************************************"
		sleep 5
	fi
	sleep 10
done
