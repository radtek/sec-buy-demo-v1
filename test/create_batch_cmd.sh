#!/bin/sh
remoteIP="127.0.0.1"
remotePort=12345
connectPerClient=10000
stopCount=900
sleepTime=1
echo '' > output/batch_connect.sh
truncate -s 0 output/batch_connect.sh
for line in `cat output/exist_local_ip.txt`
do
	echo "./clientTest -l $line -r $remoteIP -p $remotePort -c $connectPerClient -s $stopCount -t $sleepTime > log/test.log.$line &" >> output/batch_connect.sh
done

for line in `cat output/new_virtual_ip.txt`
do
	echo "./clientTest -l $line -r $remoteIP -p $remotePort -c $connectPerClient -s $stopCount -t $sleepTime > log/test.log.$line &" >> output/batch_connect.sh
done
