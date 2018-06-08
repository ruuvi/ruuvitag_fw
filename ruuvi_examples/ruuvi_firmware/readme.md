# Ruuvi firmware
 * Used to be weather station, renamed to Ruuvi Firmware.
 * Boots up in RAW mode, i.e. data is sent in [Manufacturer specific format, Ruuvi V3](https://github.com/ruuvi/ruuvi-sensor-protocols).
 * Mode can be changed by pressing button "B"
 * Uses 16x IIR on BME280, which reduces noise at the expense of slowed response time. Practically the response is limited by enclosure, so
   slowdown is noticeabble only if the enclosure is open. 
 * Does not send data before sensors are read at least once to avoid invalid data transmissions.
 * ID and MAC address of data are readable via NFC
![screenshot 25 2 2018 11 58 57 ap](https://user-images.githubusercontent.com/2360368/36638828-6e86666c-1a39-11e8-9802-16a0142838ce.png)
 * 1 Hz +4 dBm transmission on RAW mode
 * 2 Hz +4 dBm transmission on URL mode. URL mode points to https://ruu.vi/#
 * Consumes approximately 22 uA in RAW mode, 40 uA in URL mode:
<img width="1023" alt="nayttokuva 2018-4-19 kello 20 40 54" src="https://user-images.githubusercontent.com/2360368/39012042-8db02510-4413-11e8-8eea-1ba204137d30.png">
 * Theoretical lifetime is approximately 4 years in RAW mode and 2 years in URL mode. 
 * Enter bootloader by holding "B" down while resetting, assuming you're running the "Full" hex which contains softdevice + bootloader + application.