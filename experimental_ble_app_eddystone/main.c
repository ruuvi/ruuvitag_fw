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
#include "bsp.h"
#include "app_timer.h"

#define IS_SRVC_CHANGED_CHARACT_PRESENT 0                                 /**< Include the service changed characteristic. If not enabled, the server's database cannot be changed for the lifetime of the device. */

#define CENTRAL_LINK_COUNT              0                                 /**< Number of central links used by the application. When changing this number remember to adjust the RAM settings*/
#define PERIPHERAL_LINK_COUNT           0                                 /**< Number of peripheral links used by the application. When changing this number remember to adjust the RAM settings*/

#define APP_CFG_NON_CONN_ADV_TIMEOUT    0                                 /**< Time for which the device must be advertising in non-connectable mode (in seconds). 0 disables the time-out. */
#define NON_CONNECTABLE_ADV_INTERVAL    MSEC_TO_UNITS(100, UNIT_0_625_MS) /**< The advertising interval for non-connectable advertisement (100 ms). This value can vary between 100 ms and 10.24 s). */

// Eddystone common data
#define APP_EDDYSTONE_UUID              0xFEAA                            /**< UUID for Eddystone beacons according to specification. */
#define APP_EDDYSTONE_RSSI              0xEE                              /**< 0xEE = -18 dB is the approximate signal strength at 0 m. */

// Eddystone UID data
#define APP_EDDYSTONE_UID_FRAME_TYPE    0x00                              /**< UID frame type is fixed at 0x00. */
#define APP_EDDYSTONE_UID_RFU           0x00, 0x00                        /**< Reserved for future use according to specification. */
#define APP_EDDYSTONE_UID_ID            0x01, 0x02, 0x03, 0x04, \
                                        0x05, 0x06                        /**< Mock values for 6-byte Eddystone UID ID instance.  */
#define APP_EDDYSTONE_UID_NAMESPACE     0xAA, 0xAA, 0xBB, 0xBB, \
                                        0xCC, 0xCC, 0xDD, 0xDD, \
                                        0xEE, 0xEE                        /**< Mock values for 10-byte Eddystone UID ID namespace. */

// Eddystone URL data
#define APP_EDDYSTONE_URL_FRAME_TYPE    0x10                              /**< URL Frame type is fixed at 0x10. */
#define APP_EDDYSTONE_URL_SCHEME        0x00                              /**< 0x00 = "http://www" URL prefix scheme according to specification. */
#define APP_EDDYSTONE_URL_URL           0x6e, 0x6f, 0x72, 0x64, \
                                        0x69, 0x63, 0x73, 0x65, \
                                        0x6d,0x69, 0x00                   /**< "nordicsemi.com". Last byte suffix 0x00 = ".com" according to specification. */
// Eddystone TLM data
#define APP_EDDYSTONE_TLM_FRAME_TYPE    0x20                              /**< TLM frame type is fixed at 0x20. */
#define APP_EDDYSTONE_TLM_VERSION       0x00                              /**< TLM version might change in the future to accommodate other data according to specification. */
#define APP_EDDYSTONE_TLM_BATTERY       0x00, 0xF0                        /**< Mock value. Battery voltage in 1 mV per bit. */
#define APP_EDDYSTONE_TLM_TEMPERATURE   0x0F, 0x00                        /**< Mock value. Temperature [C]. Signed 8.8 fixed-point notation. */
#define APP_EDDYSTONE_TLM_ADV_COUNT     0x00, 0x00, 0x00, 0x00            /**< Running count of advertisements of all types since power-up or reboot. */
#define APP_EDDYSTONE_TLM_SEC_COUNT     0x00, 0x00, 0x00, 0x00            /**< Running count in 0.1 s resolution since power-up or reboot. */

#define DEAD_BEEF                       0xDEADBEEF                        /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

#define APP_TIMER_PRESCALER             0                                 /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_OP_QUEUE_SIZE         4                                 /**< Size of timer operation queues. */

static ble_gap_adv_params_t m_adv_params;                                 /**< Parameters to be passed to the stack when starting advertising. */

static uint8_t eddystone_url_data[] =   /**< Information advertised by the Eddystone URL frame type. */
{
    APP_EDDYSTONE_URL_FRAME_TYPE,   // Eddystone URL frame type.
    APP_EDDYSTONE_RSSI,             // RSSI value at 0 m.
    APP_EDDYSTONE_URL_SCHEME,       // Scheme or prefix for URL ("http", "http://www", etc.)
    APP_EDDYSTONE_URL_URL           // URL with a maximum length of 17 bytes. Last byte is suffix (".com", ".org", etc.)
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

/**@brief Callback function for asserts in the SoftDevice.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in]   line_num   Line number of the failing ASSERT call.
 * @param[in]   file_name  File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}

/**@brief Function for initializing the advertising functionality.
 *
 * @details Encodes the required advertising data and passes it to the stack.
 *          Also builds a structure to be passed to the stack when starting advertising.
 */
static void advertising_init(void)
{
    uint32_t      err_code;
    ble_advdata_t advdata;
    uint8_t       flags = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    ble_uuid_t    adv_uuids[] = {{APP_EDDYSTONE_UUID, BLE_UUID_TYPE_BLE}};

    uint8_array_t eddystone_data_array;                             // Array for Service Data structure.
/** @snippet [Eddystone data array] */
    eddystone_data_array.p_data = (uint8_t *) eddystone_url_data;   // Pointer to the data to advertise.
    eddystone_data_array.size = sizeof(eddystone_url_data);         // Size of the data to advertise.
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
    m_adv_params.interval    = NON_CONNECTABLE_ADV_INTERVAL;
    m_adv_params.timeout     = APP_CFG_NON_CONN_ADV_TIMEOUT;
}


/**@brief Function for starting advertising.
 */
static void advertising_start(void)
{
    uint32_t err_code;

    err_code = sd_ble_gap_adv_start(&m_adv_params);
    APP_ERROR_CHECK(err_code);

    err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 */
static void ble_stack_init(void)
{
    uint32_t err_code;
    
    nrf_clock_lf_cfg_t clock_lf_cfg = NRF_CLOCK_LFCLKSRC;
    
    // Initialize the SoftDevice handler module.
    SOFTDEVICE_HANDLER_INIT(&clock_lf_cfg, NULL);
    
    ble_enable_params_t ble_enable_params;
    err_code = softdevice_enable_get_default_config(CENTRAL_LINK_COUNT,
                                                    PERIPHERAL_LINK_COUNT,
                                                    &ble_enable_params);
    APP_ERROR_CHECK(err_code);
    
    //Check the ram settings against the used number of links
    CHECK_RAM_START_ADDR(CENTRAL_LINK_COUNT,PERIPHERAL_LINK_COUNT);
    
    // Enable BLE stack.
    err_code = softdevice_enable(&ble_enable_params);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for doing power management.
 */
static void power_manage(void)
{
    uint32_t err_code = sd_app_evt_wait();
    APP_ERROR_CHECK(err_code);
}

/**
 * @brief Function for application main entry.
 */
int main(void)
{
    uint32_t err_code;
    // Initialize.
    APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, false);
    err_code = bsp_init(BSP_INIT_LED, APP_TIMER_TICKS(100, APP_TIMER_PRESCALER), NULL);
    APP_ERROR_CHECK(err_code);
    ble_stack_init();
    advertising_init();
    LEDS_ON(LEDS_MASK);
    // Start execution.
    advertising_start();

    // Enter main loop.
    for (;; )
    {
        power_manage();
    }
}


/**
 * @}
 */
