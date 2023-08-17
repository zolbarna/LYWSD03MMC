#!/bin/sh
#grace period 1m at start
sleep 60

while true; do

        if [ -f /tmp/LYWSD03MMC.log ]; 
        then
                mod=$(date --utc --reference=tmp/LYWSD03MMC.log +%s)
                now=$(date +%s)

                diff=$(($now - $mod))

                if [ $diff -gt 600 ]; then
                        service dbus restart;
                        sleep 1;
                        service bluetooth restart;
                        echo "Service restarted at $now" >> tmp/restart.log
                fi
        fi
        sleep 300
done
