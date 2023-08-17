#!/bin/bash
# start services
printf "Init file creations....\n"
touch /tmp/restart.log
touch /tmp/LYWSD03MMC.log
printf "Starting up the DBUS Service...\n"
service dbus start
printf "Starting up the Bluetooth Service...\n"
service bluetooth start
printf "DBUS service status...\n"
service dbus status
sleep 1
printf "Bluetooth service status...\n"
service bluetooth status
sleep 5
printf "Starting NGINX on port 9198...\n"
exec /usr/sbin/nginx &
sleep 1
printf "Starting BT monitor...\n"
exec /etc/conf/bt_check.sh &

#printf "Starting php-fpm8...\n"
#exec /usr/sbin/php-fpm8 &

STOPIT=0

function sig_term {
#  echo "Sigterm caught"
#
  STOPIT=1
}

trap sig_term SIGTERM

while true
do
        if [ $STOPIT -eq 1 ]
        then
		echo 'Initiate terminating sequence...'
		break
	else
		#date | tr -d '\n'
                #echo ' ' | tr -d '\n'
                /etc/ATC_MiThermometer/python-interface/listen_all.sh
                sleep 30
	fi
done
