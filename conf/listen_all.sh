#!/bin/bash

oldifs=$IFS
IFS=$'\n'
now=$(date "+%Y-%m-%d %H%M%S")

#-Retention. Keep the last 400 lines only (Minimum 10 minutes history)

tail -n 400 /dev/shm/LYWSD03MMC.log > /dev/shm/LYWSD03MMC_old.log
mv /dev/shm/LYWSD03MMC_old.log /dev/shm/LYWSD03MMC.log


#-Read up the valuemapping file (Associate device names to MAC address)
declare -A mapping

for value in $(cat /etc/conf/device_map.txt); do
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
#        if [[ ! -z "${current}" ]];
# This was an old method where I simply checked if we have any result. 
# Unfortunately the py script returned back an error with some unexpected error
# which went straight into the script.
# Safer way: check if the input contains the part of my MI  mac addresses
	if [[ "${current}" == *"A4:C1:38"*  ]]
        then
                echo $now "$current" >> /dev/shm/LYWSD03MMC.log
                #echo $now "$current"
        fi
done

txt="# HELP\n"

for line in $(tac /dev/shm/LYWSD03MMC.log |cut -d ' ' -f 4,6,8,10,12,14 | sort -u -k1,1); do

        IFS=$oldifs
        read -a output <<< $line

        mac=${output[0]}
        name=${mapping[$mac]}
        temp=${output[1]}
        humi=${output[2]}
        batt=${output[4]}
        rssi=${output[5]}

        if [[ ! -z "${mac}" || ! -z "{$name}" || ! -z "{$temp}" || ! -z "{$humi}" ]];
        then

                echo $mac $name $temp $humi $batt $rssi
                txt+="# TYPE LYWSD03MMC_temperature gauge\n"
                txt+="LYWSD03MMC_temperature{device_mac=\"$mac\",device_name=\"$name\"} $temp\n"
                txt+="# TYPE LYWSD03MMC_humidity gauge\n"
                txt+="LYWSD03MMC_humidity{device_mac=\"$mac\",device_name=\"$name\"} $humi\n"
                txt+="# TYPE LYWSD03MMC_battery_voltage gauge\n"
                txt+="LYWSD03MMC_battery_voltage{device_mac=\"$mac\",device_name=\"$name\"} $batt\n"
                txt+="# TYPE LYWSD03MMC_rssi_level gauge\n"
                txt+="LYWSD03MMC_rssi_level{device_mac=\"$mac\",device_name=\"$name\"} $rssi\n"
        fi
done 

#-Output goes to the /dev/shm/index.html. Symlink from NGINX www root folder had been created in a early stage
# Original: echo -e "$txt" > /usr/share/nginx/www/metrics/index.html
echo -e "$txt" > /dev/shm/index.html
