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

#endif
