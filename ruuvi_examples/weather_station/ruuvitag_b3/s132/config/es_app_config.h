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

// EID security
#define MAC_RANDOMIZED //!< Configuration option to specify whether the BLE address should be randomized when advertising EIDs.

#define APP_CONFIG_LOCK_CODE {0x00, 0x11, 0x22, 0x33, \
                              0x44, 0x55, 0x66, 0x77, \
                              0x88, 0x99, 0xAA, 0xBB, \
                              0xCC, 0xDD, 0xEE, 0xFF} //!< Beacon lock code. @warning This lock code must be changed before going to production.

#ifdef NRF52 
#define APP_CONFIG_CALIBRATED_RANGING_DATA {-49, -39, -29, -24, -19, -14, -9, -7, -5}   //!< Calibrated TX power at 0 m. See the nRF52 Product Specification for corresponding TX values.
#elif NRF51
#define APP_CONFIG_CALIBRATED_RANGING_DATA {-39, -26, -23, -18, -13, -12, -9, -2}       //!< Calibrated TX power at 0 m. See the nRF51 Product Specification for corresponding TX values.
#else
#error MISSING CALIBRATED DATA
#endif 

#define DEBUG_TIMING_INIT_VALUE             65280                                       //!< Initial time (as recommended by Google) to test the TK rollover behavior.
#define APP_CONFIG_TIMING_INIT_VALUE        DEBUG_TIMING_INIT_VALUE                     //!< Initializing value for the timing value of security slots.

#define APP_CONFIG_TLM_TEMP_INTERVAL_SECONDS  (30)                                      //!< How often should the temperature of the beacon be updated when TLM slot is configured.
#define APP_CONFIG_TLM_VBATT_INTERVAL_SECONDS (30)                                      //!< How often should the battery voltage of the beacon be updated when TLM slot is configured.    

// HW CONFIGS
#define BUTTON_REGISTRATION                 BUTTON_1                                    //!< Button to push when putting the beacon in registration mode.
#define USE_ECB_ENCRYPT_HW                  1                                           //!< Configuration option to use the hardware peripheral (1) or the software library (0) for ECB encryption (decryption always uses the software library).


// ES CONFIGS
#define APP_MAX_ADV_SLOTS                   5                 //!< Maximum number of advertisement slots.
#define APP_MAX_EID_SLOTS                   1                 /**< @brief Maximum number of EID slots.
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
#define APP_ES_UUID                  0xFEAA                        //!< UUID for Eddystone beacons according to specification.

// Eddystone UID data
#define APP_ES_UID_FRAME_TYPE    0x00                               //!< UID frame type (fixed at 0x00).
#define APP_ES_UID_NAMESPACE     0xD5, 0x76, 0x28, 0xE8, \
                                        0x32, 0xB2, 0x73, 0x2A, \
                                        0x60, 0x17                 /**< First 10-bytes of SHA-1 of "ruu.vi" as Eddystone UID ID namespace. */
#define APP_ES_UID_ID            0x01, 0x02, 0x03, 0x04, \
                                 0x05, 0x06                        //!< Mock values for 6-byte Eddystone UID ID instance.
#define APP_ES_UID_RFU           0x00, 0x00                        //!< Reserved for future use according to specification.

// Eddystone URL data
#define APP_ES_URL_FRAME_TYPE    0x10                              //!< URL Frame type (fixed at 0x10).
#define APP_ES_URL_SCHEME        0x03                              //!< URL prefix scheme according to specification (0x03 = "https://").
#define APP_ES_URL_URL           'r', 'u', 'u', '.', 'v', 'i', \
                                 '/', 's', 'e', 't', 'u', 'p'      //!< "ruu.vi/setup"

#define DEFAULT_FRAME_TYPE       APP_ES_URL_FRAME_TYPE             //!< Frame type of default frame.
#define DEFAULT_FRAME_TX_POWER   0x00                              //!< Default frame TX power.

/** @brief This value should mimic the data that would be written to the RW ADV Slot characteristic (for example, no RSSI for UID). */
#define DEFAULT_FRAME_DATA              {DEFAULT_FRAME_TYPE, DEFAULT_FRAME_TX_POWER, APP_ES_URL_SCHEME, APP_ES_URL_URL}
#define DEFAULT_FRAME_LENGTH            15                                //!< 1 - Frame Type, 1 - TX - power 1 - URL Scheme, URL - 12 = 15

#endif //End include guard
