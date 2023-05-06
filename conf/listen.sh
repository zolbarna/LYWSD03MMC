#!/bin/bash

current=$(python3 /etc/ATC_MiThermometer/python-interface/listen.py)
now=$(date "+%Y-%m-%d %H%M%S")

#-Retention. Keep the last 400 lines only (Minimum 10 minutes history)

tail -n 400 /tmp/LYWSD03MMC.log > /tmp/LYWSD03MMC_old.log
mv /tmp/LYWSD03MMC_old.log /tmp/LYWSD03MMC.log

#-Get back the current value, store it

if [[ ! -z "${current}" ]]; 
then
	echo $now $current >> /tmp/LYWSD03MMC.log
	echo $now $current
fi

#-Check the last 400 values in reversed order, and write out the most recent (first found by MAC Address) values only for grafana.
#-(In the case if you have more than 400 device / or one of your device did not advertised a data at the past 400 cycle, the value will be dissapeared from the metrics)

declare -a global_mac
txt="# HELP\n"

while read line; do
        read -a output <<< $line
        mac=${output[3]}

        match=0
        for element in "${global_mac[@]}"; do
                if [ "$element" == "$mac" ];
                then 
                        #echo "Match: $element $mac"
                        match=1
                fi
        done

        if [ $match -eq 0 ];
        then
                #echo "New element: $mac"
                global_mac+=($mac)
                temp=${output[5]}
                humi=${output[7]}
                batt=${output[11]}
                #echo $mac $temp $humi $batt
                txt+="# TYPE LYWSD03MMC_temperature gauge\n"
                txt+="LYWSD03MMC_temperature{device=\"$mac\"} $temp\n"
                txt+="# TYPE LYWSD03MMC_humidity gauge\n"
                txt+="LYWSD03MMC_humidity{device=\"$mac\"} $humi\n"
                txt+="# TYPE LYWSD03MMC_battery_voltage gauge\n"
                txt+="LYWSD03MMC_battery_voltage{device=\"$mac\"} $batt\n"

        fi

        #echo $mac $temp $humi $batt
done < <(cat /tmp/LYWSD03MMC.log |tac)

#echo ${global_mac[@]}

#-Output goes to the nginx www root

echo -e "$txt" > /usr/share/nginx/www/metrics/index.html
