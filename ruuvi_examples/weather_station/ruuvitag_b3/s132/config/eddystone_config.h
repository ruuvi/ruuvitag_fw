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

// Eddystone common data
#define APP_EDDYSTONE_UUID              0xFEAA                            /**< UUID for Eddystone beacons according to specification. */
#define APP_EDDYSTONE_RSSI              0xEE                              /**< 0xEE = -18 dB is the approximate signal strength at 0 m. */

// Eddystone UID data
#define APP_EDDYSTONE_UID_FRAME_TYPE    0x00                              /**< UID frame type is fixed at 0x00. */
#define APP_EDDYSTONE_UID_RFU           0x00, 0x00                        /**< Reserved for future use according to specification. */
#define APP_EDDYSTONE_UID_ID            0x01, 0x02, 0x03, 0x04, \
                                        0x05, 0x06                        /**< Mock values for 6-byte Eddystone UID ID instance.  */
#define APP_EDDYSTONE_UID_NAMESPACE     0xD5, 0x76, 0x28, 0xE8, \
                                        0x32, 0xB2, 0x73, 0x2A, \
                                        0x60, 0x17                        /**< First 10-bytes of SHA-1 of "ruu.vi" as Eddystone UID ID namespace. */

// Eddystone URL data
#define APP_EDDYSTONE_URL_FRAME_TYPE    0x10                              /**< URL Frame type is fixed at 0x10. */
#define APP_EDDYSTONE_URL_SCHEME        0x03                              /**< 0x03 = "https:" URL prefix scheme according to specification. */
#define APP_EDDYSTONE_URL_BASE          'r', 'u', 'u', '.', 'v', 'i', '/' /**< "nordicsemi.com". Last byte suffix 0x00 = ".com" according to specification. */
#define APP_EDDYSTONE_URL_BASE_LENGTH   7                                 
// Eddystone TLM data 
//#define APP_EDDYSTONE_TLM_FRAME_TYPE    0x20                              /**< TLM frame type is fixed at 0x20. */
//#define APP_EDDYSTONE_TLM_VERSION       0x00                              /**< TLM version might change in the future to accommodate other data according to specification. */
//#define APP_EDDYSTONE_TLM_BATTERY       0x00, 0xF0                        /**< Mock value. Battery voltage in 1 mV per bit. */
//#define APP_EDDYSTONE_TLM_TEMPERATURE   0x0F, 0x00                        /**< Mock value. Temperature [C]. Signed 8.8 fixed-point notation. */
//#define APP_EDDYSTONE_TLM_ADV_COUNT     0x00, 0x00, 0x00, 0x00            /**< Running count of advertisements of all types since power-up or reboot. */
//#define APP_EDDYSTONE_TLM_SEC_COUNT     0x00, 0x00, 0x00, 0x00            /**< Running count in 0.1 s resolution since power-up or reboot. */

#define DEAD_BEEF                       0xDEADBEEF                        /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

#endif //End include guard
