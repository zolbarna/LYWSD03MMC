# Xiaomi Mijia LYWSD03MMC HW: [B1.4](https://github.com/pvvx/ATC_MiThermometer/tree/master/BoardPinout), [B1.6](https://github.com/pvvx/ATC_MiThermometer/issues/125), [B1.7](https://github.com/pvvx/ATC_MiThermometer/issues/145), [B1.9](https://github.com/pvvx/ATC_MiThermometer/issues/125), [B2.0](https://github.com/pvvx/ATC_MiThermometer/issues/166) & Xiaomi Mijia [MJWSD05MMC](https://github.com/pvvx/ATC_MiThermometer/issues/307) & Xiaomi Miaomiaoce [MHO-C401](https://pvvx.github.io/MHO_C401) & CGG1-M ver: [2020, 2021 - "Qingping Temp & RH M"](https://pvvx.github.io/CGG1), [2022 - "Qingping Temp RH M"](https://pvvx.github.io/CGG1_2022) & [CGDK2](https://pvvx.github.io/CGDK2) Qingping Temp & RH Monitor Lite

_This repository contains custom firmware for several Bluetooth Thermometer & Hygrometer devices.
The download of the appropriate firmware for your device will be presented automatically in [TelinkMiFlasher.html](https://pvvx.github.io/ATC_MiThermometer/TelinkMiFlasher.html). Downloading files from this repository is not required.
Alternative firmware and configurator from ATC1441 (variant ATC_Thermometer.bin) can be found here: [atc1441/ATC_MiThermometer](https://github.com/atc1441/ATC_MiThermometer).
Custom firmware is not supported in MiHome. But it is possible to work LYWSD03MMC with 'Xiaomi Gateway 3' by first registering in MiHome on the official firmware, flashing to custom firmware with restoring the binding key from MiHome, replacing the device name with "LYWSD03MMC" and setting the advertising type to "MIJIA (MiHome)" marked "encrypted"._

The custom firmware can be flashed _via a modern browser_ and _over-the-air (OTA)_ without opening the device and unlocks several customization options. You can go back to the original firmware at any time. OTA and configuration main page - [TelinkMiFlasher.html](https://pvvx.github.io/ATC_MiThermometer/TelinkMiFlasher.html).

**Key features**

* Extended battery life (over a year)
* Improved measurement accuracy and extended format in 0.01 units
* Full support Home Assistant ['BTHome integration'](https://bthome.io/)
* Supports [encrypted connections using **PIN-code**](https://github.com/pvvx/ATC_MiThermometer/issues/174#issuecomment-1003987084) and encrypted **bindkey** beacon
* Supports Bluetooth v5.0+ **[LE Long Range](https://www.youtube.com/watch?v=2VkbGIEFn4g&t=80s)** (LE 1M/2M/Coded 500K/125K), CSA1/CSA2 (Channel Selection Algorithm), Advertising Extensions: primary and secondary Coded PHY S=8, Connectable
* **3 LCD Display Screens** (Looping): Temperature & Humidity & Comfort, Temperature & Battery Level, Clock
* **Measurement values recording** & Charting. See [Reading Measurements from Flash](#reading-measurements-from-flash)
* **Adjustable correction offsets** and **Comfort zones**
* Support for 4 beacon formats: Xiaomi, ATC, Custom, BTHome and encrypted options.
* Adjustable RF TX Power & Bluetooth advertising interval
* DIY improvements - relay control by temperature or humidity, transmission of the reed switch status - open / closed or pulse counter

|[Xiaomi Mijia (LYWSD03MMC)](https://pvvx.github.io/ATC_MiThermometer) | [Xiaomi Miaomiaoce (MHO-C401)](https://pvvx.github.io/MHO_C401) | [Qingping Temp & RH Monitor (CGG1-Mijia)](https://pvvx.github.io/CGG1) | [CGDK2 Qingping Temp & RH Monitor Lite](https://pvvx.github.io/CGDK2) | [Xiaomi Mijia MJWSD05MMC](https://pvvx.github.io/MJWSD05MMC) |
|:--:|:--:|:--:|:--:|:--:|
|  <img src="https://pvvx.github.io/ATC_MiThermometer/img/LYWSD03MMC.png" alt="Xiaomi Mijia (LYWSD03MMC)" width="120"/> |  <img src="https://pvvx.github.io/MHO_C401/img/MHO-C401.png" alt="Xiaomi Miaomiaoce (MHO-C401)" width="120"/> | <img src="https://pvvx.github.io/CGG1/img/CGG1-M.jpg" alt="E-ink CGG1 'Qingping Temp & RH Monitor', Xiaomi Mijia DevID: 0x0B48" width="120"/> | <img src="https://github.com/pvvx/pvvx.github.io/blob/master/CGDK2/img/CGDK2.jpg" alt="CGDK2 Qingping Temp & RH Monitor Lite" width="120"/> | <img src="https://pvvx.github.io/MJWSD05MMC/img/MJWSD05MMC.png" alt="Xiaomi Mijia (MJWSD05MMC)" width="120"/> |

**Warnings**

If you have many BLE devices, the adapter must be able to receive BLE advertisements every 3 ms. UART speeds of 921600 baud and even USB1.1 are not enough to transfer information in binary form in the HCI format. Requires BT adapter with USB2.0 HS. Otherwise, there will be many gaps in the reception.

Do not use ESP32xxx - it cannot work with BLE at normal speed and drains the batteries of all BLE devices when connected!

ESPHome does not work with Bluetooth 5.0 and misses a lot of advertising packets from BLE devices.

For reliable connection and refirmware on LYWSD03MMC, the battery level must be more than 40%. This is a design feature - a [cheaper version of the hardware from Xiaomi](https://user-images.githubusercontent.com/12629515/105704705-423fc600-5f20-11eb-88ca-a2db712d0222.png).

[On devices with missing power capacities](https://github.com/pvvx/ATC_MiThermometer/issues/265#issuecomment-1431495494), do not set 'Connect latency' to more than 1000 ms!

Home Assistant in "Bluetooth" integration does not full support Bluetooth  5.0 and is not implied. 

[How to switch the USB-BT5.0+ adapter to work in LE Long Range in Home Assistant under Linux:](https://github.com/pvvx/ATC_MiThermometer/issues/297)

On thermometers with a button, if the button function is set to Connect, then a short application on the button for 80 seconds enables connection in BT4.0 mode.

If you accidentally set the 'LE Long Range' on the thermometer, and your device does not support BT5.0, remove and insert the battery - the thermometer will switch to BT4.2 support mode.

If you have a smartphone with BT5.0+ and the nRFConnect program, then you can reset all settings to default by connecting to a thermometer and sending the 0x56 command, as shown in the screenshot:

![img nRFConnect_set_default.png](https://github.com/pvvx/ATC_MiThermometer/blob/master/img/nRFConnect_set_default.png)

To disable only the 'Long Range' option, use the code 0xDD.

The Web Bluetooth API in Chrome does not yet allow scanning for Bluetooh 5.0+ devices.

**Table of content**

<!-- TOC depthFrom:2 depthTo:3 -->

- [Getting Started](#getting-started)
    - [Flashing or Updating the Firmware (OTA)](#flashing-or-updating-the-firmware-ota)
    - [Configuration](#configuration)
- [Firmware](#firmware)
    - [Firmware Binaries](#firmware-binaries)
    - [Firmware version history](#firmware-version-history)
- [Applications](#applications)
    - [Reading Measurements from Flash](#reading-measurements-from-flash)
    - [Reading Measurements in Connected Mode](#reading-measurements-in-connected-mode)
    - [Reading Measurements in Advertising Mode](#reading-measurements-in-advertising-mode)
- [Technical specifications](#technical-specifications)
    - [Average power consumption](#average-power-consumption)
    - [Bluetooth Advertising Formats](#bluetooth-advertising-formats)
    - [Bluetooth Connection Mode](#bluetooth-connection-mode)
    - [Temperature or humidity trigger](#temperature-or-humidity-trigger-gpio-pa5-lywsd03mmc-label-on-the-reset-pin)
    - [Reed switch or button functions](#reed-switch-or-button-functions)
    - [Interface for receiving and displaying data on the LCD.](#interface-for-receiving-and-displaying-data-on-the-lcd)
    - [The USB-COM adapter writes the firmware in explorer. Web version.](#the-usb-com-adapter-writes-the-firmware-in-explorer-web-version)
- [Related Work](#related-work)
- [Resources & Links](#resources--links)
    - [CJMCU-2557 BQ25570](#cjmcu-2557-bq25570)

<!-- /TOC -->


## Getting Started 

You can conveniently flash, update and configure the bluetooth thermometers remotely using a bluetooth connection and a modern web browser.

### Flashing or Updating the Firmware (OTA)

To flash or update the firmware, use a Google Chrome, Microsoft Edge or Opera Browser.

1. Go to the [Over-the-air Webupdater Page `TelinkMiFlasher.html`](https://pvvx.github.io/ATC_MiThermometer/TelinkMiFlasher.html) *
2. If using Android, Windows, Linux: Ensure you enabled "experimental web platform features". Therefore copy the according link (i.e. `chrome://flags/#enable-experimental-web-platform-features` for Chrome), open a new browser tab, paste the copied URL. Now sten the _Experimental Web Platform features_ flag to _Enabled_. Then **restart the browser**.
3. For all sensors with a button, before activation, it is necessary to reset the past bindings by holding the button for a long time.
4. In the Telink Flasher Page: Press `Connect`: The browser should open a popup with visible Bluetooth devices. Choose the according target device (i.e. `LYWSD03MMC`) to pair. 
5. After connection is established a _Do Acivation_ button appears. Press this button to start the decryption key process.
6. Now you can press the _Custom Firmware ver x.x_ button to directly flash the custom firmware. Alternatively you can choose a specific firmware binary (i.e. the original firmware) via the file chooser
7. Press _Start Flashing_.

 * Use [TelinkOTA](https://pvvx.github.io/ATC_MiThermometer/TelinkOTA.html) to flash old or alternative versions ([ATC1441](https://atc1441.github.io/TelinkFlasher.html)). This is a program for OTA projects with Telink SDK, no third-party (mijia) protections.

 * Addition for CGG1-M: Press and hold the button on the back of CGG1-M for 2 seconds until the Bluetooth icon starts blinking on the e-ink display, and then click the 'Connect' and 'Do Activation' buttons on the [TelinkMiFlasherl](https://pvvx.github.io/ATC_MiThermometer/TelinkMiFlasher.html).

 * [Addition for MJWSD05MMC](https://github.com/pvvx/ATC_MiThermometer/issues/307): Press and hold both buttons for a few seconds until the screen blink (device resets). Next, briefly press the top side button, then briefly press the bottom button. The Bluetooth icon will start flashing.  Next 'Connect' and 'Do Activation' in [TelinkMiFlasherl](https://pvvx.github.io/ATC_MiThermometer/TelinkMiFlasher.html).

### Configuration
After you have flashed the firmware, the device has changed it's bluetooth name to something like `ATC_F02AED`. Using the [`TelinkMiFlasher.html`](https://pvvx.github.io/ATC_MiThermometer/TelinkMiFlasher.html) you have various configuration options.

**General process:**
1. Press _Connect_
2. Select the according device to connect/pair (i.e.  `ATC_F02AED`)
3. Several configuration options appear
4. Choose _Send Config_ to send changed value to the device. Or press _Set default_ and then _Send config_ to revert to the Defaults

| Option | Description |
| ------ | ----------- |
| _Temperature and Humidity offset_ | Enter a value to correct the offset of the Temperature or Humidity displayed: For example `-1.4` will decrease the Temperature by 1.4°
| _Smiley or Comfort_ | Choose a static smiley or check the "Comfort" Radio box to change the smiley depending on current Temperature and Humidity. |
| _Comfort Parameters_ | Defines the Lower (Lo) and Upper (Hi) Range for Temperature and Humidity interpreted as comfort zone. In the default configuration a smiley will appear.
| _Advertising Type_ | Type of supported [Bluetooth Advertising Formats](#bluetooth-advertising-formats).
| _Set time_ | sends the current time to the device
| _Comfort, Show batt, Clock_ | Ticking the according boxes you can enable interval rotation between different LCD screens. See the example video below.


**Example of LCD display modes**

You can configure different LCD Display modes using _Comfort, Show batt_ and _Clock_ configuration checkboxes. The enabled LCD Display Modes will appear one-by-one in a loop. 

[![YoutubeVideo](https://img.youtube.com/vi/HzYh1vq8ikM/0.jpg)](https://youtu.be/HzYh1vq8ikM)

Battery and clock display are enabled in the settings. The rest of the settings is kept default. The video contains 2 cycles.

> 1. Temperature and humidity
> 2. Temperature and % of battery
> 3. Temperature and humidity
> 4. Hours and minutes


## Firmware
### Firmware Binaries
You can directly update/flash the firmware without downloading the binaries below.

**Custom Firmware Versions:**

The download of the appropriate firmware for your device will be presented automatically in [TelinkMiFlasher.html](https://pvvx.github.io/ATC_MiThermometer/TelinkMiFlasher.html). Downloading files from this repository is not required.

* [LYWSD03MMC Custom Firmware Version 4.2](https://github.com/pvvx/ATC_MiThermometer/raw/master/ATC_v42.bin) (HW: B1.4, B1.6, B1.7, B1.9, B2.0)
* [MJWSD05MMC Custom Firmware Version 4.2](https://github.com/pvvx/ATC_MiThermometer/raw/master/BTH_v42.bin) ([MJWSD05MMC](https://pvvx.github.io/MJWSD05MMC))
* [MHO-C401 2020-2021 Custom Firmware Version 4.2](https://github.com/pvvx/ATC_MiThermometer/raw/master/MHO_C401_v42.bin) (manufacturer Xiaomi, not [Azarton](https://github.com/pvvx/ATC_MiThermometer/issues/114)!)
* [MHO-C401 2022 Custom Firmware Version 4.2](https://github.com/pvvx/ATC_MiThermometer/raw/master/MHO_C401N_v42.bin) (manufacturer Xiaomi, not [Azarton](https://github.com/pvvx/ATC_MiThermometer/issues/114)!)
* [CGG1-M 2020-2021 Custom Firmware Version 4.2](https://github.com/pvvx/ATC_MiThermometer/raw/master/CGG1_v42.bin) ([CGG1-M](https://github.com/pvvx/ATC_MiThermometer/issues/41#issuecomment-812803456))
* [CGG1-M 2022 Custom Firmware Version 4.2](https://github.com/pvvx/ATC_MiThermometer/raw/master/CGG1M_v42.bin) ([CGG1-M](https://github.com/pvvx/ATC_MiThermometer/issues/41#issuecomment-812803456))
* [CGDK2 Custom Firmware Version 4.2](https://github.com/pvvx/ATC_MiThermometer/raw/master/CGDK2_v42.bin) ([CGDK2](https://pvvx.github.io/CGDK2))

**Original Manufacturer Firmware Version**

In case you want to go back to the original firmware, you can download them here:

* [Xiaomi LYWSD03MMC Original Firmware v1.0.0_0109](https://github.com/pvvx/ATC_MiThermometer/raw/master/Original_OTA_Xiaomi_LYWSD03MMC_v1.0.0_0109.bin) (HW: B1.4 only)
* [Xiaomi LYWSD03MMC Original Firmware v1.0.0_0130](https://github.com/pvvx/ATC_MiThermometer/raw/master/Original_OTA_Xiaomi_LYWSD03MMC_v1.0.0_0130.bin) (HW: B1.4..B2.0)
* [Xiaomi MJWSD05MMC Original Firmware v2.0.0_0026](https://github.com/pvvx/ATC_MiThermometer/raw/master/Original_OTA_Xiaomi_MJWSD05MMC_v2.0.0_0026.bin) (HW: V2.3)
* [Xiaomi MHO-C401 Original Firmware v1.0.0_0010](https://github.com/pvvx/ATC_MiThermometer/raw/master/Original_OTA_Xiaomi_MHO_C401_v1.0.0_0010.bin)
* [Xiaomi and Azarton MHO_C401 Original Firmware v1.0.0_0016](https://github.com/pvvx/ATC_MiThermometer/raw/master/Original_OTA_Xiaomi_MHO_C401_v1.0.0_0016.bin)
* [Xiaomi SJWS01LM Original Firmware v1.1.1_0018](https://github.com/pvvx/ATC_MiThermometer/raw/master/Original_OTA_SJWS01LM_1.1.1_0018.bin)
* [Qingping CGG1-M 2020-2021 Original Firmware v1.0.1_0093](https://github.com/pvvx/ATC_MiThermometer/raw/master/Original_OTA_CGG1_v1.0.1_0093.bin)
* [Qingping CGG1-M 2022 Original Firmware v2.2.1](https://github.com/pvvx/ATC_MiThermometer/raw/master/Original_OTA_CGG1M_v2.2.1.bin)
* [Qingping CGPR1 Original Firmware v1.1.1_0228](https://pvvx.github.io/CGPR1/img/Original_OTA_CGPR1_v1.1.1_0228.bin)
* [Qingping CGDK2 Original Firmware v1.1.1_0210](https://pvvx.github.io/CGPR1/img/Original_OTA_CGDK2_v1.1.1_0210.bin)
* [Qingping CGDK2 Original Firmware v1.1.1_0217](https://pvvx.github.io/CGPR1/img/Original_OTA_CGDK2_v1.1.1_0217.bin)
* [Qingping CGDK2 Original Firmware v1.1.1_0223](https://pvvx.github.io/CGPR1/img/Original_OTA_CGDK2_v1.1.1_0223.bin)

### Firmware version history

| Version | Changes |
| ------: | ------- |
|     1.2 | Bind, Set Pin-code, Support MHO-C401 |
|     1.3 | Get/Set comfort parameters |
|     1.4 | Get/Set device name, Get/Set MAC |
|     1.5 | Add Standard Device Information Characteristics |
|     1.6 | Fix alternation of Advertising in mi mode |
|     1.7 | Authorization and encryption in permissions to access GAP ATT attributes, if pin code is enabled |
|     1.8 | Time display (instead of a blinking smile) |
|     1.9 | Recording measurements to flash memory (cyclic buffer for 19632 measurements) |
|     2.0 | Recording measurements with averaging to flash memory |
|     2.1 | Periodic display refresh for MHO-C401 <br> 'Erase mi-keys' option to return to original firmware |
|     2.2 | Added parameter "Encrypted Mi Beacon" |
|     2.3 | Added "Delete all records" |
|     2.4 | Added parameter "Clock time step" |
|     2.5 | Remove TRG/FLG errors, minor optimization |
|     2.6 | Expanding the ranges of threshold parameters (TRG) |
|     2.7 | Reducing power consumption of MHO-C401 (EDP update), adding version for CGG1-M 2020-2021  |
|     2.8 | Added saving bindkey to EEP if mi-keys are erased, reduced TX power to 0 dB for defaults |
|     2.9 | Added additional id flags to advertising packages |
|     3.0 | Added toggle support for advertising package structures for third-party software |
|     3.1 | Fix security attributes (for pincode) |
|     3.2 | Added [new encrypted beacon formats](https://github.com/pvvx/ATC_MiThermometer/issues/94#issuecomment-846984018), reed switch maintenance |
|     3.3 | Added autodetection for [LYWSD03MMC hardware versions B1.6 and B1.9](https://github.com/pvvx/ATC_MiThermometer/issues/125). For CGG1-M and MHO-C401 - autodetection of SHTV3 or SHT4x sensors |
|     3.4 | Correct Hardware Version Setting for [LYWSD03MMC B1.7](https://github.com/pvvx/ATC_MiThermometer/issues/145) |
|     3.5 | Correction of moisture readings for SHT4x sensors. [Rounding off sensor values on display.](https://github.com/pvvx/ATC_MiThermometer/issues/163). Saving HW string B2.0 on LYWSD03MMC. Eliminated [battery voltage noise](https://github.com/pvvx/ATC_MiThermometer/issues/180) in CGG1-M. CGG1 - correction of the battery charge display. Added CGDK2 and modified [(DIY) variant of CGDK2-2](https://pvvx.github.io/CGDK2/CGDK2-2/). |
|     3.6 | Additional parameters: Support BT5.0 PHY, Channel Selection Algorithm 2, Correct RF-TX Power in suspend |
|     3.7 | Added [format 'HA BLE'](https://github.com/custom-components/ble_monitor/issues/548), reed switch operation mode: Switch and Count. Full support for "HA BLE" encryption. Speed correction I2C for LCD controller LYWSD03MMC HW:1.9. Support for CGG1-M hardware version 2022. |
|     3.8 | Fix [clear_memo](https://github.com/pvvx/ATC_MiThermometer/issues/240), renaming 'HA BLE' format to ['BTHome'](https://bthome.io/) |
|     3.9 | Changed the calculation of battery readings in %, optimization of consumption for the solar battery when the voltage drops below 2V (deep-sleep), added option to invert event for reed switch, small optimizations. |
|     4.0 | Improved stability in connected mode. I2C bus CLK and PullUp correction.|
|     4.1 | Changed "connection latency" for [stability in connected](https://github.com/pvvx/ATC_MiThermometer/issues/265#issuecomment-1431495494).|
|     4.2 | Option Increasing Communication Distance: Bluetooth 5.0 LE Long Range (Advertising Extensions: primary and secondary Coded PHY S=8, Connectable). Added support [MHO-C401 (2022.11)](https://pvvx.github.io/MHO_C401N) and [MJWSD05MMC](https://pvvx.github.io/MJWSD05MMC). Added function key: Connect |
|     4.3 | Increasing the degree of averaging (x64) of the sent values over the battery. Added "Screen Off" option. |

## Applications

### Reading Measurements from Flash

[GraphMemo.html](https://pvvx.github.io/ATC_MiThermometer/GraphMemo.html)

To record the measurement results, a cyclic buffer in the flash memory for 19632 measurements is used.
The recording step interval is set in the interface.
With the default settings, the recording step is 10 minutes, which gives a recording depth of 4 and a half months.
During the step period, the sensor data and battery voltage are averaged, time stamped, and written to flash memory.

Setting the value to 0 disable logging to internal storage.

![FlashData](https://pvvx.github.io/ATC_MiThermometer/img/Weather.gif)

### Reading Measurements in Connected Mode

[GraphAtc.html](https://pvvx.github.io/ATC_MiThermometer/GraphAtc.html)
[GraphAtc1.html](https://pvvx.github.io/ATC_MiThermometer/GraphAtc1.html)
[GraphAtc2.html](https://pvvx.github.io/ATC_MiThermometer/GraphAtc2.html)
[DevPoint.html](https://pvvx.github.io/ATC_MiThermometer/DevPoint.html)

![GraphAtc_html](https://github.com/pvvx/ATC_MiThermometer/blob/master/img/GraphAtc_html.gif) 

### Reading Measurements in Advertising Mode
This requires the _Experimental Web Platform Features_ flag enabled in your browser. See [Flashing or Updating the Firmware (OTA)](#flashing-or-updating-the-firmware-ota).

[Advertising.html](https://pvvx.github.io/ATC_MiThermometer/Advertising.html)

## Technical specifications

### Average power consumption

**Xiaomi Mijia (LYWSD03MMC B1.4)**

Using the default settings for advertising interval of 2.5 seconds
and measurement interval of 10 seconds:

* Bluetooth Advertisement: 14..15 uA 3.3V ([CR2032](https://pvvx.github.io/ATC_MiThermometer/CustPower.html) [over 1 years](https://github.com/pvvx/ATC_MiThermometer/issues/23#issuecomment-766898945))
* Bluetooth Connection: 14..25 uA 3.3V (CR2032 over 10 months)

![PowerAdvInt](https://github.com/pvvx/ATC_MiThermometer/blob/master/img/PowerAdvInt.gif)

![PowerLife.gif](https://github.com/pvvx/ATC_MiThermometer/blob/master/img/PowerLife.gif)

**Xiaomi Mijia (MJWSD05MMC)**

Using the default settings for advertising interval of 5 seconds and measurement interval of 20 seconds:

* Bluetooth Advertisement: 19..21 uA 3.3V (CR2450 over 2 years)
* Bluetooth Connection: 25..30 uA 3.3V (CR2450 over 1.5 years)

**Xiaomi Miaomiaoce (MHO-C401)**

Using the default settings for advertising interval of 2.5 seconds and measurement interval of 20 seconds:

* Bluetooth Advertisement: 15..30 uA 3.3V ([depends on the amount of temperature or humidity changes over time to display](https://pvvx.github.io/MHO_C401/power_altfw.html))
* Bluetooth Connection: 15..30 uA 3.3V (depends on the amount of temperature or humidity changes over time to display)

**Xiaomi Miaomiaoce (MHO-C401N)**

Using the default settings for advertising interval of 2.5 seconds and measurement interval of 20 seconds:

* Bluetooth Advertisement: 10..14 uA 3.3V (depends on the amount of temperature or humidity changes over time to display) 
* Bluetooth Connection: 13..17 uA 3.3V (depends on the amount of temperature or humidity changes over time to display)

**Qingping (CGDK2)**

Using the default settings for advertising interval of 2.5 seconds and measurement interval of 20 seconds:

* Bluetooth Advertisement: 18..21 uA 3.3V 
* Bluetooth Connection: 23..30 uA 3.3V

**Qingping (CGG1)**

Using the default settings for advertising interval of 2.5 seconds and measurement interval of 20 seconds:

* Bluetooth Advertisement: 12..16 uA 3.3V 
* Bluetooth Connection: 23..30 uA 3.3V

**Qingping (CGG1N)**

Using the default settings for advertising interval of 2.5 seconds and measurement interval of 10 seconds:

* Bluetooth Advertisement: 12..16 uA 3.3V 
* Bluetooth Connection: 23..30 uA 3.3V

### Bluetooth Advertising Formats

The Firmware can be configured to support one of four different Bluetooth advertisements data formats:

Xiaomi, ATC, Custom, BTHome and encrypted options. 

Only the BTHome and Xiaomi support the native [HA Xiaomi Integration](https://www.home-assistant.io/integrations/xiaomi_ble/), for the others use the Custom Component [Passive BLE](https://github.com/custom-components/ble_monitor).

Supports bindkey beacon encryption.

[Advertising formats](https://github.com/pvvx/ATC_MiThermometer/issues/298).

[Test program](https://github.com/pvvx/ATC_MiThermometer/tree/master/python-interface) of used Bluetooth Advertising formats provided by [Ircama](https://github.com/Ircama).

Only the [BTHome](https://www.home-assistant.io/integrations/bthome/) and Xiaomi support the native [HA Xiaomi Integration](https://www.home-assistant.io/integrations/xiaomi_ble/), for the others use the Custom Component [Passive BLE](https://github.com/custom-components/ble_monitor). 
Supports bindkey beacon encryption.

* (!) Custom Component [Passive BLE](https://github.com/custom-components/ble_monitor) does not [support Bluetooth 5.0](https://github.com/custom-components/ble_monitor/issues/1135) (LE Long Range).


#### atc1441 format:
UUID 0x181A - size 16 (temperature in 0.1%, humidity in 1%): [atc1441 format](https://github.com/atc1441/ATC_MiThermometer#advertising-format-of-the-custom-firmware)

#### Custom format (all data little-endian):  
UUID 0x181A - size 19: Custom extended format in 0.01 units (all data little-endian): 

   ```c
   uint8_t     size;   // = 18
   uint8_t     uid;    // = 0x16, 16-bit UUID
   uint16_t    UUID;   // = 0x181A, GATT Service 0x181A Environmental Sensing
   uint8_t     MAC[6]; // [0] - lo, .. [6] - hi digits
   int16_t     temperature;    // x 0.01 degree
   uint16_t    humidity;       // x 0.01 %
   uint16_t    battery_mv;     // mV
   uint8_t     battery_level;  // 0..100 %
   uint8_t     counter;        // measurement count
   uint8_t     flags;  // GPIO_TRG pin (marking "reset" on circuit board) flags: 
                       // bit0: Reed Switch, input
                       // bit1: GPIO_TRG pin output value (pull Up/Down)
                       // bit2: Output GPIO_TRG pin is controlled according to the set parameters
                       // bit3: Temperature trigger event
                       // bit4: Humidity trigger event
   ```

#### 'BTHome' format  

[The 'BTHome' format](https://bthome.io/)

### Encrypted beacon formats (uses bindkey):

* [Mijia standard format](https://github.com/pvvx/ATC_MiThermometer/blob/master/InfoMijiaBLE/README.md)
* [Energy efficient format (low security level)](https://github.com/pvvx/ATC_MiThermometer/issues/94#issuecomment-846984018)
* ['BTHome' encrypted](https://bthome.io/)

#### Xiaomi Battery Charge
3. UUID 0xFE95 - 0x0A: [Xiaomi](https://github.com/pvvx/ATC_MiThermometer/blob/master/InfoMijiaBLE/README.md) - battery charge level 0..100%, battery voltage in mV

#### Xiaomi Temperature & Humidity
4. UUID 0xFE95 - 0x0D: [Xiaomi](https://github.com/pvvx/ATC_MiThermometer/blob/master/InfoMijiaBLE/README.md) - temperature x0.1C, humidity x0.1%

#### Xiaomi Encrypted (bindkey enable)
5. UUID 0xFE95 - 0x0A, 0x04, 0x06 [Xiaomi](https://github.com/pvvx/ATC_MiThermometer/blob/master/InfoMijiaBLE/Mijia%20BLE%20Object%20Definition.md) - battery charge level 0..100%, temperature x0.1C, humidity x0.1% (All data are averaged over a period of 16 measurements)

### Bluetooth Connection Mode
+ Primary Service - Environmental Sensing Service (0x181A):
 * Characteristic UUID [0x2A1F](https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Characteristics/org.bluetooth.characteristic.temperature_celsius.xml) - Notify temperature x0.1C
 * Characteristic UUID [0x2A6E](https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Characteristics/org.bluetooth.characteristic.temperature.xml) - Notify temperature x0.01C
 * Characteristic UUID [0x2A6F](https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Characteristics/org.bluetooth.characteristic.humidity.xml) - Notify about humidity x0.01%
+ Primary Service - Battery Service (0x180F):
 * Characteristic UUID [0x2A19](https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Characteristics/org.bluetooth.characteristic.battery_level.xml) - Notify the battery charge level 0..99%
+ Primary Service (0x1F10):
 * Characteristic UUID [0x1F1F](https://github.com/pvvx/ATC_MiThermometer#primary-service-uuid-0x1f10-characteristic-uuid-0x1f1f) - Notify, frame id 0x33 (configuring or making a request): temperature x0.01C, humidity x0.01%, battery charge level 0..100%, battery voltage in mV, GPIO-pin flags and triggers.

### Temperature or humidity trigger (GPIO PA5 LYWSD03MMC label on the "reset" pin)

Xiaomi LYWSD03MMC has a PCB soldering test point. Available without disassembling the case and [marked "Reset"](https://github.com/pvvx/ATC_MiThermometer/tree/master/BoardPinout). 
Use as a trigger output with adjustable temperature and / or humidity hysteresis. 
Hysteresis and thresholds are set in TelinkMiFlasher.html. The output status is displayed in the advertising package.

![trg_menu](https://github.com/pvvx/ATC_MiThermometer/blob/master/img/trg_menu.gif)

![trg_grf](https://github.com/pvvx/ATC_MiThermometer/blob/master/img/trg_grf.gif)

Hysteresis: 
> * =0 off, 
> * if less than zero - activation on decrease, 
> * if more than zero - activation on excess

Setting the pin to "1" or "0" works if both hysteresis are set to zero (TRG off). 

![OnOff](https://github.com/pvvx/ATC_MiThermometer/blob/master/img/OnOff.gif)

![ESP01Relay](https://raw.githubusercontent.com/pvvx/pvvx.github.io/master/ATC_MiThermometer/img/TriggerESP01Relay.gif)

[Trigger and reed switch status receiver on JDY-10 module ($1).](https://github.com/pvvx/AdScanerTrg)

### Reed switch or button functions

For CGG1, MHO_C401, CGDK2, MJWSD05MMC, a button on the case is used.

It is possible to solder a reed switch on the LYWSD03MMC board to the pins marked "P8" (GPIO PA6) and GND.

The button on the body can turn the light on and off when creating a scenario in an external program...

The state of the reed switch or button is transferred to the advertising packet and events are sent in the "Switch" and "Ccounter" modes.

Setting the "Connect" option has several functions:

1. Short press for 80 seconds turns on the ability to connect with a device in BLE 4.2 format
2. Long press toggles temperature display to C or F.
3. Holding more than 20 seconds will reset the device settings to default. 

* [For MJWSD05MMC, the 'Connect' function is always configured on the top button](https://github.com/pvvx/ATC_MiThermometer/issues/307).

### Interface for receiving and displaying data on the LCD.
>* LCD shows: 
> * Big number: -99.5..1999.5 
> * Small number: -9..99
> * Smiley, battery, degrees
> * Setting the display time limit in sec

![ShowData](https://github.com/pvvx/ATC_MiThermometer/blob/master/img/ShowData.gif) 

### The USB-COM adapter writes the firmware in explorer. Web version.
>* Connect only TX-SWS and GND wires.

[USBCOMFlashTx.html](https://pvvx.github.io/ATC_MiThermometer/USBCOMFlashTx.html)

![USBCOMFlashTxHtml](https://github.com/pvvx/ATC_MiThermometer/blob/master/img/USBCOMFlashTxHtml.gif)


#### Chipset LYWSD03MMC HW:B1.4
> * TLSR8251F512ET24 (TLSR8258 in 24-pin TQFN). SoC: TC32 32-bit MCU 48Mhz, 64 KiB SRAM, 512 KiB Flash (GD25LE40C), Bluetooth 5.0: Mesh, 6LoWPAN, Zigbee, RF4CE, HomeKit, Long Range, Operating temperature: -40°C to +85°C, Power supply: 1.8V to 3.6V.
> * SHTV3 sensor. Measurement range: Temperature -40°C to +125°C, Humidity 0 to 100 %RH. Power supply: 1.8V to 3.6V
> * IST3055NA0 LCD controller 

[LYWSD03MMC B1.4 BoardPinout](https://github.com/pvvx/ATC_MiThermometer/blob/master/BoardPinout)

![TabPins](https://github.com/pvvx/ATC_MiThermometer/blob/master/BoardPinout/TabPins.gif)


#### Building the firmware

Windows:

1. Go to [wiki.telink-semi.cn](http://wiki.telink-semi.cn/wiki/IDE-and-Tools/IDE-for-TLSR8-Chips/) and get the IDE for TLSR8 Chips.
2. Install the IDE and import the 'ATC_MiThermometer' project
3. Compile the project

Ubuntu:

```
  git clone https://github.com/pvvx/ATC_MiThermometer.git
  cd ATC_MiThermometer
  make
```

## Related Work

ATC_MiThermometer is based on the original work of [@atc1441](https://twitter.com/atc1441), who developed the [initial custom firmware version and the web-based OTA flasher (Source)](https://github.com/atc1441/ATC_MiThermometer).


## Resources & Links
* [HTML files](https://github.com/pvvx/pvvx.github.io)

* [Telink Linux Tool Chain 2020-06-26](https://yadi.sk/d/pt_qTBB-t24i9A)

* [MHO-C401 Info](https://pvvx.github.io/MHO_C401/)

* [CGG1-Mijia Info](https://pvvx.github.io/CGG1)

* [Build Telink EVK on E104-BT10-G/N module (TLSR8269)](https://github.com/pvvx/TLSR8269-EVK)

* [Alternative programmer TLSR SWire on module TB-04/03F or E104-BT10](https://github.com/pvvx/TLSRPGM)

* [Additional information on the format Advertising for Xiaomi LYWSD03MMC](https://github.com/Magalex2x14/LYWSD03MMC-info)

* [Reading and recovering Mi-Home keys](https://github.com/pvvx/ATC_MiThermometer/blob/master/img/)

* [LYWSD03MMC Forum link (russian)](https://esp8266.ru/forum/threads/tlsr8251-lcd-termometr-lywsd03mmc-xiaomi-bluetooth-termometr.5263/)

* [MHO-C401 Forum link (russian)](https://esp8266.ru/forum/threads/tlsr8251f512et24-e-inc-display-termometr-mho-c401-bluetooth-termometr.5446/)

* [Solar powered Bluetooth Thermometer with Supercap](https://hackaday.io/project/177910-solar-powered-bluetooth-thermometer-with-supercap)

### CJMCU-2557 BQ25570

![CJMCU-2557](https://raw.githubusercontent.com/pvvx/pvvx.github.io/master/ATC_MiThermometer/img/CJMCU-2557.jpg)

## Control function ID when connected

#### Primary Service UUID 0x1F10, Characteristic UUID 0x1F1F

|  ID  | Command                                       |
| :--: | --------------------------------------------- |
| 0x01 | Get/Set device name                           |
| 0x02 | Get address of internal devices               |
| 0x03 | I2C scan                           		   |
| 0x04 | I2C Universal TRansaction (UTR)               |
| 0x10 | Get/Set MAC                                   |
| 0x11 | Get/Set Mi key: DevNameID                     |
| 0x12 | Get/Set Mi keys: Token & Bind                 |
| 0x15 | Get all Mi keys                               |
| 0x16 | Restore prev mi token & bindkeys              |
| 0x17 | Delete all Mi keys                            |
| 0x18 | Get/set binkey in EEP                         |
| 0x20 | Get/Set comfort parameters                    |
| 0x22 | Get/Set show LCD ext.data                     |
| 0x23 | Get/Set Time                                  |
| 0x24 | Get/set adjust time clock delta               |
| 0x33 | Start/Stop notify measures in connection mode |
| 0x35 | Read memory measures                          |
| 0x36 | Clear memory measures                         |
| 0x44 | Get/Set TRG config                            |
| 0x45 | Set TRG output pin                            |
| 0x4A | Get/Set TRG data (not save to Flash)          |
| 0x55 | Get/Set device config                         |
| 0x56 | Set default device config                     |
| 0x5A | Get/Set device config (not save to Flash)     |
| 0x60 | Get/Set LCD buffer                            |
| 0x61 | Start/Stop notify LCD buffer                  |
| 0x70 | Set PinCode                                   |
| 0x71 | Request Mtu Size Exchange                     |
| 0x72 | Set Reboot on disconnect                      |
| 0x73 | Extension BigOTA (MJWSD05MMC)                 |
| 0xDD | Reset LE Long Range mode                      |

---

![foto](https://raw.githubusercontent.com/pvvx/pvvx.github.io/master/SensorsTH.jpg)

