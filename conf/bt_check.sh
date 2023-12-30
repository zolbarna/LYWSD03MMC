#!/bin/sh
#grace period 1m at start
sleep 60

while true; do

        if [ -f /dev/shm/LYWSD03MMC.log ]; 
        then
                mod=$(date --utc --reference=dev/shm/LYWSD03MMC.log +%s)
                now=$(date +%s)

                diff=$(($now - $mod))

                if [ $diff -gt 600 ]; then
                        service dbus restart;
                        sleep 1;
                        service bluetooth restart;
                        echo "Service restarted at $now" >> dev/shm/restart.log
                fi
        fi
        sleep 300
done
