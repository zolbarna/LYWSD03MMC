FROM debian:bookworm

MAINTAINER Stone
ARG BUILD_DATE

RUN apt-get update
RUN DEBIAN_FRONTEND="noninteractive" TZ="Etc/UTC" apt-get install bash python3 python3-pip dbus bluez bluetooth nginx procps python3-construct python3-bleak -y

#Have to deploy it on the old way, otherwise our script can't find it. IDC

RUN python3 -m pip install pycryptodome --break-system-packages
RUN useradd nginx
RUN mkdir -p /etc/ATC_MiThermometer /etc/conf /usr/share/nginx/www/metrics

COPY conf/device.py /usr/lib/python3/dist-packages/bleak/backends/device.py

#OLD DEBIAN version
#COPY conf/device.py /usr/local/lib/python3.9/dist-packages/bleak/backends/device.py

COPY conf/* /etc/conf

COPY conf/atc1441_TelinkFlasher/* /usr/share/nginx/www/flasher/
COPY conf/nginx.conf /etc/nginx/nginx.conf
COPY conf/default.conf /etc/nginx/http.d/default.conf
COPY ATC_MiThermometer/ /etc/ATC_MiThermometer
COPY conf/listen_all.py /etc/ATC_MiThermometer/python-interface/listen_all.py
COPY conf/listen_all.sh /etc/ATC_MiThermometer/python-interface/listen_all.sh
COPY conf/start.sh /start.sh

RUN chmod 700 /etc/conf/*.sh && chmod 700 /etc/ATC_MiThermometer/python-interface/listen_all.sh
CMD ["/etc/conf/start.sh"]

ENTRYPOINT [""]
