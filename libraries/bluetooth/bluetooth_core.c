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
#include "ble_advdata.h"
#include "nrf_assert.h"

#include "bluetooth_core.h"
#include "bluetooth_config.h"
#include "bsp.h"

#define NRF_LOG_MODULE_NAME "BLE_CORE"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#ifndef APP_COMPANY_IDENTIFIER
  #error Please define APP_COMPANY_INDENTIFIER in bluetooth_config.h
#endif
#ifndef CENTRAL_LINK_COUNT
  #error Please define CENTRAL_LINK_COUNT in bluetooth_config.h
#endif
#ifndef PERIPHERAL_LINK_COUNT
  #error Please define PERIPHERAL_LINK_COUNT in bluetooth_config.h
#endif
#ifndef APP_CFG_NON_CONN_ADV_INTERVAL_MS
  #error Please define APP_CFG_NON_CONN_ADV_INTERVAL_MS in bluetooth_config.h
#endif
#ifndef APP_CFG_NON_CONN_ADV_TIMEOUT
  #error Please define APP_CFG_NON_CONN_ADV_TIMEOUT in bluetooth_config.h
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
 * @brief Function to setsBLE transmission power
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

/**@brief Function for initializing the Advertising functionality.
 *
 * @details Encodes the required advertising data and passes it to the stack.
 *          Also builds a structure to be passed to the stack when starting advertising.
 *
 * @param m_beacon_info pointer to manufacturer specific data structure, for example:
 * uint8_t m_beacon_info[APP_BEACON_INFO_LENGTH] = 
 * {
 *   APP_DEVICE_TYPE,     // Manufacturer specific information. Specifies the device type in this
 *                        // implementation.
 *
 *   APP_ADV_DATA_LENGTH, // Manufacturer specific information. Specifies the length of the
 *                        // manufacturer specific data in this implementation.
 *   APP_BEACON_UUID,     // 128 bit UUID value.
 *   APP_MAJOR_VALUE,     // Major arbitrary value that can be used to distinguish between Beacons.
 *   APP_MINOR_VALUE,     // Minor arbitrary value that can be used to distinguish between Beacons.
 *   APP_MEASURED_RSSI    // Manufacturer specific information. The Beacon's measured TX power in
 *                        // this implementation.
 *  };
 *
 * @param info_size size of manufacturer specific data to advertise
 */
void advertising_init(uint8_t *m_beacon_info, uint8_t info_size)
{
    uint32_t      err_code;
    ble_advdata_t advdata;
    uint8_t       flags = BLE_GAP_ADV_FLAG_BR_EDR_NOT_SUPPORTED;

    ble_advdata_manuf_data_t manuf_specific_data;

    manuf_specific_data.company_identifier = APP_COMPANY_IDENTIFIER;
    manuf_specific_data.data.p_data = (uint8_t *) m_beacon_info;
    manuf_specific_data.data.size   = APP_BEACON_INFO_LENGTH;

    // Build and set advertising data.
    memset(&advdata, 0, sizeof(advdata));

    advdata.name_type             = BLE_ADVDATA_NO_NAME;
    advdata.flags                 = flags;
    advdata.p_manuf_specific_data = &manuf_specific_data;

    err_code = ble_advdata_set(&advdata, NULL);
    APP_ERROR_CHECK(err_code);

}

/**@brief Function for starting advertising.
 *
 * @param connectable
 * BLE_GAP_ADV_TYPE_ADV_IND         //Connectable, undirected, not implemented
 * BLE_GAP_ADV_TYPE_ADV_DIRECT_IND  //Connectable, directed, not implemented
 * BLE_GAP_ADV_TYPE_ADV_SCAN_IND    //Scannable, undirected, not implemented
 * BLE_GAP_ADV_TYPE_ADV_NONCONN_IND //Non-connectable, undirected, not implemented
 */
static ble_gap_adv_params_t m_adv_params;    
void advertising_start(uint8_t connectable)
{
    uint32_t err_code;

    // Initialize advertising parameters (used when starting advertising).
    memset(&m_adv_params, 0, sizeof(m_adv_params));

    switch(connectable)
    {
        case BLE_GAP_ADV_TYPE_ADV_NONCONN_IND:
            m_adv_params.type        = BLE_GAP_ADV_TYPE_ADV_NONCONN_IND;
            m_adv_params.p_peer_addr = NULL;                             // Undirected advertisement.
            m_adv_params.fp          = BLE_GAP_ADV_FP_ANY;
            m_adv_params.interval    = APP_CFG_NON_CONN_ADV_INTERVAL_MS;
            m_adv_params.timeout     = APP_CFG_NON_CONN_ADV_TIMEOUT;
            break;
        default :
            NRF_LOG_ERROR("Advertising mode not implemented\r\n");
            ASSERT(0);
            break;
    }

    err_code = sd_ble_gap_adv_start(&m_adv_params);
    APP_ERROR_CHECK(err_code);

    err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING);
    APP_ERROR_CHECK(err_code);
}

