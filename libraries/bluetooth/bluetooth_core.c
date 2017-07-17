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

/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 * @return error code from BLE stack initialization, NRF_SUCCESS if init was ok
 */

#include "bluetooth_core.h"
#include "ble_advdata.h"
#include "ble_advertising.h"

#define NRF_LOG_MODULE_NAME "BLE_CORE"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#ifndef CENTRAL_LINK_COUNT
    #error "Please define CENTRAL_LINK_COUNT in bluetooth_config.h"
#endif
#ifndef PERIPHERAL_LINK_COUNT
    #error "Please define PERIPHERAL_LINK_COUNT in bluetooth_config.h"
#endif
#ifndef PERIPHERAL_LINK_COUNT
    #error "Please define PERIPHERAL_LINK_COUNT in bluetooth_config.h"
#endif
#ifndef BLE_COMPANY_IDENTIFIER
    #error "Please define BLE_COMPANY_IDENTIFIER in bluetooth_config.h"
#endif
#ifndef APP_CFG_NON_CONN_ADV_INTERVAL_MS
    #error "Please define APP_CFG_NON_CONN_ADV_INTERVAL_MS in bluetooth_config.h"
#endif
#ifndef APP_CFG_NON_CONN_ADV_TIMEOUT
    #error "APP_CFG_NON_CONN_ADV_TIMEOUT in bluetooth_config.h"
#endif


uint32_t ble_stack_init(void)
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

    return err_code;
}

/**
 * @brief Function to set BLE transmission power
 *  
 * @details set the BLE transmission power in dBm
 * @param int8_t power power in dBm, must be one of -40, -30, -20, -16, -12, -8, -4, 0, 4
 * @return error code, 0 if operation was success.
 */
uint32_t ble_tx_power_set(int8_t power)
{
    uint32_t err_code = sd_ble_gap_tx_power_set(power);
    APP_ERROR_CHECK(err_code);
    return err_code;
}

/**@brief Initialize advertising parameters. Parameters can be adjusted by calling this function again. 
 *
 * @details Initializes the BLE advertisement 
 *
 * @return error code from BLE stack initialization, NRF_SUCCESS if init was ok
 */
static uint16_t advertising_interval = APP_CFG_NON_CONN_ADV_INTERVAL_MS;
uint32_t bluetooth_advertising_init()
{
    static bool init = false;
    uint32_t err_code = NRF_SUCCESS;
    //Stop advertising before making adjustments
    if(init)
    {
      sd_ble_gap_adv_stop();
    }
    static ble_gap_adv_params_t m_adv_params;
    memset(&m_adv_params, 0, sizeof(m_adv_params));

    m_adv_params.type        = BLE_GAP_ADV_TYPE_ADV_NONCONN_IND;
    m_adv_params.p_peer_addr = NULL;                             // Undirected advertisement.
    m_adv_params.fp          = BLE_GAP_ADV_FP_ANY;
    m_adv_params.interval    = advertising_interval;
    m_adv_params.timeout     = APP_CFG_NON_CONN_ADV_TIMEOUT;

    err_code = sd_ble_gap_adv_start(&m_adv_params);
    if(NRF_SUCCESS != err_code)
    {
        NRF_LOG_INFO("Advertisement fail: %d \r\n",err_code);
    }
    init = true;
    return err_code;
}

/**@brief Function for advertising data. 
 *
 * @details Initializes the BLE advertisement with given data as manufacturer specific data.
 * Company ID is included by default and doesn't need to be included in parameter data.  
 *
 * @param data pointer to data to advertise, maximum length 24 bytes
 * @param length length of data to advertise
 *
 * @return error code from BLE stack initialization, NRF_SUCCESS if init was ok
 */
uint32_t bluetooth_advertise_data(uint8_t *data, uint8_t length)
{
    uint32_t      err_code;
    
    err_code = bluetooth_advertising_init();
    
    ble_advdata_t advdata;
    uint8_t       flags = BLE_GAP_ADV_FLAG_BR_EDR_NOT_SUPPORTED;

    ble_advdata_manuf_data_t manuf_specific_data;

    // Build and set advertising data.
    memset(&advdata, 0, sizeof(advdata));

    advdata.name_type             = BLE_ADVDATA_NO_NAME;
    advdata.flags                 = flags;
    advdata.p_manuf_specific_data = &manuf_specific_data;

    // Initialize advertising parameters (used when starting advertising).
    uint8_t *m_beacon_info = malloc(length);                   /**< Information advertised by the Beacon. */

    //m_beacon_info[0] =      0x02,                // Manufacturer specific information. Specifies the device type in this
                                                 // implementation.
    //m_beacon_info[1] =      length,              // Manufacturer specific information. Specifies the length of the
                                                 // manufacturer specific data in this implementation.
    memcpy(m_beacon_info, data, length);      // copy rest of data to broadcast

    manuf_specific_data.company_identifier = BLE_COMPANY_IDENTIFIER;
    manuf_specific_data.data.p_data = m_beacon_info;
    manuf_specific_data.data.size   =  length;

    err_code = ble_advdata_set(&advdata, NULL);
    APP_ERROR_CHECK(err_code);

    free(m_beacon_info);

    return err_code;
    
}

/**@brief Function adjusting advertising interval. 
 *
 * @details Sets the advertising interval in program.
 *         
 * @param interval advertisement interval in milliseconds, 100 - 10 000 
 *
 * @return 0 if value was updated, 1 if value was outside acceptable range, err_code from stack if stack returned error
 */
 uint32_t set_advertising_interval(uint16_t interval)
 {
   if(interval > 10000) return 1;
   if(interval < 100) return 1;
   advertising_interval = MSEC_TO_UNITS(interval, UNIT_0_625_MS);
   return bluetooth_advertising_init();
 }

