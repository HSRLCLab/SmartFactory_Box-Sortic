#!/bin/bash
while true; do
	echo -e "Show-Case Script :\n
	\t1 publish params \t(for STATE 1)\n \
	\t2 publish ack \t\t(for STATE 3 ,4)\n \
	\t3 publish transported \t(for STATE 5)\n \
	\t4 exit \n \
	\t5 log \n \
	\t6 SB level \n \
	\t7 Veh params \n \
	\t8 Veh ack \n \
	enter your choice : "
read goNext
if (("$goNext"=="1")); then
	mosquitto_pub -t Vehicle/Vehicle1234/params -m "{hostname:Vehicle1234,params:[1,1,1,1]}"
	sleep 0.1
	mosquitto_pub -t Vehicle/Vehicle9876/params -m "{hostname:Vehicle9876,params:[2,1,1,1]}"
	sleep 0.1
	mosquitto_pub -t Vehicle/Vehicle1010/params -m "{hostname:Vehicle1010,params:[3,1,1,1]}"
clear
elif (("$goNext"=="2")); then
	mosquitto_pub -t Vehicle/Vehicle1234/ack -m "{hostname:SmartBox4c7f}"
	sleep 0.1
	mosquitto_pub -t Vehicle/Vehicle9876/ack -m "{hostname:SmartBox4c7f}"
	sleep 0.1
	mosquitto_pub -t Vehicle/Vehicle1010/ack -m "{hostname:SmartBox4c7f}"
clear
elif (("$goNext"=="3")); then
	mosquitto_pub -t Vehicle/Vehicle1234/ack -m "{request:SmartBox4c7f}"
	sleep 0.1
	mosquitto_pub -t Vehicle/Vehicle9876/ack -m "{request:SmartBox4c7f}"
	sleep 0.1
	mosquitto_pub -t Vehicle/Vehicle1010/ack -m "{request:SmartBox4c7f}"
clear
elif (("$goNext"=="4")); then
	echo -e " exiting ..."
clear
	exit 1
elif (("$goNext"=="5")); then
	clear
	tail /var/log/mosquitto/mosquitto.log
elif (("$goNext"=="6")); then
	mosquitto_sub -t SmartBox/SmartBox4c7f/level
	clear
elif (("$goNext"=="7")); then
	mosquitto_sub -t Vehicle/Vehicle1234/params
clear
elif (("$goNext"=="8")); then
	mosquitto_sub -t Vehicle/Vehicle1234/ack
	clear
else
	echo -e " Wrong number ! Please reenter "
fi

done