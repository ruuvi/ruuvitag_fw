#include <stdbool.h>
#include "init.h"

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
    uint32_t err_code;
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
    uint32_t err_code;

    //Enable DC/DC for BLE
    NRF_POWER->DCDCEN = 1;
    NRF_LOG_DEBUG("BLE Stack init start\r\n");
    
    //Enable scheduler - required for BLE stack
    APP_SCHED_INIT(SCHED_MAX_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE);
    APP_TIMER_APPSH_INIT(RUUVITAG_APP_TIMER_PRESCALER, RUUVITAG_APP_TIMER_OP_QUEUE_SIZE, true);

    //Enable BLE STACK
    err_code =  ble_stack_init();
    
    NRF_LOG_DEBUG("BLE Stack init done\r\n");
    return (NRF_SUCCESS == err_code) ? INIT_SUCCESS : INIT_ERR_UNKNOWN;
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
 */
init_err_code_t init_timer(app_timer_id_t main_timer_id, uint32_t main_interval, void (*timer_handler)(void *))
{
    //TODO Check lfclk config
    // Requires low-frequency clock initialized.
    // Create timer
    uint32_t err_code = app_timer_create(&main_timer_id,
                                APP_TIMER_MODE_REPEATED,
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
    nrf_gpio_cfg_output	(LED_RED);
    nrf_gpio_pin_set(LED_RED);
    nrf_gpio_cfg_output	(LED_GREEN);
    nrf_gpio_pin_set(LED_GREEN);
    NRF_LOG_DEBUG("LEDs init\r\n");
    return INIT_SUCCESS; // Cannot fail under any reasonable circumstance
}

/**
 * Initialize buttons
 *
 * This function initializes GPIO for buttons
 * TODO: event / interrupt driven button reading 
 * TODO: Generalise
 *
 */
init_err_code_t init_buttons(void)
{
    nrf_gpio_cfg_sense_input(BUTTON_1,
                             BUTTON_PULL,
                             NRF_GPIO_PIN_SENSE_LOW);
    return INIT_SUCCESS; // Cannot fail under any reasonable circumstance
}

/**
 * Initialize sensors
 *
 * This function initializes the sensor drivers.
 * It should be called even if sensors are not used, 
 * since initialization will put sensors in low-power mode
 *
 *
 */
init_err_code_t init_sensors(void)
{
    //Init accelerometer lis2dh12
    uint8_t retval = 0;
    LIS2DH12_Ret Lis2dh12RetVal;
    Lis2dh12RetVal = LIS2DH12_init(LIS2DH12_POWER_DOWN, LIS2DH12_SCALE2G, NULL);

    if (LIS2DH12_RET_OK == Lis2dh12RetVal)
    {
        NRF_LOG_DEBUG("LIS2DH12 init Done\r\n");
    }
    else
    {
        retval = 1;
        NRF_LOG_ERROR("LIS2DH12 init Failed: Error Code: %d\r\n", (int32_t)Lis2dh12RetVal);
        return retval;
    }

    // Read calibration
    BME280_Ret BME280RetVal;
    BME280RetVal = bme280_init();
    bme280_set_mode(BME280_MODE_SLEEP); //Set sleep mode to allow configuration, sensor might have old config in internal RAM
    BME280RetVal |= bme280_set_interval(BME280_STANDBY_1000_MS);

    if (BME280_RET_OK == BME280RetVal)
    {
        NRF_LOG_DEBUG("BME280 init Done\r\n");
    }
    else
    {
        retval = 1;
        NRF_LOG_ERROR("BME280 init Failed: Error Code: %d\r\n", (int32_t)BME280RetVal); 
    }

    return INIT_SUCCESS;
}

init_err_code_t init_lis2dh12(void)
{
    LIS2DH12_Ret Lis2dh12RetVal;
    Lis2dh12RetVal = LIS2DH12_init(LIS2DH12_POWER_DOWN, LIS2DH12_SCALE2G, NULL);

    if (LIS2DH12_RET_OK == Lis2dh12RetVal)
    {
        NRF_LOG_DEBUG("LIS2DH12 init Done\r\n");
    }
    else
    {
        NRF_LOG_ERROR("LIS2DH12 init Failed: Error Code: %d\r\n", (int32_t)Lis2dh12RetVal);
        return INIT_ERR_UNKNOWN;
    }
  return INIT_SUCCESS;
}

init_err_code_t init_bme280(void)
{
    // Read calibration
    BME280_Ret BME280RetVal;
    BME280RetVal = bme280_init();
    if (BME280_RET_OK != BME280RetVal)
    {
      return (BME280_RET_ERROR_SELFTEST == BME280RetVal) ? INIT_ERR_SELFTEST : INIT_ERR_NO_RESPONSE;
    }
    //TODO: reset
    bme280_set_mode(BME280_MODE_SLEEP); //Set sleep mode to allow configuration, sensor might have old config in internal RAM
    BME280RetVal |= bme280_set_interval(BME280_STANDBY_1000_MS);
    BME280RetVal |= bme280_set_oversampling_hum(BME280_OVERSAMPLING_1);
    BME280RetVal |= bme280_set_oversampling_temp(BME280_OVERSAMPLING_1);
    BME280RetVal |= bme280_set_oversampling_press(BME280_OVERSAMPLING_1);

    if (BME280_RET_OK == BME280RetVal)
    {
        NRF_LOG_DEBUG("BME280 init Done\r\n");
    }
    else
    {
        NRF_LOG_ERROR("BME280 init Failed: Error Code: %d\r\n", (uint32_t)BME280RetVal); 
        return INIT_ERR_UNKNOWN;
    }

    return INIT_SUCCESS;
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

