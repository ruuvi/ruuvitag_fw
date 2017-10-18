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

/** @file
 *
 * @defgroup experimental_ble_sdk_app_eddystone_main main.c
 * @{
 * @ingroup experimental_ble_sdk_app_eddystone
 * @brief Eddystone Beacon UID Transmitter sample application main file.
 *
 * This file contains the source code for an Eddystone beacon transmitter sample application.
 */

#include <stdbool.h>
#include <stdint.h>
#include "ble_advdata.h"
#include "nordic_common.h"
#include "softdevice_handler.h"
#include "nrf_error.h"
#include "sdk_errors.h"
#include "app_timer.h"
#include "bluetooth_config.h"
#include "bluetooth_core.h"
#include "bsp.h"
#include "es.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#include "bluetooth_core.h"
#define EDDYSTONE_UUID 0xFEAA
/**
 *  @brief Helper for advertising Eddystone URLs. 
 *
 *  @param advdata Advertisement data which will be filled with Eddystone URL
 *  @param url url to advertise. May include prefix and suffix bytes, such as 0x03 for https://
 *  @param length length of URL to advertise. 
 *  @return Error code, 0 on success
 */
ret_code_t eddystone_prepare_url_advertisement(ble_advdata_t* advdata, char* url, size_t length)
{
    if(length > 17) { return NRF_ERROR_INVALID_PARAM; }
    ble_uuid_t    adv_uuids[] = {{EDDYSTONE_UUID, BLE_UUID_TYPE_BLE}};

    uint8_array_t eddystone_data_array;                             // Array for Service Data structure.
    uint8_t rf_power[] = APP_CONFIG_CALIBRATED_RANGING_DATA;

    char eddystone_url_data[21] = {0};
    eddystone_url_data[0] = ES_FRAME_TYPE_URL;                      // Eddystone URL frame type.
    eddystone_url_data[1] = rf_power[7];                            // RSSI value at 0 m. at 0 dbm transmit. TODO
    for (int ii = 0; ii < length; ii++)
    {
       eddystone_url_data[2+ii] = url[ii];  // URL with a maximum length of 17 bytes. Last byte is suffix (".com", ".org", etc.)
    }  

    eddystone_data_array.p_data = (uint8_t *) eddystone_url_data;   // Pointer to the data to advertise.
    eddystone_data_array.size = 3 + length;                         // Size of the data to advertise.


    ble_advdata_service_data_t service_data;                        // Structure to hold Service Data.
    service_data.service_uuid = EDDYSTONE_UUID;                     // Eddystone UUID to allow discoverability on iOS devices.
    service_data.data = eddystone_data_array;                       // Array for service advertisement data.

    // Build and set advertising data.
    memset(advdata, 0, sizeof(ble_advdata_t));

    advdata->uuids_complete.uuid_cnt = sizeof(adv_uuids) / sizeof(adv_uuids[0]);
    advdata->uuids_complete.p_uuids  = adv_uuids;
    advdata->p_service_data_array    = &service_data;  // Pointer to Service Data structure.
    advdata->service_data_count      = 1;

    return NRF_SUCCESS;
}
