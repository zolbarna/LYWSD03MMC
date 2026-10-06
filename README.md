# LYWSD03MMC

Docker-based Prometheus exporter for Xiaomi LYWSD03MMC / ATC MiThermometer
Bluetooth LE sensors. The container exposes the metrics endpoint on port
`9198`.

## Build

```sh
docker build -t lywsd03mmc .
```

The current Dockerfile also expects `conf/pvvx_flasher/` and
`ATC_MiThermometer/` in the build context. Restore or provide these directories
before building; they are not included in this repository snapshot.

## Run

Bluetooth scanning uses the host network and Linux capabilities:

```sh
docker run -d \
  --restart unless-stopped \
  --stop-timeout 300 \
  --network host \
  --cap-add SYS_ADMIN \
  --cap-add NET_ADMIN \
  --name LYWSD03MMC \
  lywsd03mmc
```

Prometheus metrics are available at `http://<host>:9198/metrics/`.
Configure the sensor MAC addresses and display names in `conf/device_map.txt`.
