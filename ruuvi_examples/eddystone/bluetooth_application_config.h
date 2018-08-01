#ifndef BLUETOOTH_APP_CONFIG
#define BLUETOOTH_APP_CONFIG

#define APPLICATION_DEVICE_NAME "Ruuvi"
#define APP_DEVICE_NAME APPLICATION_DEVICE_NAME
#define APPLICATION_DEVICE_NAME_LENGTH 5
#define APP_DEVICE_NAME_LENGTH APPLICATION_DEVICE_NAME_LENGTH
#define APPLICATION_ADV_INTERVAL        500                             /**< ms **/
#define APP_TX_POWER                    4                               /**< dBm **/

#define INIT_FWREV                          "Eddystone_2.3.0"                             /**< Github tag **/
#define INIT_SWREV                          INIT_FWREV                                    /**< Practicially same thing, as there is no separate SW **/

#define APP_GATT_PROFILE_ENABLED 1

#endif