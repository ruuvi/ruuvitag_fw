/* Copyright (c) 2015 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

#ifndef BLUETOOTH_CONFIG_H
#define BLUETOOTH_CONFIG_H 

#define BLE_COMPANY_IDENTIFIER          0x0499                            /**< Ruuvi **/
#define APP_DEVICE_NAME                 "RuuviTag"                        //!< Advertised device name in the scan response when in connectable mode.

#define APP_BEACON_INFO_LENGTH          24                                //!< Manufacturer specific data length. in bytes

#define IS_SRVC_CHANGED_CHARACT_PRESENT 1                                 /**< Include the service changed characteristic. If not enabled, the server's database cannot be changed for the lifetime of the device. */

#define CENTRAL_LINK_COUNT              0                                 /**< Number of central links used by the application. When changing this number remember to adjust the RAM settings*/
#define PERIPHERAL_LINK_COUNT           0                                 /**< Number of peripheral links used by the application. When changing this number remember to adjust the RAM settings*/

#define APP_CFG_NON_CONN_ADV_TIMEOUT    0                                 /**< Time for which the device must be advertising in non-connectable mode (in seconds). 0 disables the time-out. */

#define APP_CFG_NON_CONN_ADV_INTERVAL_MS    MSEC_TO_UNITS(500, UNIT_0_625_MS) /**< The advertising interval for non-connectable advertisement (100 ms). This value can vary between 100 ms and 10.24 s). */

#define BLE_TX_POWER                    0                                 /** dBm **/

#endif
