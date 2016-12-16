/* Copyright (c) 2014 Nordic Semiconductor. All Rights Reserved.
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
 * @defgroup test_drivers
 * @{
 * @ingroup test_drivers
 * @brief Test Cases for Ruuvitag drivers.
 *
 * This file contains some tests to ensure the Ruuvitag drivers are working correct. The output is transmittet via
 * the J-Link RTT.
 */

#include <stdbool.h>
#include <stdint.h>
#include "nordic_common.h"
#include "softdevice_handler.h"
#include "bsp.h"
#include "app_timer.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "LIS2DH12.h"
#include "bme280.h"
#include "nrf_delay.h"
//#include "boards.h"
#include "acceleration.h"

#include "nrf_drv_gpiote.h"

#define DEAD_BEEF                       0xDEADBEEF                        /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

#define APP_TIMER_PRESCALER             0                                 /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_OP_QUEUE_SIZE         4                                 /**< Size of timer operation queues. */

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

/**@brief Function for doing power management.
 */
static void power_manage(void)
{
    NRF_LOG_DEBUG("Managing power\r\n");
    __WFI();
    //uint32_t err_code = sd_app_evt_wait();
   // APP_ERROR_CHECK(err_code);
    
}

/**
 * Initialize leds
 *
 * This function initializes GPIO for leds
 *
 */
static void init_leds(void)
{
    nrf_gpio_cfg_output (LED_RED);
    nrf_gpio_pin_set(LED_RED);
    nrf_gpio_cfg_output (LED_GREEN);
    nrf_gpio_pin_set(LED_GREEN);
    NRF_LOG_DEBUG("LEDs init\r\n");
}

static void movementDetected(void)
{
    nrf_gpio_pin_toggle(LED_RED);
    NRF_LOG_INFO ("### Movement detected ###\n");
}

/**
 * @brief Function for application main entry.
 */
int main(void)
{
    uint32_t err_code, humidity, pressure;
    int32_t testX, testY, testZ, temperature;
    //LIS2DH12_Ret Lis2dh12RetVal;
    nrf_clock_lf_cfg_t clock_lf_cfg = NRF_CLOCK_LFCLKSRC;

    // Initialize.
    err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);
    init_leds();
    nrf_gpio_pin_clear(LED_RED); // Red LED will turn off after initialization was successful

    // Initialize the SoftDevice handler module, necessary for power saving modes to work
    SOFTDEVICE_HANDLER_INIT(&clock_lf_cfg, NULL);

    APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, false);
    err_code = bsp_init(BSP_INIT_LED, APP_TIMER_TICKS(100, APP_TIMER_PRESCALER), NULL);

    APP_ERROR_CHECK(err_code);
	


/*    NRF_LOG_DEBUG("LIS2DH12 init Start\r\n");
    Lis2dh12RetVal = LIS2DH12_init(LIS2DH12_POWER_LOW, LIS2DH12_SCALE2G, NULL);

    if (LIS2DH12_RET_OK == Lis2dh12RetVal)
    {
        NRF_LOG_DEBUG("LIS2DH12 init Done\r\n");
    }
    else
    {
        NRF_LOG_ERROR("LIS2DH12 init Failed: Error Code: %d\r\n", (int32_t)Lis2dh12RetVal);
        //TODO: Enter error handler?
    }*/
    
    NRF_LOG_DEBUG("BME280 init Start\r\n");
    // Read calibration
    bme280_init();
    //setup sensor readings
    bme280_set_oversampling_hum(BME280_OVERSAMPLING_1);
    bme280_set_oversampling_temp(BME280_OVERSAMPLING_1);
    bme280_set_oversampling_press(BME280_OVERSAMPLING_1);
    //Start measurement
    bme280_set_mode(BME280_MODE_NORMAL);
    acceleration_init();
    acceleration_initMovementAlert(100,100,100,500,movementDetected);

    NRF_LOG_DEBUG("BME280 init done\r\n");   
    nrf_gpio_pin_set(LED_RED);
    // Enter main loop.
    for (;; )
    {
         NRF_LOG_DEBUG("Looping'\r\n");
         nrf_gpio_pin_clear(LED_GREEN);


         LIS2DH12_getALLmG(&testX, &testY, &testZ);
         NRF_LOG_INFO ("X-Axis: %d, Y-Axis: %d, Z-Axis: %d\n", testX, testY, testZ);

         temperature = bme280_get_temperature();
         pressure = bme280_get_pressure();
         humidity = bme280_get_humidity();
         NRF_LOG_DEBUG ("temperature: %d, pressure: %d, humidity: %d\n", temperature, pressure, humidity);
         nrf_gpio_pin_set(LED_GREEN);
         power_manage();
    }
}


/**
 * @}
 */
