FROM debian:11-slim

MAINTAINER Stone
ARG BUILD_DATE

RUN apt-get update
RUN DEBIAN_FRONTEND="noninteractive" TZ="Etc/UTC" apt-get install bash python3 python3-pip dbus bluez bluetooth nginx procps -y
RUN python3 -m pip install construct pycryptodome bleak
RUN useradd nginx
RUN mkdir -p /etc/ATC_MiThermometer /etc/conf /usr/share/nginx/www/metrics

COPY conf/device.py /usr/local/lib/python3.9/dist-packages/bleak/backends/device.py
COPY conf/nginx.conf /etc/nginx/nginx.conf
COPY conf/default.conf /etc/nginx/http.d/default.conf
COPY ATC_MiThermometer/ /etc/ATC_MiThermometer
COPY conf/listen_all.py /etc/ATC_MiThermometer/python-interface/listen_all.py
COPY conf/listen_all.sh /etc/ATC_MiThermometer/python-interface/listen_all.sh
COPY conf/start.sh /start.sh

RUN chmod 700 start.sh && chmod 700 /etc/ATC_MiThermometer/python-interface/listen_all.sh
CMD ["/start.sh"]

ENTRYPOINT [""]
