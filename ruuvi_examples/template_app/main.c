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
#include <math.h>

//Nordic SDK
#include "ble_advdata.h"
#include "nordic_common.h"
#include "softdevice_handler.h"
#include "app_scheduler.h"
#include "app_timer_appsh.h"
#include "nrf_drv_clock.h"
#include "nrf_gpio.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_delay.h"

//BSP
#include "bsp.h"

//Drivers
#include "LIS2DH12.h"
#include "bme280.h"

//Libraries
#include "bluetooth_core.h"

//SDK configuration
//#define NRF_LOG_ENABLED 1 //Uncomment to enable logging. This causes increased current consumption, even if logging is not used.

//Constants
#define DEAD_BEEF                       0xDEADBEEF                        /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

//Timers
#define APP_TIMER_PRESCALER             RUUVITAG_APP_TIMER_PRESCALER      /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_OP_QUEUE_SIZE         RUUVITAG_APP_TIMER_OP_QUEUE_SIZE  /**< Size of timer operation queues. */
// Scheduler settings
APP_TIMER_DEF(main_timer_id);                                             /** Creates timer id for our program **/
#define SCHED_MAX_EVENT_DATA_SIZE       MAX(APP_TIMER_SCHED_EVT_SIZE, sizeof(nrf_drv_gpiote_pin_t))
#define SCHED_QUEUE_SIZE                10

//milliseconds until main loop timer function is called. Other timers can bring
//application out of sleep at higher (or lower) interval
#define MAIN_LOOP_INTERVAL 5000u 

#define ERROR_BLINK_INTERVAL 250u   //toggle interval of error led
#define INIT_OK_DELAY        3000u  //delay after successful init

/**
 * @brief Function for doing power management.
 * Uncomment the FPU register clearing if FPU
 * is in use.
 * 
 * Turns off red "activity" led when entering sleep
 * and turns led on when sleep stops to give
 * indication of CPU activity.
 */

static void power_manage(void)
{
    nrf_gpio_pin_set(LED_GREEN);
      /* Clear exceptions and PendingIRQ from the FPU unit */
      //__set_FPSCR(__get_FPSCR()  & ~(FPU_EXCEPTION_MASK));      
      //(void) __get_FPSCR();
      //NVIC_ClearPendingIRQ(FPU_IRQn);
    uint32_t err_code = sd_app_evt_wait();
    APP_ERROR_CHECK(err_code);
    nrf_gpio_pin_clear(LED_GREEN); 
}

// Timeout handler for the repeated timer
static void main_timer_handler(void * p_context)
{
    //nrf_gpio_pin_toggle(LED_RED); Uncomment to verify timer interval
}

// Timeout handler for the repeated timer
static void init_error_blink(uint8_t error)
{
    for(uint8_t ii = 0; ii < error * 2; ii++){ 
        nrf_gpio_pin_toggle(LED_RED);
        nrf_delay_ms(250u);//Delay prevents power saving, use with care
    }
}

/**
 * @brief Function for application main entry.
 */
int main(void)
{
    uint32_t err_code;
    uint8_t init_status = 6; // counter, gets decremented by each successful init. Is 0 in the end if init was ok.

    // Initialize log
    err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);
    init_status--; //5
    NRF_LOG_INFO("Logging init\r\n");

    //Initialize BLE Stack. Required in all applications for timer operation
    ble_stack_init();
    NRF_LOG_INFO("BLE Stack init\r\n");
    init_status--; //4

    // Initialize the application timer module.
    // Requires low-frequency clock initialized above
    APP_SCHED_INIT(SCHED_MAX_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE);
    APP_TIMER_APPSH_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, true);
    // Create timer
    err_code = app_timer_create(&main_timer_id,
                                APP_TIMER_MODE_REPEATED,
                                main_timer_handler);
    APP_ERROR_CHECK(err_code);
    //Start timer
    err_code = app_timer_start(main_timer_id, APP_TIMER_TICKS(MAIN_LOOP_INTERVAL, APP_TIMER_PRESCALER), NULL); // 1 event / 5000 ms
    APP_ERROR_CHECK(err_code);
    NRF_LOG_INFO("Timers init\r\n");
    init_status--; //3

    //setup leds. LEDs are active low, so setting them turns leds off.
    nrf_gpio_cfg_output	(LED_RED);
    nrf_gpio_pin_set(LED_RED);
    nrf_gpio_cfg_output	(LED_GREEN);
    nrf_gpio_pin_set(LED_GREEN);
    NRF_LOG_INFO("LEDs init\r\n");
    init_status--; //2

    LIS2DH12_Ret Lis2dh12RetVal;
    Lis2dh12RetVal = LIS2DH12_init(LIS2DH12_POWER_LOW, LIS2DH12_SCALE2G, NULL);

    if (LIS2DH12_RET_OK == Lis2dh12RetVal)
    {
        NRF_LOG_INFO("LIS2DH12 init Done\r\n");
        init_status--; //1
    }
    else
    {
        NRF_LOG_ERROR("LIS2DH12 init Failed: Error Code: %d\r\n", (int32_t)Lis2dh12RetVal);
    }

    // Read calibration
    BME280_Ret BME280RetVal;
    BME280RetVal = bme280_init();

    //setup sensor readings
    bme280_set_oversampling_hum(BME280_OVERSAMPLING_1);
    bme280_set_oversampling_temp(BME280_OVERSAMPLING_1);
    bme280_set_oversampling_press(BME280_OVERSAMPLING_1);
    //Start single measurement.
    //Read values later, and start a new measurement after read
    //to keep updating sensor data.
    bme280_set_mode(BME280_MODE_FORCED);

    if (BME280_RET_OK == BME280RetVal)
    {
        NRF_LOG_INFO("BME280 init Done\r\n");
        init_status--; //0
    }
    else
    {
        NRF_LOG_ERROR("BME280 init Failed: Error Code: %d\r\n", (int32_t)BME280RetVal);
        
    }

    //Visually display init status
    nrf_gpio_pin_clear(LED_RED);
    do
    {
        init_error_blink(init_status); //infinite loop if there is error
        nrf_delay_ms(3000u); // Gives user time to cout the blinks
    }while(init_status);
    nrf_gpio_pin_set(LED_RED);

    // Enter main loop.
    for (;; )
    {
         if(NRF_LOG_PROCESS() == false){
           app_sched_execute();
           power_manage();
         }
    }
}
