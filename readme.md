# ESP32 Arduino PPP

This is example from ESP32 Arduino PPP
Original Link: https://github.com/espressif/arduino-esp32/tree/master/libraries/PPP/examples/PPP_Basic

# Note
Before build please install latest ESP32 Arduino version (From 3.x.x)

# Pin out

Pin | #Function | #Note
--- | --- | ---|
IO16 | UART_RX | ESP32_RX
IO17 | UART_TX | ESP32_TX
IO15 | RESET | (High to reset Modem)

# Log
```
rst:0x1 (POWERON_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
configsip: 0, SPIWP:0xee
clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
mode:DIO, clock div:1
load:0x3fff0030,len:1288
load:0x40078000,len:13872
load:0x40080400,len:4
ho 8 tail 4 room 4
load:0x40080404,len:3048
entry 0x40080590
Starting the modem. It might take a while!
PPP Started
Manufacturer: INCORPORATED
Model: A7600C
IMEI: 861779065998608
Waiting to connect to network...................23.6s
Attached: 1
State: 1
Operator: 45204
IMSI: 452048836643596
RSSI: 31
BER: 99
NetMode: 8
Switching to data mode...
PPP Got IP
Connected to internet!
PPP Connected
Attempting MQTT connection to test.mosquitto.org:1883
Connected to MQTT broker
Connection Success to google.com:80 OK
```
