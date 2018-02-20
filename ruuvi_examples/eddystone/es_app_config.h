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

#ifndef EDDYSTONE_CONFIG_H
#define EDDYSTONE_CONFIG_H 

#include "bluetooth_config.h"
#include "bluetooth_board_config.h"

// EID security
#define MAC_RANDOMIZED //!< Configuration option to specify whether the BLE address should be randomized when advertising EIDs.

#define APP_CONFIG_LOCK_CODE {0x00, 0x11, 0x22, 0x33, \
                              0x44, 0x55, 0x66, 0x77, \
                              0x88, 0x99, 0xAA, 0xBB, \
                              0xCC, 0xDD, 0xEE, 0xFF} //!< Beacon lock code. @warning This lock code must be changed before going to production.

#ifndef APP_CONFIG_CALIBRATED_RANGING_DATA // in bluetooth_board_config
#error MISSING CALIBRATED DATA
#endif 

#define APP_CONFIG_TLM_TEMP_VBATT_UPDATE_INTERVAL_SECONDS   10                          //!< How often should the data in the TLM frame be updated.

#define APP_CONFIG_ADV_FRAME_SPACING_MS_MIN                 100                         //!< Minimum time between advertisement frames. Imposes limit on minumum accepted advertisement interval.
#if defined(NRF52)
#define APP_CONFIG_ADV_FRAME_ETLM_SPACING_MS                300                         //!< The time that is reqired for preparing an eTLM slot. Imposes limit on minimum accepted advertisement interval.
#elif defined(NRF51)
#define APP_CONFIG_ADV_FRAME_ETLM_SPACING_MS                600                         //!< The time that is reqired for preparing an eTLM slot. Imposes limit on minimum accepted advertisement interval.
#else
#error MISSING ETLM DELAY TIMING
#endif
#define ES_STOPWATCH_MAX_USERS                              4                           //!< Maximum amount of users that can be registered with the es_stopwatch module.

#define DEBUG_TIMING_INIT_VALUE             65280                                       //!< Initial time (as recommended by Google) to test the TK rollover behavior.
#define APP_CONFIG_TIMING_INIT_VALUE        DEBUG_TIMING_INIT_VALUE                     //!< Initializing value for the timing value of security slots.

#define APP_CONFIG_TLM_TEMP_INTERVAL_SECONDS  (30)                                      //!< How often should the temperature of the beacon be updated when TLM slot is configured.
#define APP_CONFIG_TLM_VBATT_INTERVAL_SECONDS (30)                                      //!< How often should the battery voltage of the beacon be updated when TLM slot is configured.    

// HW CONFIGS
#define BUTTON_REGISTRATION                 BUTTON_1                                    //!< Button to push when putting the beacon in registration mode.
#define USE_ECB_ENCRYPT_HW                  1                                           //!< Configuration option to use the hardware peripheral (1) or the software library (0) for ECB encryption (decryption always uses the software library).


// ES CONFIGS
#define APP_MAX_ADV_SLOTS                   5                 //!< Maximum number of advertisement slots.
#define APP_MAX_EID_SLOTS                   5                 /**< @brief Maximum number of EID slots.
                                                               * @note The maximum number of EID slots must be equal to the maximum number of advertisement slots (@ref APP_MAX_ADV_SLOTS). If your application does not adhere to this convention, you must modify the @ref eddystone_security module, because the security module maps the security slots' slot numbers 1 to 1 to the slots'. */
#define APP_ETLM_DELAY_MS                   300               //!< The delay that is introduced between advertisement slots of type eTLM.

// Broadcast Capabilities
#define APP_IS_VARIABLE_ADV_SUPPORTED       ESCS_BROADCAST_VAR_ADV_SUPPORTED_No         //!< Information whether variable advertisement intervals are supported.
#define APP_IS_VARIABLE_TX_POWER_SUPPORTED  ESCS_BROADCAST_VAR_TX_POWER_SUPPORTED_Yes   //!< Information whether variable advertisement TX power is supported.

#define APP_IS_UID_SUPPORTED                ESCS_FRAME_TYPE_UID_SUPPORTED_Yes           //!< Information whether the UID frame is supported.
#define APP_IS_URL_SUPPORTED                ESCS_FRAME_TYPE_URL_SUPPORTED_Yes           //!< Information whether the URL frame is supported.
#define APP_IS_TLM_SUPPORTED                ESCS_FRAME_TYPE_TLM_SUPPORTED_Yes           //!< Information whether the TLM frame is supported.
#define APP_IS_EID_SUPPORTED                ESCS_FRAME_TYPE_EID_SUPPORTED_Yes           //!< Information whether the EID frame is supported.

// Remain connectable
#define APP_IS_REMAIN_CONNECTABLE_SUPPORTED ESCS_FUNCT_REMAIN_CONNECTABLE_SUPPORTED_Yes //!< Information whether the 'remain connectable' option is supported.

// Eddystone common data
#define APP_ES_UUID              0xFEAA                            //!< UUID for Eddystone beacons according to specification.

// Eddystone UID data
#define APP_ES_UID_FRAME_TYPE    ES_FRAME_TYPE_UID                 //!< UID frame type (fixed at 0x00).
#define APP_ES_UID_NAMESPACE     0xAA, 0xAA, 0xBB, 0xBB, \
                                 0xCC, 0xCC, 0xDD, 0xDD, \
                                 0xEE, 0xEE                        //!< Mock values for 10-byte Eddystone UID ID namespace.
#define APP_ES_UID_ID            0x01, 0x02, 0x03, 0x04, \
                                 0x05, 0x06                        //!< Mock values for 6-byte Eddystone UID ID instance.
#define APP_ES_UID_RFU           0x00, 0x00                        //!< Reserved for future use according to specification.

// Eddystone URL data
#define APP_ES_URL_FRAME_TYPE    ES_FRAME_TYPE_URL                 //!< URL Frame type (fixed at 0x10).
#define APP_ES_URL_SCHEME        0x03                              //!< URL prefix scheme according to specification (0x03 = "https://").
#define APP_ES_URL_URL           'r', 'u', 'u', 'v', 'i', 0x07     //!< "ruuvi.com"

#define DEFAULT_FRAME_TYPE       APP_ES_URL_FRAME_TYPE             //!< Frame type of default frame.
#define DEFAULT_FRAME_TX_POWER   0x04                              //!< Default frame TX power.

/** @brief This value should mimic the data that would be written to the RW ADV Slot characteristic (for example, no RSSI for UID). */
#define DEFAULT_FRAME_DATA              {DEFAULT_FRAME_TYPE, DEFAULT_FRAME_TX_POWER, APP_ES_URL_SCHEME, APP_ES_URL_URL}
#define DEFAULT_FRAME_LENGTH            9                          //!< 1 - Frame Type, 1 - TX - power 1 - URL Scheme, URL - 6 = 9
#define APP_CFG_CONNECTABLE_ADV_TIMEOUT 60                         //!< seconds
#define APP_CFG_DEFAULT_RADIO_TX_POWER  4                          //dBm

#endif //End include guard
