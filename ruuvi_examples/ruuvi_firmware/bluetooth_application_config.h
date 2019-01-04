#ifndef BLUETOOTH_APP_CONFIG
#define BLUETOOTH_APP_CONFIG

#define APPLICATION_DEVICE_NAME         "Ruuvi"                         /**< BLE name displayed in scan response. */
#define APPLICATION_DEVICE_NAME_LENGTH  5                               /**< number of characters in above string, excluding null */
#define APP_DEVICE_NAME                 APPLICATION_DEVICE_NAME         /**< TODO: Refactoring **/
#define APP_DEVICE_NAME_LENGTH          APPLICATION_DEVICE_NAME_LENGTH
#define APPLICATION_ADV_INTERVAL        1010                            /**< ms. Use value which is not exactly divisible by 1000 ms to be seen by gateways which have limited scan windows in second divisible intervals. **/
#define APP_TX_POWER                    4                               /**< dBm **/
#define INIT_FWREV                      "2.4.0"                         /**< Github tag. Do not include specifiers such as "alpha" so you can accept ready binaries as they are **/
#define INIT_SWREV                      INIT_FWREV                      /**< FW and SW are same thing in this context **/                             

// milliseconds until main loop timer function is called. Other timers can bring
// application out of sleep at higher (or lower) interval.
#define MAIN_LOOP_INTERVAL_RAW        2010u
#define ADVERTISING_INTERVAL_RAW      2010u
#define MAIN_LOOP_INTERVAL_RAW_SLOW   6010u
#define ADVERTISING_INTERVAL_RAW_SLOW 6010u
#define ADVERTISING_STARTUP_PERIOD    5000u // milliseconds app advertises at startup speed.
#define ADVERTISING_INTERVAL_STARTUP  100u  // Interval of startup advertising

//Raw v2
#define RAWv1_DATA_LENGTH 14
#define RAWv2_DATA_LENGTH 24

/**
 *  BLE_GAP_ADV_TYPE_ADV_IND   0x00           Connectable, scannable
 *  BLE_GAP_ADV_TYPE_ADV_DIRECT_IND   0x01
 *  BLE_GAP_ADV_TYPE_ADV_SCAN_IND   0x02      Nonconnectable, scannable
 *  BLE_GAP_ADV_TYPE_ADV_NONCONN_IND   0x03   Nonconnectable, nonscannable
 */
// Most of the time non-connectable
#define APPLICATION_ADVERTISEMENT_TYPE 0x03

//Set to 0 if you don't want to include GATT connectivity. Remember to adjust advertisement type
#define APP_GATT_PROFILE_ENABLED        0

// After boot or user interaction, such as button press and NFC read
#if APP_GATT_PROFILE_ENABLED
  #define STARTUP_ADVERTISEMENT_TYPE     0x03
#else
  #define STARTUP_ADVERTISEMENT_TYPE     0x00
#endif

#endif