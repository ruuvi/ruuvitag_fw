#include <stdbool.h>
#include "init.h"

//Nordic SDK
#include "ble_advdata.h"
#include "nordic_common.h"
#include "softdevice_handler.h"
#include "app_scheduler.h"
#include "app_timer_appsh.h"
#include "nrf_drv_clock.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"


//BSP
#include "bsp.h"
#include "boards.h"

//Drivers
#include "lis2dh12.h"
#include "bme280.h"
#include "battery.h"
#include "ble_bulk_transfer.h"
#include "bluetooth_core.h"
#include "bme280_temperature_handler.h"
#include "lis2dh12.h"
#include "lis2dh12_acceleration_handler.h"
#include "nfc.h"
#include "pin_interrupt.h"
#include "pwm.h"
#include "watchdog.h"

//Libraries
#include "ruuvi_endpoints.h"
#include "chain_channels.h"


#define NRF_LOG_MODULE_NAME "INIT"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

/**
 * Initialize logging
 *
 * This function initializes logging peripherals, and must be called before using NRF_LOG().
 * Exact functionality depends on defines at sdk_config.h.
 * 
 * @return 0          Operation successful
 * @retval 1          Something went wrong
 *
 */
init_err_code_t init_log(void)
{
    init_err_code_t err_code;
    err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);
    NRF_LOG_DEBUG("Logging init\r\n");
    return (NRF_SUCCESS == err_code) ? INIT_SUCCESS : INIT_ERR_UNKNOWN;
}

/**
 * Initialize ble stack
 *
 * This function initializes BLE stack, related timers and clock sources.
 * Most of the functions of RuuviTag require BLE stack initialization for timers
 * and clocks even if the do not use BLE functionality
 */
init_err_code_t init_ble(void)
{
    init_err_code_t err_code;

    //Enable DC/DC for BLE
    NRF_POWER->DCDCEN = 1;
    NRF_LOG_DEBUG("BLE Stack init start\r\n");
    
    //Enable scheduler - required for BLE stack
    APP_SCHED_INIT(SCHED_MAX_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE);
    APP_TIMER_APPSH_INIT(RUUVITAG_APP_TIMER_PRESCALER, SCHED_QUEUE_SIZE, true);

    //Enable BLE STACK
    err_code =  bluetooth_stack_init();
    
    // Application Replies are sent by BLE GATT
    #if APP_GATT_PROFILE_ENABLED
      set_ble_gatt_handler(ble_std_transfer_asynchronous);
      set_reply_handler(ble_std_transfer_asynchronous);
    #endif
    
    NRF_LOG_DEBUG("BLE Stack init done\r\n");
    return (NRF_SUCCESS == err_code) ? INIT_SUCCESS : INIT_ERR_UNKNOWN;
}

/**
 * Initialize NFC driver
 *  
 * Puts NFC on standby, ready to transmit ID of the tag.
 * 
 * @return 0          Operation successful
 * @retval 1          Something went wrong
 *
 */
init_err_code_t init_nfc(void)
{
    init_err_code_t err_code = NRF_SUCCESS;

    //Enable NFC with empty data message
    err_code = nfc_init(NULL, 0);

    NRF_LOG_INFO("NFC init\r\n");
    return (NRF_SUCCESS == err_code) ? 0 : 1;
}

/**
 * Initialize timers
 *
 * This function initializes timers used by main application.
 * Requires low-frequency clock source initialized by BLE init
 *
 * @param main_timer_id pointer to timer data structure
 * @param mode mode of the timer, APP_TIMER_MODE_REPEATED or APP_TIMER_MODE_SINGLESHOT
 * @param main_interval Interval at which the main loop should be run in ms
 * @param timer_handler function to be called at main interval
 *
 */
init_err_code_t init_timer(app_timer_id_t main_timer_id, app_timer_mode_t mode, uint32_t main_interval, void (*timer_handler)(void *))
{
    //TODO Check lfclk config
    // Requires low-frequency clock initialized.
    // Create timer
    init_err_code_t err_code = app_timer_create(&main_timer_id,
                                mode,
                                timer_handler);
                                
    APP_ERROR_CHECK(err_code);
    
    //Start timer
    err_code = app_timer_start(main_timer_id, APP_TIMER_TICKS(main_interval, RUUVITAG_APP_TIMER_PRESCALER), NULL); // 1 event / MAIN_TIMER_INTERVAL
    APP_ERROR_CHECK(err_code);
    NRF_LOG_DEBUG("Timers init\r\n");
    return (NRF_SUCCESS == err_code) ? INIT_SUCCESS : INIT_ERR_UNKNOWN;
}

/**
 * Initialize leds
 *
 * This function initializes GPIO for leds
 * TODO: Generalise
 *
 */
init_err_code_t init_leds(void)
{
  if(LED_RED)
  {
    nrf_gpio_cfg_output	(LED_RED);
    nrf_gpio_pin_set(LED_RED);
  }
  if(LED_GREEN)
  {
    nrf_gpio_cfg_output	(LED_GREEN);
    nrf_gpio_pin_set(LED_GREEN);
  }
  NRF_LOG_DEBUG("LEDs init\r\n");
  return INIT_SUCCESS; // Cannot fail under any reasonable circumstance
}

/**
 * Initialize buttons
 *
 * This function initializes GPIO for buttons, setting pull-up and sensing hi-to-low transition.
 *
 */
init_err_code_t init_buttons(void)
{
    nrf_gpio_cfg_sense_input(BUTTON_1,
                             BUTTON_PULL,
                             NRF_GPIO_PIN_SENSE_LOW);
    return INIT_SUCCESS; // Cannot fail under any reasonable circumstance
}

init_err_code_t init_lis2dh12(void)
{
    init_err_code_t err_code = INIT_SUCCESS;
    err_code |= lis2dh12_init();

    if (INIT_SUCCESS == err_code)
    {
        NRF_LOG_DEBUG("LIS2DH12 init Done\r\n");
        set_acceleration_handler(lis2dh12_acceleration_handler);
    }
    else
    {
        NRF_LOG_ERROR("LIS2DH12 init Failed: Error Code: %d\r\n", (int32_t)err_code);
    }
  return err_code;
}

init_err_code_t init_bme280(void)
{
    // Read calibration
    init_err_code_t err_code = INIT_SUCCESS;
    err_code = bme280_init();
    if (INIT_SUCCESS != err_code)
    {
      return (BME280_RET_ERROR_SELFTEST == (BME280_Ret)err_code) ? INIT_ERR_SELFTEST : INIT_ERR_NO_RESPONSE;
    }
    //TODO: reset
    bme280_set_mode(BME280_MODE_SLEEP); //Set sleep mode to allow configuration, sensor might have old config in internal RAM
    err_code |= bme280_set_interval(BME280_STANDBY_1000_MS);
    err_code |= bme280_set_oversampling_hum(BME280_OVERSAMPLING_1);
    err_code |= bme280_set_oversampling_temp(BME280_OVERSAMPLING_1);
    err_code |= bme280_set_oversampling_press(BME280_OVERSAMPLING_1);

    if (BME280_RET_OK == (BME280_Ret)err_code)
    {
        NRF_LOG_DEBUG("BME280 init Done, setting up message handlers\r\n");
        set_temperature_handler(bme280_temperature_handler);
    }
    else
    {
        NRF_LOG_ERROR("BME280 init Failed: Error Code: %d\r\n", (uint32_t)err_code); 
    }

    return err_code;
}

/**
 * Initialize accelerometer
 *
 * This function initializes the lis2dh12 drivers.
 * It should be called even if lis2dh12 is not used, 
 * since initialization will put lis2dh12 in low-power mode
 */
init_err_code_t init_sensors(void)
{
    init_err_code_t err_code = INIT_SUCCESS;
    //Init accelerometer lis2dh12
    err_code |= init_lis2dh12();
    //init environmental sensor bme280
    err_code |= init_bme280();

    return err_code;
}

/**
 * Initialise PWM channels
 */
init_err_code_t init_pwm(void)
{
  pwm_init(200, LED_RED, LED_GREEN, 0, 0);
  return INIT_SUCCESS;
}

/**
 *  Initialize and enable watchdog. After calling this function
 *  watchdog_feed() must be called at interval defined by sdk_config.
 *  If NULL is given as a handler, default handler which prints error
 *  log is used.
 */
init_err_code_t init_watchdog(watchdog_event_handler_t handler)
{
  if(NULL == handler) { handler = watchdog_default_handler; }
  init_err_code_t err_code = INIT_SUCCESS;
  err_code |= watchdog_init(handler);
  watchdog_enable();
  return err_code;
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
init_err_code_t init_blink_status(uint8_t init_status)
{
    nrf_gpio_pin_clear(LED_RED);
    do
    {
        for(uint8_t ii = 0; ii < init_status * 2; ii++)
        { 
            nrf_gpio_pin_toggle(LED_RED);
            nrf_delay_ms(150u);//Delay prevents power saving, use with care
        }//infinite loop if there is error

        nrf_delay_ms(1000u); // Gives user time to count the blinks
    }while(init_status);
    nrf_gpio_pin_set(LED_RED);
    
    return INIT_SUCCESS;
}
