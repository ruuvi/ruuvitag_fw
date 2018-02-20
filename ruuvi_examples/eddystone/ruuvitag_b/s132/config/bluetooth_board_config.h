#ifndef BLUETOOTH_BOARD_CONFIG_H
#define BLUETOOTH_BOARD_CONFIG_H

#define BLE_COMPANY_IDENTIFIER          0x0499                            /**< Ruuvi **/

/** Device Information Service initial values **/
#define SERIAL_LENGTH                       9                                             /**< 2 UINT32_t hex-encoded + NULL */
#define INIT_MANUFACTURER                   "Ruuvi Innovations Ltd"
#define INIT_MODEL                          "RuuviTag"
#define INIT_HWREV                          "B"
#define INIT_FWREV                          "2.0.0-alpha"                                 /**< Github tag **/
#define INIT_SWREV                          "Eddystone"                                   /**< FW revision **/

#endif
