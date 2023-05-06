#!/bin/bash

oldifs=$IFS
IFS=$'\n'
now=$(date "+%Y-%m-%d %H%M%S")

#-Retention. Keep the last 400 lines only (Minimum 10 minutes history)

tail -n 400 /tmp/LYWSD03MMC.log > /tmp/LYWSD03MMC_old.log
mv /tmp/LYWSD03MMC_old.log /tmp/LYWSD03MMC.log


#-Read up the valuemapping file (Associate device names to MAC address)
declare -A mapping

for value in $(cat /tmp/device_map.txt); do
        index="${value:0:17}"
        name="${value:18}"
        mapping+=([${index}]=${name})
done

#for key in "${!mapping[@]}"; do echo $key; done
#echo ${mapping[@]}


#-Check the last 400 values in reversed order, and write out the most recent (first found by MAC Address) values only for grafana.
#-(In the case if you have more than 400 device / or one of your device did not advertised a data at the past 400 cycle, the value will be dissapeared from the metrics)



#-Get back the current value(s), and store it

for current in $(python3 /etc/ATC_MiThermometer/python-interface/listen_all.py); do
        if [[ ! -z "${current}" ]];
        then
                echo $now "$current" >> /tmp/LYWSD03MMC.log
                echo $now "$current"
        fi
done

IFS=$oldifs



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
                name=${mapping[$mac]}
                temp=${output[5]}
                humi=${output[7]}
                batt=${output[11]}
                rssi=${output[13]}
                echo $mac $name $temp $humi $batt $rssi
                txt+="# TYPE LYWSD03MMC_temperature gauge\n"
                txt+="LYWSD03MMC_temperature{device_mac=\"$mac\",device_name=\"$name\"}   $temp\n"
                txt+="# TYPE LYWSD03MMC_humidity gauge\n"
                txt+="LYWSD03MMC_humidity{device_mac=\"$mac\",device_name=\"$name\"} $humi\n"
                txt+="# TYPE LYWSD03MMC_battery_voltage gauge\n"
                txt+="LYWSD03MMC_battery_voltage{device_mac=\"$mac\",device_name=\"$name\"} $batt\n"
                txt+="# TYPE LYWSD03MMC_rssi_level gauge\n"
                txt+="LYWSD03MMC_rssi_level{device_mac=\"$mac\",device_name=\"$name\"} $rssi\n"

        fi
        #echo $mac $temp $humi $batt
done < <(cat /tmp/LYWSD03MMC.log |tac)

#echo ${global_mac[@]}

#-Output goes to the nginx www root

echo -e "$txt" > /usr/share/nginx/www/metrics/index.html
