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

#include "es.h"
#include "boards.h"
#include "bsp_config.h"
#include "init.h"

/**
 * @brief Bluetooth configuration settings for the application.
 * @details These configuration settings are defined in the application.
 */

// BLE CONFIGS
#define APP_COMPANY_IDENTIFIER              0xFFFF                                      //!< Test identifier, must be changed when in production
#define APP_DEVICE_NAME                     "RuuviTag"                                  //!< Advertised device name in the scan response when in connectable mode.
#define APP_BEACON_INFO_LENGTH              24                                          //!< Manufacturer specific data length. in bytes
#define IS_SRVC_CHANGED_CHARACT_PRESENT     0                                           //!< Information whether the service changed characteristic is available. If it is not enabled, the server's database cannot be changed for the lifetime of the device.
#define MAX_ADV_INTERVAL                   (10240)                                      //!< Maximum connection interval (in ms).
#define MIN_CONN_ADV_INTERVAL              (20)                                         //!< Minimum connection interval (in ms).
#define MIN_NON_CONN_ADV_INTERVAL          (100)                                        //!< Minimum advertisement interval for non-connectable advertisements (in ms).

#define CENTRAL_LINK_COUNT                  0                                           //!< Number of central links used by the application. When changing this number, remember to adjust the RAM settings.
#define PERIPHERAL_LINK_COUNT               1                                           //!< Number of peripheral links used by the application. When changing this number, remember to adjust the RAM settings.

#define APP_CFG_NON_CONN_ADV_TIMEOUT        0                                           //!< Time for which the device must be advertising in non-connectable mode (in seconds). 0 disables the time-out.
#define APP_CFG_NON_CONN_ADV_INTERVAL_MS    1000                                        //!< The advertising interval for non-connectable advertisement (in milliseconds). This value can vary between 100 ms and 10.24 s.
#define APP_CFG_CONNECTABLE_ADV_TIMEOUT     60                                          //!< Time for which the device must be advertising in connectable mode (in seconds). 0 disables the time-out.
#define APP_CFG_CONNECTABLE_ADV_INTERVAL_MS 100                                         //!< The advertising interval for connectable advertisement (in milliseconds). This value can vary between 20 ms and 10.24 s.

#define APP_CFG_DEFAULT_RADIO_TX_POWER      0x00                                        //!< Default TX power of the radio.

#define MIN_CONN_INTERVAL                   MSEC_TO_UNITS(50, UNIT_1_25_MS)             //!< Minimum acceptable connection interval (20 ms). The connection interval uses 1.25 ms units.
#define MAX_CONN_INTERVAL                   MSEC_TO_UNITS(90, UNIT_1_25_MS)             //!< Maximum acceptable connection interval (75 ms). The connection interval uses 1.25 ms units.
#define SLAVE_LATENCY                       0                                           //!< Slave latency.
#define CONN_SUP_TIMEOUT                    MSEC_TO_UNITS(4000, UNIT_10_MS)             //!< Connection supervision time-out (4 seconds). The supervision time-out uses 10 ms units.
#define FIRST_CONN_PARAMS_UPDATE_DELAY      APP_TIMER_TICKS(5000, APP_TIMER_PRESCALER)  //!< Time from initiating an event (connect or start of notification) to the first time @ref sd_ble_gap_conn_param_update is called (5 seconds).
#define NEXT_CONN_PARAMS_UPDATE_DELAY       APP_TIMER_TICKS(30000, APP_TIMER_PRESCALER) //!< Time between each call to @ref sd_ble_gap_conn_param_update after the first call (30 seconds).
#define MAX_CONN_PARAMS_UPDATE_COUNT        3                                           //!< Number of attempts before giving up the connection parameter negotiation.

#define BLE_TX_POWER                        0                                           //!< dBm 

#endif
