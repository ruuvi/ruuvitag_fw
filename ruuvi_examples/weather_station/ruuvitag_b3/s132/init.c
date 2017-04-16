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
    APP_ERROR_CHECK(err_code);

    
       
    NRF_LOG_INFO("BLE Stack init\r\n");
    return (NRF_SUCCESS == err_code) ? 0 : 1;
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
      // Requires low-frequency clock initialized above
    APP_SCHED_INIT(SCHED_MAX_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE);
    APP_TIMER_APPSH_INIT(RUUVITAG_APP_TIMER_PRESCALER, RUUVITAG_APP_TIMER_OP_QUEUE_SIZE, true);
    // Create timer
    uint32_t err_code = app_timer_create(&main_timer_id,
                                APP_TIMER_MODE_REPEATED,
                                timer_handler);
    APP_ERROR_CHECK(err_code);
    //Start timer
    err_code = app_timer_start(main_timer_id, APP_TIMER_TICKS(main_interval, RUUVITAG_APP_TIMER_PRESCALER), NULL); // 1 event / 5000 ms
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
    Lis2dh12RetVal = LIS2DH12_init(LIS2DH12_POWER_DOWN, LIS2DH12_SCALE2G, NULL);

    if (LIS2DH12_RET_OK == Lis2dh12RetVal)
    {
        NRF_LOG_INFO("LIS2DH12 init Done\r\n");
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
    bme280_set_mode(BME280_MODE_SLEEP); //Set sleep mode to allow configuration, sensor might have old config
    BME280RetVal |= bme280_set_interval(BME280_STANDBY_1000_MS);

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
        }//infinite loop if there is error

        nrf_delay_ms(3000u); // Gives user time to count the blinks
    }while(init_status);
    nrf_gpio_pin_set(LED_RED);
}

