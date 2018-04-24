#ifndef BLUETOOTH_APP_CONFIG
#define BLUETOOTH_APP_CONFIG

#define APPLICATION_DEVICE_NAME         "Ruuvi"                         /**< BLE name displayed in scan response. */
#define APPLICATION_DEVICE_NAME_LENGTH  5                               /**< number of characters in above string, excluding null */
#define APP_DEVICE_NAME                 APPLICATION_DEVICE_NAME         /**< TODO: Refactoring **/
#define APP_DEVICE_NAME_LENGTH          APPLICATION_DEVICE_NAME_LENGTH
#define APPLICATION_ADV_INTERVAL        1000                            /**< ms **/
#define APP_TX_POWER                    4                               /**< dBm **/
#define INIT_FWREV                      "Ruuvi 1.2.12"                  /**< Github tag **/
#define INIT_SWREV                      INIT_FWREV                      /**< Essentially same s FWrev since there is no separate SW (i.e. Espruino) **/


// milliseconds until main loop timer function is called. Other timers can bring
// application out of sleep at higher (or lower) interval.
#define MAIN_LOOP_INTERVAL_RAW       1000u
#define ADVERTISING_INTERVAL_RAW     1000u
#define MAIN_LOOP_INTERVAL_URL       5000u
#define ADVERTISING_INTERVAL_URL     500u
#define ADVERTISING_STARTUP_PERIOD   30000u //milliseconds app advertises at startup speed.
#define ADVERTISING_INTERVAL_STARTUP 100u   //milliseconds app advertises at startup speed.



// Base length includes URL scheme prefix, URL is 17 bytes
#define URL_BASE_LENGTH 9
#define URL_DATA_LENGTH 9
#define URL_BASE {0x03, 'r', 'u', 'u', '.', 'v', 'i', '/', '#'}; // https://ruu.vi/#
//Raw v2
#define RAW_DATA_LENGTH 14

/**
 *  BLE_GAP_ADV_TYPE_ADV_IND   0x00           Connectable, scannable
 *  BLE_GAP_ADV_TYPE_ADV_DIRECT_IND   0x01
 *  BLE_GAP_ADV_TYPE_ADV_SCAN_IND   0x02      Nonconnectable, scannable
 *  BLE_GAP_ADV_TYPE_ADV_NONCONN_IND   0x03   Nonconnectable, nonscannable
 */
#define APPLICATION_ADVERTISEMENT_TYPE 0x03
//Set to 0 if you don't want to include GATT connectivity. Remember to adjust advertisement type
#define APPLICATION_GATT 1

#endif
