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
#include "ble_conn_params.h"
#include "ble_nus.h"
#include "peer_manager.h"
#include "sdk_errors.h"
#include "nrf_delay.h"

#include "bluetooth_config.h"
#include "bluetooth_application_config.h"
#include "ble_bulk_transfer.h"
#include "eddystone.h"
#include "ruuvi_endpoints.h"
#include "ble_event_handlers.h" 

#if APP_GATT_PROFILE_ENABLED
#include "application_service_if.h"
#endif 

#define NRF_LOG_MODULE_NAME "BLE_CORE"
#define NRF_LOG__DEFAULT_LEVEL 4
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

#define NUS_SERVICE_UUID_TYPE           BLE_UUID_TYPE_VENDOR_BEGIN                  /**< UUID type for the Nordic UART Service (vendor specific). */
//static ble_uuid_t                       m_adv_uuids[] = {{BLE_UUID_NUS_SERVICE, NUS_SERVICE_UUID_TYPE}};  /**< Universally unique service identifier. */

//TODO: Move defaults to application configuration.
static int8_t tx_power = BLE_TX_POWER;
//https://infocenter.nordicsemi.com/index.jsp?topic=%2Fcom.nordic.infocenter.s132.api.v3.0.0%2Fstructble__gap__adv__params__t.html
static ble_gap_adv_params_t m_adv_params = {
   // BLE_GAP_ADV_TYPE_ADV_DIRECT_IND,  // Connectable, directed to specific device
   // BLE_GAP_ADV_TYPE_ADV_IND,         // Connecttable, scannable
   // BLE_GAP_ADV_TYPE_ADV_NONCONN_IND, // Non-connectable, non-scannable
   // BLE_GAP_ADV_TYPE_ADV_SCAN_IND,    // Non-connectable, scannable
  .type = BLE_GAP_ADV_TYPE_ADV_IND, 
  
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
 .name_type = BLE_ADVDATA_NO_NAME, //scan response has full name
 .short_name_len = 0, // in scan resp.
 .include_appearance = false, // scan response has appearance
 .flags = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE, // Low energy, discoverable
 .p_tx_power_level        = NULL,
 .uuids_more_available    = {.uuid_cnt = 0, .p_uuids = NULL},
 .uuids_complete          = {.uuid_cnt = 0, .p_uuids = NULL},
 .uuids_solicited         = {.uuid_cnt = 0, .p_uuids = NULL},
 .p_slave_conn_int        = NULL,
 .p_manuf_specific_data   = NULL,
 .p_service_data_array    = NULL,
 .service_data_count      = 0,
 .include_ble_device_addr = false, // Is included in protocol anyway. iOS users might benefit?
 .le_role                 = BLE_ADVDATA_ROLE_NOT_PRESENT, //always when on BLE
 .p_tk_value              = NULL, //always when on BLE
 .p_sec_mgr_oob_flags     = NULL, //always when on BLE
 .p_lesc_data             = NULL  //always when on BLE
};

ble_advdata_t scanresp =
{
 .name_type = BLE_ADVDATA_FULL_NAME, //scan response
 .short_name_len = 5,              //Name gets truncated to "Ruuvi" if full name does not fit
 .include_appearance = true,       // scan response has appearance
 .flags = 0,                       // Flags shall not be included in the scan response data.
 .p_tx_power_level        = &tx_power,
 .uuids_more_available    = {.uuid_cnt = 0, .p_uuids = NULL},    // Add some services?
 .uuids_complete          = {.uuid_cnt = 0, .p_uuids = NULL},
 .uuids_solicited         = {.uuid_cnt = 0, .p_uuids = NULL},
 .p_slave_conn_int        = NULL,
 .p_manuf_specific_data   = NULL,
 .p_service_data_array    = NULL,
 .service_data_count      = 0,
 .include_ble_device_addr = false, // add for IOS users? 
 .le_role                 = BLE_ADVDATA_ROLE_NOT_PRESENT, //always when on BLE
 .p_tk_value              = NULL, //always when on BLE
 .p_sec_mgr_oob_flags     = NULL, //always when on BLE
 .p_lesc_data             = NULL //always when on BLE
};

static bool advertising = false;
static ble_gap_conn_params_t   gap_conn_params;
static ble_gap_conn_sec_mode_t sec_mode;
static ble_advdata_manuf_data_t m_manufacturer_data;

/**
 * Generate name "BASEXXXX", where Base is human-readable (i.e. Ruuvi) and XXXX is  last 4 chars of mac address
 *
 * @param name_base character array with the base name and 5 extra chars of space (including trailing null)
 * @base_length length of name base, 5 for "RuuviXXXX"
 */
void bluetooth_name_postfix_add(char* name_base, size_t base_length)
{
    uint32_t mac0 =  NRF_FICR->DEVICEADDR[0]&0xFFFF;
    char postfix[5] = { 0 };
    snprintf(postfix, 5, "%04x", (unsigned int)(mac0));
    // ok to write trailing null, altough unnecessary if the base pointer includes it already
    memcpy(name_base + base_length, postfix, sizeof(postfix));
}
 
 /**
  *  Set name to be advertised
  */
ret_code_t bluetooth_set_name(const char* name_base, size_t name_length)
{
  uint32_t err_code = NRF_SUCCESS;
  if(name_length > 15) { return NRF_ERROR_INVALID_PARAM; }
  bool was_advertising = advertising;
  if(advertising) { bluetooth_advertising_stop(); }
  // base + 4 hex chars, leave space for trailing null
  char name[20] = { 0 };
  memcpy(name, name_base, name_length);
  bluetooth_name_postfix_add(name, name_length);
  NRF_LOG_DEBUG("%s\r\n", (uint32_t)name);
  NRF_LOG_HEXDUMP_DEBUG((uint8_t*)name, name_length + 4);
  err_code |= sd_ble_gap_device_name_set(&sec_mode,
                                        (const uint8_t *) name,
                                        name_length + 4);
  if(was_advertising) { bluetooth_advertising_start(); }
  return err_code;
}

 /**
 * @brief Function adjusting advertising interval. 
 * @details Sets the advertising interval in program.    
 * @param interval advertisement interval in milliseconds, 100 - 10 000 
 */
ret_code_t bluetooth_configure_advertising_interval(uint16_t interval)
 {
   if(interval > 10000) return NRF_ERROR_INVALID_PARAM;
   if(interval < 100) return NRF_ERROR_INVALID_PARAM;
   m_adv_params.interval = MSEC_TO_UNITS(interval, UNIT_0_625_MS);
   return NRF_SUCCESS;
 }
 
 /**
 * @brief Function for configuring advertisement type
 * @details https://infocenter.nordicsemi.com/index.jsp?topic=%2Fcom.nordic.infocenter.s132.api.v3.0.0%2Fgroup___b_l_e___g_a_p___a_d_v___t_y_p_e_s.html
 * @param type Advertisement type, 0 ... 3
 */
ret_code_t bluetooth_configure_advertisement_type(uint8_t type)
 {
  
  if(type > 3){ return NRF_ERROR_INVALID_PARAM; }
  m_adv_params.type = type;
  return NRF_SUCCESS;
 }
 
/**
 * @brief Function for configuring advertisement filter policy
 * @details  https://infocenter.nordicsemi.com/index.jsp?topic=%2Fcom.nordic.infocenter.s132.api.v3.0.0%2Fstructble__gap__adv__params__t.html
 * @param fp Advertisement filter policy, 0 ... 3
 */
 void bluetooth_configure_advertisement_filter(uint8_t fp)
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
 void bluetooth_configure_advertisement_timeout(uint16_t timeout)
 {
  //TODO: Handle invalid parameter
  if(timeout > 0x3FFF){ return; }
  m_adv_params.timeout = timeout;
 }

ret_code_t bluetooth_apply_configuration()
{
  ret_code_t err_code = NRF_SUCCESS;
  err_code |= bluetooth_advertising_start();
  if(err_code != NRF_SUCCESS) { NRF_LOG_ERROR("Failed to apply configuration: %d\r\n", err_code); }
  return err_code;
}

/**@brief Function for the GAP initialization.
 *
 * @details This function will set up all the necessary GAP (Generic Access Profile) parameters of
 *          the device. It also sets the permissions and appearance.
 */
static void gap_params_init(void)
{
    uint32_t                err_code = NRF_SUCCESS;
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));
    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

    err_code |= bluetooth_set_name(APP_DEVICE_NAME, APP_DEVICE_NAME_LENGTH);

    err_code |= sd_ble_gap_ppcp_set(&gap_conn_params);
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
    NRF_LOG_DEBUG("Conn params init status %s\r\n", (uint32_t)ERR_TO_STR(err_code));
    NRF_LOG_FLUSH();
    //nrf_delay_ms(10);
    APP_ERROR_CHECK(err_code);
}
//TODO: Enable & differentiate slow / fast advertising
static void advertising_init(void)
{
    uint32_t err_code = NRF_SUCCESS;
    ble_adv_modes_config_t options = {0};
    options.ble_adv_fast_enabled  = true;
    options.ble_adv_fast_interval = APP_ADV_INTERVAL;
    options.ble_adv_fast_timeout  = APP_ADV_TIMEOUT_IN_SECONDS;
    err_code = ble_advertising_init(&advdata, &scanresp, &options, on_adv_evt, NULL);
    APP_ERROR_CHECK(err_code);
}

ret_code_t bluetooth_stack_init(void)
{
    uint32_t err_code = NRF_SUCCESS;

    nrf_clock_lf_cfg_t clock_lf_cfg = NRF_CLOCK_LFCLKSRC;
    NRF_LOG_DEBUG("Softdevice handler init start\r\n");
    NRF_LOG_FLUSH();

    // Initialize the SoftDevice handler module.
    SOFTDEVICE_HANDLER_INIT(&clock_lf_cfg, NULL);

    NRF_LOG_DEBUG("Softdevice handler init done\r\n");      
    NRF_LOG_FLUSH();

    ble_enable_params_t ble_enable_params;
    err_code |= softdevice_enable_get_default_config(CENTRAL_LINK_COUNT,
                                                    PERIPHERAL_LINK_COUNT,
                                                    &ble_enable_params);
    
    //TODO: refactor into application                                                
    #define BLE_ATTRIBUTE_TABLE_SIZE 0x1000
    #ifdef BLE_ATTRIBUTE_TABLE_SIZE
    //Adjust attribute table size, linkerscript has to be adjusted if this value is changed
    NRF_LOG_DEBUG("Attribute table size: %d\r\n", ble_enable_params.gatts_enable_params.attr_tab_size);
    ble_enable_params.gatts_enable_params.attr_tab_size   = BLE_ATTRIBUTE_TABLE_SIZE;
    NRF_LOG_DEBUG("Attribute table size: %d\r\n", ble_enable_params.gatts_enable_params.attr_tab_size);
    #endif
    
    
    #ifdef BLE_UUID_COUNT
      ble_enable_params.common_enable_params.vs_uuid_count = BLE_UUID_COUNT;
    #endif
    NRF_LOG_INFO("Softdevice configuration ready, status: %s\r\n", (uint32_t)ERR_TO_STR(err_code));       
    nrf_delay_ms(10);
    //APP_ERROR_CHECK(err_code);

    //Check the ram settings against the used number of links
    CHECK_RAM_START_ADDR(CENTRAL_LINK_COUNT,PERIPHERAL_LINK_COUNT);
    NRF_LOG_DEBUG("RAM checked\r\n");
    
    #if (NRF_SD_BLE_API_VERSION == 3)
      ble_enable_params.gatt_enable_params.att_mtu = NRF_BLE_MAX_MTU_SIZE;
    #endif

    // Subscribe for BLE events.
    err_code |= softdevice_ble_evt_handler_set(ble_evt_dispatch);
    NRF_LOG_INFO("BLE event handler set, status %d\r\n", err_code);
    //nrf_delay_ms(10);

    // Register with the SoftDevice handler module for BLE events.
    err_code |= softdevice_sys_evt_handler_set(sys_evt_dispatch);
    NRF_LOG_INFO("System event handler set, status %d\r\n", err_code);
    nrf_delay_ms(10);

    // Enable BLE stack.
    err_code = softdevice_enable(&ble_enable_params);
    NRF_LOG_INFO("Softdevice enabled, status: %s\r\n", (uint32_t)ERR_TO_STR(err_code));
    nrf_delay_ms(10);

    #if APP_GATT_PROFILE_ENABLED
      //Enable peer manager, erase bonds
      //Init filesystem
      err_code |= fs_init();
      peer_manager_init(true);
      NRF_LOG_INFO("Peer manager init \r\n");
      nrf_delay_ms(10);
    
    
      err_code |= application_services_init();
      NRF_LOG_INFO("Services init status %d\r\n", err_code);
      nrf_delay_ms(10);
    #endif 
    
    gap_params_init();
    NRF_LOG_INFO("GAP params init\r\n");
    //nrf_delay_ms(10);

    conn_params_init();
    NRF_LOG_INFO("Conn params init, status\r\n");
    //nrf_delay_ms(10);

    advertising_init();    
    NRF_LOG_INFO("Advertising init, status\r\n");
    nrf_delay_ms(10);

    return err_code;
}

/**@brief Function for the Peer Manager initialization.
 *
 * @param[in] erase_bonds  Indicates whether bonding information should be cleared from
 *                         persistent storage during initialization of the Peer Manager.
 */
void peer_manager_init(bool erase_bonds)
{
    ble_gap_sec_params_t sec_param;
    ret_code_t           err_code;

    err_code = pm_init();
    APP_ERROR_CHECK(err_code);

    if (erase_bonds)
    {
        err_code = pm_peers_delete();
        APP_ERROR_CHECK(err_code);
    }

    memset(&sec_param, 0, sizeof(ble_gap_sec_params_t));

    // Security parameters to be used for all security procedures.
    sec_param.bond           = SEC_PARAM_BOND;
    sec_param.mitm           = SEC_PARAM_MITM;
    sec_param.lesc           = SEC_PARAM_LESC;
    sec_param.keypress       = SEC_PARAM_KEYPRESS;
    sec_param.io_caps        = SEC_PARAM_IO_CAPABILITIES;
    sec_param.oob            = SEC_PARAM_OOB;
    sec_param.min_key_size   = SEC_PARAM_MIN_KEY_SIZE;
    sec_param.max_key_size   = SEC_PARAM_MAX_KEY_SIZE;
    sec_param.kdist_own.enc  = 1;
    sec_param.kdist_own.id   = 1;
    sec_param.kdist_peer.enc = 1;
    sec_param.kdist_peer.id  = 1;

    err_code = pm_sec_params_set(&sec_param);
    APP_ERROR_CHECK(err_code);

    err_code = pm_register(pm_evt_handler);
    APP_ERROR_CHECK(err_code);
}

/**
 * @brief Function to set BLE transmission power
 *  
 * @details set the BLE transmission power in dBm
 * @param int8_t power power in dBm, must be one of -40, -30, -20, -16, -12, -8, -4, 0, 4
 * @return error code, 0 if operation was success.
 */
ret_code_t bluetooth_tx_power_set(int8_t power)
{
    uint32_t err_code = sd_ble_gap_tx_power_set(power);
    //APP_ERROR_CHECK(err_code);
    tx_power = power;
    return err_code;
}

/**
 * @brief Start advertising. 
 * BLE stack and advertising must be init. Also advertisement data should be set.
 *
 * @return error code from BLE stack, NRF_SUCCESS if ok.
 */
ret_code_t bluetooth_advertising_start(void)
{

    ret_code_t err_code = NRF_SUCCESS;
    //Stop advertising before making adjustments
    if(advertising)
    {
      err_code |= sd_ble_gap_adv_stop();
    }

    err_code |= sd_ble_gap_adv_start(&m_adv_params);
    if(NRF_SUCCESS != err_code)
    {
        NRF_LOG_INFO("Advertisement fail: %d \r\n", err_code);
    }
    else { advertising = true; }
    return err_code;
}

/**
 * @brief Stop advertising. 
 *
 * @return error code from BLE stack, NRF_SUCCESS if ok.
 */
ret_code_t bluetooth_advertising_stop(void)
{
  ret_code_t err_code = NRF_SUCCESS;
  if(advertising)
  {
    err_code = sd_ble_gap_adv_stop();
  }
  if(NRF_SUCCESS != err_code) { NRF_LOG_INFO("Advertisement fail: %d \r\n",err_code); }
  else { advertising = false; }
  return err_code;
}

/**@brief Function for advertising data. 
 *
 * @details Initializes the BLE advertisement with given data as manufacturer specific data.
 * Company ID is included by default and doesn't need to be included in parameter data.  
 * Call bluetooth advertising start to apply new data
 *
 * @return error code from BLE stack initialization, NRF_SUCCESS if init was ok
 */
ret_code_t bluetooth_set_manufacturer_data(uint8_t* data, size_t length)
{
  ret_code_t err_code = NRF_SUCCESS;

  //31 bytes - overhead - 2 bytes for manufacturer ID
  if(24 < length)  { return NRF_ERROR_INVALID_PARAM; }
  if(0 == length ) { advdata.p_manuf_specific_data = NULL; }
  else 
  {
    // Configuration of manufacturer specific data
    static uint8_t data_array[24];
    memset(data_array, 0, sizeof(data_array));
    memcpy(data_array, data, length);
    m_manufacturer_data.company_identifier = BLE_COMPANY_IDENTIFIER;
    m_manufacturer_data.data.size = length;
    m_manufacturer_data.data.p_data = data_array;
    
    memset(&advdata, 0, sizeof(advdata));
    advdata.p_manuf_specific_data = &m_manufacturer_data;
    advdata.flags = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    err_code |= ble_advdata_set(&advdata, &scanresp);
  }
  NRF_LOG_DEBUG("ADV data status %s\r\n", (uint32_t)ERR_TO_STR(err_code));

  return err_code;
}

/**
 * Set Eddystone URL advertisement package in advdata.
 * 
 * @param url_buffer character array containing new URL. May contain eddystone
 *        shortcuts, such as 0x03: "https://"
 * @param length length of URL to transmit. must be <18. Shortcut bytes are counted as one, i.e. https://ruu.vi is 7 bytes long
 *        as long as https:// is written as 0x03
 */
ret_code_t bluetooth_set_eddystone_url(char* url_buffer, size_t length)
{
  ret_code_t err_code = eddystone_prepare_url_advertisement(&advdata, url_buffer, length);
  err_code |= ble_advdata_set(&advdata, &scanresp);
  return err_code;
}
