#ifndef BLUETOOTH_APP_CONFIG
#define BLUETOOTH_APP_CONFIG

#define APPLICATION_DEVICE_NAME         "Ruuvi"                         /**< BLE name displayed in scan response. */
#define APPLICATION_DEVICE_NAME_LENGTH  5                               /**< number of characters in above string, excluding null */
#define APP_DEVICE_NAME                 APPLICATION_DEVICE_NAME         /**< TODO: Refactoring **/
#define APP_DEVICE_NAME_LENGTH          APPLICATION_DEVICE_NAME_LENGTH
#define APPLICATION_ADV_INTERVAL        1000                            /**< ms **/
#define APP_TX_POWER                    4                               /**< dBm **/
#define INIT_FWREV                      "2.2.0"                         /**< Github tag. Do not include specifiers such as "alpha" so you can accept ready binaries as they are **/
#define INIT_SWREV                      INIT_FWREV                      /**< FW and SW are same thing in this context **/

// milliseconds until main loop timer function is called. Other timers can bring
// application out of sleep at higher (or lower) interval.
#define MAIN_LOOP_INTERVAL_RAW   1000u
#define ADVERTISING_INTERVAL_RAW 1000u
#define MAIN_LOOP_INTERVAL_URL   5000u
#define ADVERTISING_INTERVAL_URL  500u



// Base length includes URL scheme prefix, URL is 17 bytes
#define URL_BASE_LENGTH 9
#define URL_DATA_LENGTH 9
#define URL_BASE {0x03, 'r', 'u', 'u', '.', 'v', 'i', '/', '#'}; // https://ruu.vi/#
//Raw v2
#define RAW_DATA_LENGTH 24

/**
 *  BLE_GAP_ADV_TYPE_ADV_IND   0x00           Connectable, scannable
 *  BLE_GAP_ADV_TYPE_ADV_DIRECT_IND   0x01
 *  BLE_GAP_ADV_TYPE_ADV_SCAN_IND   0x02      Nonconnectable, scannable
 *  BLE_GAP_ADV_TYPE_ADV_NONCONN_IND   0x03   Nonconnectable, nonscannable
 */
#define APPLICATION_ADVERTISEMENT_TYPE 0x00

#endif
