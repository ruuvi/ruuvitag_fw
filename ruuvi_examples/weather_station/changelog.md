# Weather station
## Current functionality
* Boots in RAW mode, data format 0x03.
* Sends and updates data once per second in RAW mode.
* Sends at 2 Hz, updates data once per 5 seconds in URL mode.
* Tx power is +4dBm.
* button press switches tag to URL mode (0x04).
* Current consumption is roughly 30 µA in room temperature @ 3.0 V.
* Blinks red led in RAW mode, Green led in URL mode.
* Turns green led on for a while at boot if sensors are responding over SPI.

## 1.1.0
* Boot in RAW mode, used to be URL mode.

## 1.0.0
* URL mode updated, 0x04.
* Sends at 4 dBm, 2 Hz in URL mode and 1 Hz in RAW mode.
* URL mode data is updated once per 5 seconds, RAW data is updated once per second.

## "Kickstarter"
* Has RAW and URL mode
* Boots in URL mode
* Sends at 0 dBm, 3 Hz.
