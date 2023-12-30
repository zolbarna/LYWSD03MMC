# LYWSD03MMC 
LYWSD03MMC prometheus exporter\
docker build . -t lywsd03mmc_231229\
docker run -d --restart always --network host --cap-add NET_ADMIN --cap-add SYS_ADMIN --name LYWSD03MMC lywsd03mmc_231229\
docker run -d --restart unless-stopped --stop-timeout 300  --cap-add=SYS_ADMIN --cap-add=NET_ADMIN --net=host --name LYWSD03MMC lywsd03mmc_310523\

