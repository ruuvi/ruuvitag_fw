#ifndef BLUETOOTH_APP_CONFIG
#define BLUETOOTH_APP_CONFIG

#define APPLICATION_DEVICE_NAME "Ruuvi"
#define APP_DEVICE_NAME APPLICATION_DEVICE_NAME
#define APPLICATION_DEVICE_NAME_LENGTH 5
#define APP_DEVICE_NAME_LENGTH APPLICATION_DEVICE_NAME_LENGTH
#define APPLICATION_ADV_INTERVAL        1000                            /**< ms **/
#define APP_TX_POWER                    4                               /**< dBm **/
#define INIT_FWREV                      "Test 1.3.0"                    /**< Github tag **/
#define INIT_SWREV                      INIT_FWREV                      /**< Essentially same s FWrev since there is no separate SW (i.e. Espruino) **/

#endif