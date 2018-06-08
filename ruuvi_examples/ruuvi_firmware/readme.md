# Ruuvi firmware
 * Adds GATT connectivity with Device Information Service and DFU Service, as well as currently unused Nordic UART Service.
[!GATT](images/gatt.png)
 * Data is sent in [Manufacturer specific format, Ruuvi V5](https://github.com/ruuvi/ruuvi-sensor-protocols).
 * Tag is always connectable and advertises name "RuuviXXXX" in scan response.
 * Uses 16x IIR on BME280, which reduces noise at the expense of slowed response time. Practically the response is limited by enclosure, so
   slowdown is noticeable only if the enclosure is open. 
 * Does not send data before sensors are read at least once to avoid invalid data transmissions.
 * Sends at 10 Hz on boot to speed up detection of recently booted tags.
 * ID and MAC address of data are readable via NFC
[!NFC](images/nfc.png)
 * 1 Hz +4 dBm transmission on RAW mode
 * Consumes approximately 30 uA in RAW mode.
(!Profile)[images/profile.png]
 * Theoretical lifetime is approximately 3 years in RAW mode.
 * Enter bootloader by holding "B" down while resetting, assuming you're running the "Full" hex which contains softdevice + bootloader + application.