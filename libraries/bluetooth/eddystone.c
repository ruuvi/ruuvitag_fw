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
#include "app_timer.h"
#include "eddystone_config.h"
#include "bluetooth_config.h"
#include "bsp.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"



static ble_gap_adv_params_t m_adv_params;                                 /**< Parameters to be passed to the stack when starting advertising. */

static uint8_t eddystone_url_data[19] =   /**< Information advertised by the Eddystone URL frame type. */
{
    APP_EDDYSTONE_URL_FRAME_TYPE,   // Eddystone URL frame type.
    APP_EDDYSTONE_RSSI,             // RSSI value at 0 m.
    APP_EDDYSTONE_URL_SCHEME,       // Scheme or prefix for URL ("http", "http://www", etc.)
    APP_EDDYSTONE_URL_BASE          // URL with a maximum length of 17 bytes. Last byte is suffix (".com", ".org", etc.)
};

/** @snippet [Eddystone UID data] */
//static uint8_t eddystone_uid_data[] =   /**< Information advertised by the Eddystone UID frame type. */
//{
//    APP_EDDYSTONE_UID_FRAME_TYPE,   // Eddystone UID frame type.
//    APP_EDDYSTONE_RSSI,             // RSSI value at 0 m.
//    APP_EDDYSTONE_UID_NAMESPACE,    // 10-byte namespace value. Similar to Beacon Major.
//    APP_EDDYSTONE_UID_ID,           // 6-byte ID value. Similar to Beacon Minor.
//    APP_EDDYSTONE_UID_RFU           // Reserved for future use.
//};
/** @snippet [Eddystone UID data] */

//static uint8_t eddystone_tlm_data[] =   /**< Information advertised by the Eddystone TLM frame type. */
//{
//    APP_EDDYSTONE_TLM_FRAME_TYPE,   // Eddystone TLM frame type.
//    APP_EDDYSTONE_TLM_VERSION,      // Eddystone TLM version.
//    APP_EDDYSTONE_TLM_BATTERY,      // Battery voltage in mV/bit.
//    APP_EDDYSTONE_TLM_TEMPERATURE,  // Temperature [C].
//    APP_EDDYSTONE_TLM_ADV_COUNT,    // Number of advertisements since power-up or reboot.
//    APP_EDDYSTONE_TLM_SEC_COUNT     // Time since power-up or reboot. 0.1 s increments.
//};

/**@brief Function for initializing the advertising functionality.
 *
 * @details Encodes the required advertising data and passes it to the stack.
 *          Also builds a structure to be passed to the stack when starting advertising.
 */
static void eddystone_advertising_init(char* url, uint8_t length)
{
    uint32_t      err_code;
    ble_advdata_t advdata;
    uint8_t       flags = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    ble_uuid_t    adv_uuids[] = {{APP_EDDYSTONE_UUID, BLE_UUID_TYPE_BLE}};

    uint8_array_t eddystone_data_array;                             // Array for Service Data structure.
/** @snippet [Eddystone data array] */

    eddystone_url_data[0] = APP_EDDYSTONE_URL_FRAME_TYPE;   // Eddystone URL frame type.
    eddystone_url_data[1] = APP_EDDYSTONE_RSSI;             // RSSI value at 0 m.
    eddystone_url_data[2] = APP_EDDYSTONE_URL_SCHEME;       // Scheme or prefix for URL ("http", "http://www", etc.)
    for (int ii = 0; ii < length; ii++)
    {
       eddystone_url_data[3+ii] = url[ii];  // URL with a maximum length of 17 bytes. Last byte is suffix (".com", ".org", etc.)
    }  

    eddystone_data_array.p_data = (uint8_t *) eddystone_url_data;   // Pointer to the data to advertise.
    eddystone_data_array.size = 3 + length;         // Size of the data to advertise.
/** @snippet [Eddystone data array] */

    ble_advdata_service_data_t service_data;                        // Structure to hold Service Data.
    service_data.service_uuid = APP_EDDYSTONE_UUID;                 // Eddystone UUID to allow discoverability on iOS devices.
    service_data.data = eddystone_data_array;                       // Array for service advertisement data.

    // Build and set advertising data.
    memset(&advdata, 0, sizeof(advdata));

    advdata.name_type               = BLE_ADVDATA_NO_NAME;
    advdata.flags                   = flags;
    advdata.uuids_complete.uuid_cnt = sizeof(adv_uuids) / sizeof(adv_uuids[0]);
    advdata.uuids_complete.p_uuids  = adv_uuids;
    advdata.p_service_data_array    = &service_data;                // Pointer to Service Data structure.
    advdata.service_data_count      = 1;

    err_code = ble_advdata_set(&advdata, NULL);
    APP_ERROR_CHECK(err_code);

    // Initialize advertising parameters (used when starting advertising).
    memset(&m_adv_params, 0, sizeof(m_adv_params));

    m_adv_params.type        = BLE_GAP_ADV_TYPE_ADV_NONCONN_IND;
    m_adv_params.p_peer_addr = NULL;                                // Undirected advertisement.
    m_adv_params.fp          = BLE_GAP_ADV_FP_ANY;
    m_adv_params.interval    = APP_CFG_NON_CONN_ADV_INTERVAL_MS;
    m_adv_params.timeout     = APP_CFG_NON_CONN_ADV_TIMEOUT;
}

void eddystone_advertise_url(char* url, uint8_t length)
{
    static bool advertising = false;
    // Update advertisement parameters, this is ok even if advertisement has started.
    eddystone_advertising_init(url, length);

    //Advertising needs to be started only once
    if(!advertising){
        uint32_t      err_code;
        err_code = sd_ble_gap_adv_start(&m_adv_params);
        APP_ERROR_CHECK(err_code);
        NRF_LOG_INFO("Advertising started\r\n");
        advertising = true;
    }
}
