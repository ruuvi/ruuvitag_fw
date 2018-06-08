# Eddystone
 * Advertises "https://ruuvi.com" at 2 Hz, +0 dBm by default.
 * Enter configuration mode by pressing "B" or connecting NFC
 * Default password 00112233445566778899AABBCCDDEEFF
    - *Important* If you change password and forget it, there is no way to recover the tag without a dev shield.
 * You can configure the beacon with Nordic's "nRF Beacon for Eddystone" app.
 * GATT profile has Device Information Service and DFU service (experimental)
 * _Important_ TLM-frames are not compatible with Google Beacon Tools for unknown reason. 
 * ID and MAC address of data are readable via NFC
![screenshot 25 2 2018 11 58 57 ap](https://user-images.githubusercontent.com/2360368/36638828-6e86666c-1a39-11e8-9802-16a0142838ce.png)
 * Consumes approximately 24 uA at boot, which theoretically gives 4 years of battery life at room temperature.
 * Enter bootloader by holding "B" down while resetting, assuming you're running the "Full" hex which contains softdevice + bootloader + application.
