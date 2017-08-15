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
#include "sdk_errors.h"
#include "nrf_delay.h"

#include "ble_event_handlers.h" 
#include "bluetooth_config.h"

#include "application_service_if.h"

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

static int8_t tx_power = 0;
//https://infocenter.nordicsemi.com/index.jsp?topic=%2Fcom.nordic.infocenter.s132.api.v3.0.0%2Fstructble__gap__adv__params__t.html
static ble_gap_adv_params_t m_adv_params = {
   // BLE_ADV_MODE_IDLE, 
   // BLE_ADV_MODE_DIRECTED, 
   // BLE_ADV_MODE_DIRECTED_SLOW, 
   // BLE_ADV_MODE_FAST, 
   // BLE_ADV_MODE_SLOW 
  .type = BLE_GAP_ADV_TYPE_ADV_NONCONN_IND, 
  
  // NULL on undirected advertisement, peer address on directed
  .p_peer_addr = NULL,

  // BLE_GAP_ADV_FP_ANY            Allow scan requests and connect requests from any device.
  // BLE_GAP_ADV_FP_FILTER_SCANREQ Filter both scan and connect requests with whitelist.   
  // BLE_GAP_ADV_FP_FILTER_CONNREQ Filter connect requests with whitelist.
  // BLE_GAP_ADV_FP_FILTER_BOTH    Filter scan requests with whitelist.
  .fp = BLE_GAP_ADV_FP_ANY,
  
  //Advertising interval between 0x0020 and 0x4000 in 0.625 ms units (20ms to 10.24s),
  .interval    = APP_CFG_NON_CONN_ADV_INTERVAL_MS,
  
  //Advertising timeout between 0x0001 and 0x3FFF in seconds, 0x0000 disables timeout. 
  .timeout     = APP_CFG_NON_CONN_ADV_TIMEOUT
};

//https://infocenter.nordicsemi.com/topic/com.nordic.infocenter.sdk5.v12.3.0/group__ble__sdk__lib__advdata.html?cp=4_0_3_6_2_8
ble_advdata_t advdata =
{
// BLE_ADVDATA_NO_NAME
// BLE_ADVDATA_SHORT_NAME
// BLE_ADVDATA_FULL_NAME
 .name_type = BLE_ADVDATA_NO_NAME, //scan response
 .short_name_len = 5, //Name get truncated to "Ruuvi" if full name does not fit
 .include_appearance = false, // scan response
 .flags = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE, // Low energy, discoverable
 .p_tx_power_level        = &tx_power,
 .uuids_more_available    = {.uuid_cnt = 0, .p_uuids = NULL},
 .uuids_complete          = {.uuid_cnt = 0, .p_uuids = NULL},
 .uuids_solicited         = {.uuid_cnt = 0, .p_uuids = NULL},
 .p_slave_conn_int        = NULL,
 .p_manuf_specific_data   = NULL,
 .p_service_data_array    = NULL,
 .service_data_count      = 0,
 .include_ble_device_addr = true,
 .le_role                 = BLE_ADVDATA_ROLE_NOT_PRESENT, //always when on BLE
 .p_tk_value              = NULL, //always when on BLE
 .p_sec_mgr_oob_flags     = NULL, //always when on BLE
 .p_lesc_data             = NULL  //always when on BLE
};

ble_advdata_t scanresp =
{
 .name_type = BLE_ADVDATA_FULL_NAME, //scan response
 .short_name_len = 5,                //Name get truncated to "Ruuvi" if full name does not fit
 .include_appearance = true,         // scan response
 .flags = 0, // Flags shall not be included in the scan response data.
 .p_tx_power_level        = &tx_power,
 .uuids_more_available    = {.uuid_cnt = 0, .p_uuids = NULL},    // Add some services?
 .uuids_complete          = {.uuid_cnt = 0, .p_uuids = NULL},
 .uuids_solicited         = {.uuid_cnt = 0, .p_uuids = NULL},
 .p_slave_conn_int        = NULL,
 .p_manuf_specific_data   = NULL,
 .p_service_data_array    = NULL,
 .service_data_count      = 0,
 .include_ble_device_addr = true,
 .le_role                 = BLE_ADVDATA_ROLE_NOT_PRESENT, //always when on BLE
 .p_tk_value              = NULL, //always when on BLE
 .p_sec_mgr_oob_flags     = NULL, //always when on BLE
 .p_lesc_data             = NULL //always when on BLE
};
 
 /**
 * @brief Function adjusting advertising interval. 
 * @details Sets the advertising interval in program.    
 * @param interval advertisement interval in milliseconds, 100 - 10 000 
 */
void configure_advertising_interval(uint16_t interval)
 {
   if(interval > 10000) return;
   if(interval < 100) return;
   m_adv_params.interval = MSEC_TO_UNITS(interval, UNIT_0_625_MS);
 }
 
 /**
 * @brief Function for configuring advertisement type
 * @details https://infocenter.nordicsemi.com/index.jsp?topic=%2Fcom.nordic.infocenter.s132.api.v3.0.0%2Fgroup___b_l_e___g_a_p___a_d_v___t_y_p_e_s.html
 * @param type Advertisement type, 0 ... 3
 */
void configure_advertisement_type(uint8_t type)
 {
  //TODO: Handle invalid parameter
  if(type > 3){ return; }
  m_adv_params.type = type;
 }
 
/**
 * @brief Function for configuring advertisement filter policy
 * @details  https://infocenter.nordicsemi.com/index.jsp?topic=%2Fcom.nordic.infocenter.s132.api.v3.0.0%2Fstructble__gap__adv__params__t.html
 * @param fp Advertisement filter policy, 0 ... 3
 */
 void configure_advertisement_filter(uint8_t fp)
 {
  //TODO: Handle invalid parameter
  if(fp > 3){ return; }
  m_adv_params.fp = fp;
 }
 
/**
 * @brief Function for configuring advertisement timeout
 * @details  https://infocenter.nordicsemi.com/index.jsp?topic=%2Fcom.nordic.infocenter.s132.api.v3.0.0%2Fstructble__gap__adv__params__t.html
 * @param fp Advertisement timout in seconds, 0x0001 ... 0x3FFF, 0 to disable
 */
 void configure_advertisement_timout(uint16_t timeout)
 {
  //TODO: Handle invalid parameter
  if(timeout > 0x3FFF){ return; }
  m_adv_params.timeout = timeout;
 }

uint32_t ble_apply_configuration()
{
  return bluetooth_advertising_start();
}

/**@brief Function for the GAP initialization.
 *
 * @details This function will set up all the necessary GAP (Generic Access Profile) parameters of
 *          the device. It also sets the permissions and appearance.
 */
static void gap_params_init(void)
{
    uint32_t                err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *) APP_DEVICE_NAME,
                                          strlen(APP_DEVICE_NAME));
    APP_ERROR_CHECK(err_code);

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing the Connection Parameters module.
 */
static void conn_params_init(void)
{
    uint32_t               err_code;
    ble_conn_params_init_t cp_init;

    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params                  = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail             = false;
    cp_init.evt_handler                    = on_conn_params_evt;
    cp_init.error_handler                  = conn_params_error_handler;

    err_code = ble_conn_params_init(&cp_init);
    NRF_LOG_INFO("Conn params init status %s\r\n", (uint32_t)ERR_TO_STR(err_code));
    NRF_LOG_FLUSH();
    nrf_delay_ms(10);
    APP_ERROR_CHECK(err_code);
}

uint32_t ble_stack_init(void)
{
    uint32_t err_code;

    nrf_clock_lf_cfg_t clock_lf_cfg = NRF_CLOCK_LFCLKSRC;
    NRF_LOG_INFO("Softdevice handler init start\r\n");
    NRF_LOG_FLUSH();

    // Initialize the SoftDevice handler module.
    SOFTDEVICE_HANDLER_INIT(&clock_lf_cfg, NULL);

    NRF_LOG_INFO("Softdevice handler init done\r\n");      
    NRF_LOG_FLUSH();

    ble_enable_params_t ble_enable_params;
    err_code = softdevice_enable_get_default_config(CENTRAL_LINK_COUNT,
                                                    PERIPHERAL_LINK_COUNT,
                                                    &ble_enable_params);
    NRF_LOG_INFO("Softdevice configuration ready, status: %s\r\n", (uint32_t)ERR_TO_STR(err_code));       
    NRF_LOG_FLUSH();                                             
    APP_ERROR_CHECK(err_code);

    //Check the ram settings against the used number of links
    CHECK_RAM_START_ADDR(CENTRAL_LINK_COUNT,PERIPHERAL_LINK_COUNT);
    NRF_LOG_INFO("RAM checked\r\n");
    
    #if (NRF_SD_BLE_API_VERSION == 3)
      ble_enable_params.gatt_enable_params.att_mtu = NRF_BLE_MAX_MTU_SIZE;
    #endif

    // Subscribe for BLE events.
    err_code = softdevice_ble_evt_handler_set(ble_evt_dispatch);
    APP_ERROR_CHECK(err_code);

    // Enable BLE stack.
    err_code = softdevice_enable(&ble_enable_params);
    NRF_LOG_INFO("Softdevice enabled, status: %s\r\n", (uint32_t)ERR_TO_STR(err_code));
    APP_ERROR_CHECK(err_code);
    
    gap_params_init();
    NRF_LOG_INFO("GAP params init\r\n");
    NRF_LOG_FLUSH();
    nrf_delay_ms(20);
    application_services_init();
    bluetooth_advertise_data();
    bluetooth_advertising_start();
    conn_params_init();

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

/**
 * @brief Start advertising. 
 * BLE stack and advertising must be init. Also advertisement data should be set.
 *
 * @return error code from BLE stack, NRF_SUCCESS if ok.
 */
static bool advertising = false;
uint32_t bluetooth_advertising_start(void)
{

    uint32_t err_code = NRF_SUCCESS;
    //Stop advertising before making adjustments
    if(advertising)
    {
      err_code = sd_ble_gap_adv_stop();
    }

    err_code |= sd_ble_gap_adv_start(&m_adv_params);
    if(NRF_SUCCESS != err_code)
    {
        NRF_LOG_INFO("Advertisement fail: %d \r\n",err_code);
    }
    advertising = true;
    return err_code;
}

/**
 * @brief Stop advertising. 
 *
 * @return error code from BLE stack, NRF_SUCCESS if ok.
 */
uint32_t bluetooth_advertising_stop(void)
{

    uint32_t err_code = NRF_SUCCESS;
    //Stop advertising before making adjustments
    if(advertising)
    {
      err_code = sd_ble_gap_adv_stop();
    }

    advertising = false;
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
uint32_t bluetooth_advertise_data()
{
    uint32_t err_code = NRF_SUCCESS;

    err_code |= ble_advdata_set(&advdata, &scanresp);
    NRF_LOG_INFO("ADV data status %s\r\n", (uint32_t)ERR_TO_STR(err_code));
    NRF_LOG_FLUSH();
    nrf_delay_ms(10);
    APP_ERROR_CHECK(err_code);
    return err_code;
    
}



