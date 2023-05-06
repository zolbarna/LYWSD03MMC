import asyncio
from bleak import BleakScanner
from functools import partial
from construct_atc_mi import general_format
from atc_mi_adv_format import atc_mi_advertising_format

bindkey = {
    "A4:C1:38:1D:53:E0": bytes.fromhex("093c8bfd6d676101a9569192f1b91c36"),
    "A4:C1:38:BB:E1:C1": bytes.fromhex("582f8f3c05c3827fd0180c5c71ea039f"),
    "A4:C1:38:13:38:77": bytes.fromhex("f448ed47bdccdeebd426595e06e0218c"),
}

async def main():
    count = [0]
    stop_event = asyncio.Event()

    def detection_callback(count, device, advertisement_data):
        format_label, adv_data = atc_mi_advertising_format(advertisement_data)
        if not adv_data:
            return
        mac_address = bytes.fromhex(device.address.replace(":", ""))
        atc_mi_data = general_format.parse(
            adv_data,
            mac_address=mac_address,
            bindkey=bindkey[mac_address] if mac_address in bindkey else None
        )
        #print(f"{count[0]}. {format_label} advertisement: {atc_mi_data}. "
        #    f"RSSI: {device.rssi}")
        print("MAC", atc_mi_data.search("MAC"))
        print("temp:", atc_mi_data.search("temperature"))
        print("humi:", atc_mi_data.search("humidity"))
        print("blvl:", atc_mi_data.search("battery_level"))
        print("bv:", atc_mi_data.search("battery_v"))
        #print("RSSI:", device.rssi)
        count[0] += 1
        if count[0] == 1:
            stop_event.set()

    async with BleakScanner(
        detection_callback=partial(detection_callback, count)
    ) as scanner:
        await stop_event.wait()
#    print("Stopped")


asyncio.run(main())
