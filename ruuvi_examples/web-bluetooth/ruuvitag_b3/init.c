#include "init.h"

//Configuration
#include "bluetooth_config.h"

//Event handlers
#include "event_handlers.h"
#include "application.h"

//BSP
#include "bsp.h"

//Drivers
#include "LIS2DH12.h"
#include "bme280.h"

//Libraries
#include "bluetooth_core.h"

//Nordic libs
#include "app_scheduler.h"
#include "app_timer_appsh.h"
//Log
#define NRF_LOG_MODULE_NAME "init"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

//stdlib
#include <string.h>

//static ble_uuid_t m_adv_uuids[] = {{BLE_UUID_DEVICE_INFORMATION_SERVICE, BLE_UUID_TYPE_BLE}}; /**< Universally unique service identifiers. */

/**
 * Initialize logging
 *
 * This function initializes logging peripherals, and must be called before using NRF_LOG().
 * Exact functionality depends on defines at sdk_config.h.
 * 
 * 
 * 
 * @return 0          Operation successful
 * @retval 1          Something went wrong
 *
 */
uint8_t init_log(void)
{
    uint32_t err_code;
    err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);
    NRF_LOG_INFO("Logging init\r\n");
    return (NRF_SUCCESS == err_code) ? 0 : 1;
}

/**
 * Initialize ble stack
 *
 * This function initializes BLE stack, related timers and clock sources.
 * Most of the functions of RuuviTag require BLE stack initialization for timers
 * and clocks even if the do not use BLE functionality
 *  
 * 
 * @return 0          Operation successful
 * @retval 1          Something went wrong
 *
 */
uint8_t init_ble(void)
{
    uint32_t err_code;

    //Enable DC/DC for BLE
    NRF_POWER->DCDCEN = 1;

    //Enable BLE STACK
    err_code =  ble_stack_init();
    NRF_LOG_INFO("BLE Stack init\r\n");
    APP_ERROR_CHECK(err_code);

    //Initialize Peer Manager, erase bonds
    peer_manager_init(true);
    NRF_LOG_INFO("Peer Manager init\r\n");

    gap_params_init();
    NRF_LOG_INFO("GAP init\r\n");

    advertising_init(NULL, 0);            //No manufacturer specific data
    NRF_LOG_INFO("Advertising init\r\n");

    services_init();
    NRF_LOG_INFO("Services init\r\n");

    init_register_events();

    // Initialize timer module for connection parameters TODO: remove / refactor
    APP_TIMER_APPSH_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, true);

    conn_params_init();
    NRF_LOG_INFO("Conn params init\r\n");
   
    return (NRF_SUCCESS == err_code) ? 0 : 1;
}

/**@brief Function for the GAP initialization.
 *
 * @details This function sets up all the necessary GAP (Generic Access Profile) parameters of the
 *          device including the device name, appearance, and the preferred connection parameters.
 */
void gap_params_init(void)
{
    uint32_t                err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;
    NRF_LOG_INFO("SET Security\r\n");
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);


    unsigned int mac =  NRF_FICR->DEVICEADDR[0];
    char display_name[DEVICE_NAME_LENGTH + 6];
    strcpy(display_name, DEVICE_NAME);
    uint8_t index = DEVICE_NAME_LENGTH;
    display_name[index++] = ' ';
    sprintf(&display_name[index], "%x", mac);
    index+=4;
    display_name[index++] = 0x00;
    NRF_LOG_INFO("SET Name %s\r\n", (uint32_t)display_name);
    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *)display_name,
                                          strlen(display_name));
    APP_ERROR_CHECK(err_code);

    /* YOUR_JOB: Use an appearance value matching the application's use case.
    err_code = sd_ble_gap_appearance_set(BLE_APPEARANCE_);
    APP_ERROR_CHECK(err_code); */

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;  //TODO: Disable timeout

    NRF_LOG_INFO("SET GAP\r\n");
    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing services that will be used by the application.
 */
extern nrf_ble_qwr_t     m_qwr;                    /**< Queued Writes structure.*/
void services_init(void)
{
    nrf_ble_qwr_init_t qwr_init;
    uint32_t           err_code;

    // Initialize Queued Write Module
    memset(&qwr_init, 0, sizeof(qwr_init));
    qwr_init.mem_buffer.len   = 0;
    qwr_init.mem_buffer.p_mem = NULL;

    err_code = nrf_ble_qwr_init(&m_qwr, &qwr_init);
    APP_ERROR_CHECK(err_code);

    err_code = bluetooth_init();
    APP_ERROR_CHECK(err_code);
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

    err_code = fds_register(fds_evt_handler);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing the Advertising functionality.
 */
/*void advertising_init(void)
{
    uint32_t      err_code;
    ble_advdata_t advdata;

    // Build advertising data struct to pass into @ref ble_advertising_init.
    memset(&advdata, 0, sizeof(advdata));

    advdata.name_type               = BLE_ADVDATA_FULL_NAME;
    advdata.include_appearance      = true;
    advdata.flags                   = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    advdata.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
    advdata.uuids_complete.p_uuids  = m_adv_uuids;

    ble_adv_modes_config_t options = {0};
    options.ble_adv_fast_enabled  = true;
    options.ble_adv_fast_interval = APP_ADV_INTERVAL;
    options.ble_adv_fast_timeout  = APP_ADV_TIMEOUT_IN_SECONDS;

    err_code = ble_advertising_init(&advdata, NULL, &options, on_adv_evt, NULL);
    APP_ERROR_CHECK(err_code);
}*/

/**@brief Function for initializing the Connection Parameters module.
 */
void conn_params_init(void)
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

    NRF_LOG_INFO("SET Conn params\r\n");
    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}


/**
 * Initialize timers
 *
 * This function initializes timers used by main application.
 * Requires low-frequency clock source initialized by BLE init
 *
 * @param main_timer_id pointer to timer data structure
 * @param main_interval Interval at which the main loop should be run in ms
 * @param timer_handler function to be called at main interval
 * 
 * @return 0          Operation successful
 * @retval 1          Something went wrong
 *
 */
uint8_t init_timer(app_timer_id_t main_timer_id, uint32_t main_interval, void (*timer_handler)(void *))
{
    // Requires low-frequency clock initialized
    APP_SCHED_INIT(SCHED_MAX_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE);
    APP_TIMER_APPSH_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, true);
    // Create timer
    uint32_t err_code = app_timer_create(&main_timer_id,
                                APP_TIMER_MODE_REPEATED,
                                timer_handler);
    APP_ERROR_CHECK(err_code);
    //Start timer
    err_code = app_timer_start(main_timer_id, APP_TIMER_TICKS(main_interval, APP_TIMER_PRESCALER), NULL); // 1 event / 5000 ms
    APP_ERROR_CHECK(err_code);
    NRF_LOG_INFO("Timers init\r\n");
    return (NRF_SUCCESS == err_code) ? 0 : 1;
}

/**
 * Initialize leds
 *
 * This function initializes GPIO for leds
 * 
 * @return 0          Operation successful
 * @retval 1          Something went wrong
 *
 */
uint8_t init_leds(void)
{
    nrf_gpio_cfg_output	(LED_RED);
    nrf_gpio_pin_set(LED_RED);
    nrf_gpio_cfg_output	(LED_GREEN);
    nrf_gpio_pin_set(LED_GREEN);
    NRF_LOG_INFO("LEDs init\r\n");
    return 0; // Cannot fail under any reasonable circumstance
}

/**
 * Initialize buttons
 *
 * This function initializes GPIO for buttons
 * TODO: event / interrupt driven button reading 
 *
 * @return 0          Operation successful
 * @retval 1          Something went wrong
 *
 */
uint8_t init_buttons(void)
{
    nrf_gpio_cfg_sense_input(BUTTON_1,
                             BUTTON_PULL,
                             NRF_GPIO_PIN_SENSE_LOW);
    NRF_LOG_INFO("Button init\r\n");
    return 0; // Cannot fail under any reasonable circumstance
}

/**
 * Initialize sensors
 *
 * This function initializes the sensor drivers.
 * It should be called even if sensors are not used, 
 * since initialization will put sensors in low-power mode
 *
 * @return 0          Operation successful
 * @retval 1          Something went wrong
 *
 */
uint8_t init_sensors(void)
{
    //Init accelerometer lis2dh12
    uint8_t retval = 0;
    LIS2DH12_Ret Lis2dh12RetVal;
    Lis2dh12RetVal = LIS2DH12_init(LIS2DH12_POWER_DOWN, LIS2DH12_SCALE16G, accelerometer_callback);

    if (LIS2DH12_RET_OK == Lis2dh12RetVal)
    {
        NRF_LOG_INFO("LIS2DH12 init Done\r\n");
    }
    else
    {
        retval = 1;
        NRF_LOG_ERROR("LIS2DH12 init Failed: Error Code: %d\r\n", (int32_t)Lis2dh12RetVal);
    }

    // Read calibration
    BME280_Ret BME280RetVal;
    BME280RetVal = bme280_init();
    BME280RetVal = bme280_set_mode(BME280_MODE_NORMAL);

    if (BME280_RET_OK == BME280RetVal)
    {
        NRF_LOG_INFO("BME280 init Done\r\n");
    }
    else
    {
        retval = 1;
        NRF_LOG_ERROR("BME280 init Failed: Error Code: %d\r\n", (int32_t)BME280RetVal); 
    }

    return retval;
}

/**
 * Register event handlers
 *
 * This function registers all event handlers used in application
 *
 */
void init_register_events(void)
{
  uint32_t err_code;
  
  // Register with the SoftDevice handler module for BLE events.
  err_code = softdevice_ble_evt_handler_set(ble_evt_dispatch);
  APP_ERROR_CHECK(err_code);

  // Register with the SoftDevice handler module for system events.
  err_code = softdevice_sys_evt_handler_set(sys_evt_dispatch);
  APP_ERROR_CHECK(err_code);

  NRF_LOG_INFO("Events registered\r\n");
}

/**
 * Display init status
 *
 * This function checks the init status from previous
 * init operations, and blinks the led in infinite loop
 * if there was error.
 * 
 * @param init_status number of errors occured in init, 0 on successful init.
 */
void init_blink_status(uint8_t init_status)
{
    nrf_gpio_pin_clear(LED_RED);
    do
    {
        for(uint8_t ii = 0; ii < init_status * 2; ii++)
        { 
            nrf_gpio_pin_toggle(LED_RED);
            nrf_delay_ms(250u);//Delay prevents power saving, use with care
        }

        nrf_delay_ms(3000u);   // Gives user time to count the blinks
    }while(init_status);       //infinite loop if there is error
    nrf_gpio_pin_set(LED_RED); //TODO: Use BSP functions
}

