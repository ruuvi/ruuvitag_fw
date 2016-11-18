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

//STDLIB
#include <stdbool.h>
#include <stdint.h>

//Nordic SDK
#include "ble_advdata.h"
#include "nordic_common.h"
#include "softdevice_handler.h"
#include "app_timer.h"
#include "nrf_drv_clock.h"
#include "nrf_gpio.h"

//BSP
#include "bsp.h"

//Drivers
#include "LIS2DH12.h"
#include "bme280.h"

//#include "base91.h"
#include "eddystone.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#define swap_u32(num) ((num>>24)&0xff) | ((num<<8)&0xff0000) | ((num>>8)&0xff00) | ((num<<24)&0xff000000);
#define float2fix(a) ((int)((a)*256.0))   //Convert float to fix. a is a float

#define DEAD_BEEF                       0xDEADBEEF                        /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

#define APP_TIMER_PRESCALER             RUUVITAG_APP_TIMER_PRESCALER      /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_OP_QUEUE_SIZE         RUUVITAG_APP_TIMER_OP_QUEUE_SIZE  /**< Size of timer operation queues. */
APP_TIMER_DEF(main_timer_id);                                             /** Creates timer id for our program **/

/**@brief Function for doing power management.
 */
static void power_manage(void)
{
    uint32_t err_code = sd_app_evt_wait();
    APP_ERROR_CHECK(err_code);
}

// Timeout handler for the repeated timer
static void main_timer_handler(void * p_context)
{
    nrf_gpio_pin_toggle(17);
}

/**
 * @brief Function for application main entry.
 */
int main(void)
{
    uint32_t err_code;//, humidity, pressure;
    int32_t testX, testY, testZ;// temperature;

    // Initialize log
    err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    //Initialize Eddystone
    char url[] = {'r', 'u', 'u', '.', 'v', 'i'};
    eddystone_init();
    eddystone_advertise_url(url, 6);
    NRF_LOG_INFO("Eddystone Start!\r\n");

    // Initialize the application timer module.
    // Requires low-frequency clock initialized above
    APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, false);
    // Create timer
    err_code = app_timer_create(&main_timer_id,
                                APP_TIMER_MODE_REPEATED,
                                main_timer_handler);
    APP_ERROR_CHECK(err_code);
    //Start timer
    err_code = app_timer_start(main_timer_id, APP_TIMER_TICKS(500u, APP_TIMER_PRESCALER), NULL); // 1 event / 1000 ms
    APP_ERROR_CHECK(err_code);
    nrf_gpio_cfg_output	(17);
    nrf_gpio_cfg_output	(19);

    LIS2DH12_Ret Lis2dh12RetVal;
	
    NRF_LOG_INFO("LIS2DH12 init Start\r\n");
    Lis2dh12RetVal = LIS2DH12_init(LIS2DH12_POWER_LOW, LIS2DH12_SCALE2G, NULL);

    if (LIS2DH12_RET_OK == Lis2dh12RetVal)
    {
        NRF_LOG_INFO("LIS2DH12 init Done\r\n");
    }
    else
    {
        NRF_LOG_ERROR("LIS2DH12 init Failed: Error Code: %d\r\n", (int32_t)Lis2dh12RetVal);
        //TODO: Enter error handler?
    }
    /*
    NRF_LOG_INFO("BME280 init Start\r\n");
    // Read calibration
    bme280_init();
    //setup sensor readings
    bme280_set_oversampling_hum(BME280_OVERSAMPLING_1);
    bme280_set_oversampling_temp(BME280_OVERSAMPLING_1);
    bme280_set_oversampling_press(BME280_OVERSAMPLING_1);
    //Start measurement
    bme280_set_mode(BME280_MODE_NORMAL);

    NRF_LOG_INFO("BME280 init done\r\n");   */


    

    // Enter main loop.
    for (;; )
    {
         NRF_LOG_DEBUG("Loopin'\r\n");

         LIS2DH12_getALLmG(&testX, &testY, &testZ);
         NRF_LOG_INFO ("X-Axis: %d, Y-Axis: %d, Z-Axis: %d", testX, testY, testZ);
/*
         temperature = bme280_get_temperature();
         pressure = bme280_get_pressure();
         humidity = bme280_get_humidity();
         NRF_LOG_INFO ("temperature: %d, pressure: %d, humidity: %d", temperature, pressure, humidity);*/

         power_manage();
    }
}


/**
 * @}
 */
