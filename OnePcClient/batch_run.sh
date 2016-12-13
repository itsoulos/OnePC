#!/bin/bash
# DEFAULT PORT IS 51000
SERVER_IP=192.168.1.5
SERVER_PORT=51000
INSTANCES=4
ONEPCCLIENT=./OnePcClient
for i in $(seq 1 $INSTANCES)
do
	name=$(hostname)-$i
	$ONEPCCLIENT  --gui=false --serverip=$SERVER_IP --serverport=$SERVER_PORT --seed=$i --machinename=$name & 
done
