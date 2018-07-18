# Eddystone firmware
 
 * Uses Nordic's Eddystone implementation to provide Eddystone beacon capablities.
 * Enter configuration mode by doing NFC scan or pressing button "B"
 * Default password is (0x)00112233445566778899AABBCCDDEEFF
 * If you change your password, be sure to remember it. You'll need a wired connection to RuuviTag to reset forgotten password.
 * Advertises "https://ruuvi.com" at 2 Hz, +0 dBm at boot.  
 * TLM frames use nRF52 as temperature sensor, regardless of if BME280 is present. 

 # Changelog
 ## 2.3.0
 * Removes Ruuvi BLE code, uses fully Nordic code instead
 * Fixes issue with Eddystone EID ECDH ky exchange
 * Removes DFU and DIS services from the GATT profile 
 

