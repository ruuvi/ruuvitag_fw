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
#include "nrf_delay.h" //Use with care, consumes lot of power

//BSP
#include "bsp.h"

//Drivers
#include "LIS2DH12.h"
#include "bme280.h"

//Libraries
#include "bluetooth_core.h"

//Project
#include "init.h"

//SDK configuration
//#define NRF_LOG_ENABLED 1 //Uncomment to enable logging. This causes increased current consumption, even if logging is not used.

//Constants
#define DEAD_BEEF                       0xDEADBEEF                        /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

//Timers
#define APP_TIMER_PRESCALER             RUUVITAG_APP_TIMER_PRESCALER      /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_OP_QUEUE_SIZE         RUUVITAG_APP_TIMER_OP_QUEUE_SIZE  /**< Size of timer operation queues. */
// Scheduler settings
APP_TIMER_DEF(main_timer_id);                                             /** Creates timer id for our program, global **/
#define SCHED_MAX_EVENT_DATA_SIZE       MAX(APP_TIMER_SCHED_EVT_SIZE, sizeof(nrf_drv_gpiote_pin_t))
#define SCHED_QUEUE_SIZE                10 //TODO Define queue size

//milliseconds until main loop timer function is called. Other timers can bring
//application out of sleep at higher (or lower) interval
#define MAIN_LOOP_INTERVAL 5000u 





/**
 * @brief Function for doing power management.
 * Uncomment the FPU register clearing if FPU
 * is in use.
 * 
 * Turns off green "activity" led when entering sleep
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
void main_timer_handler(void * p_context)
{
    //nrf_gpio_pin_toggle(LED_RED); //Uncomment to verify timer interval
}

/**
 * @brief Function for application main entry.
 */
int main(void)
{
    uint8_t init_status = 0; // counter, gets incremented by each failed init. It Is 0 in the end if init was ok.

    // Initialize log
    init_status += init_log();

    //Initialize BLE Stack. Required in all applications for timer operation
    init_status += init_ble();

    // Initialize the application timer module.
    init_status += init_timer(main_timer_id, MAIN_LOOP_INTERVAL, main_timer_handler);

    //setup leds. LEDs are active low, so setting them turns leds off.
    init_status += init_leds();

    //Initialize BME 280 and lis2dh12
    init_status += init_sensors();

    //Visually display init status. Hangs if there was an error
    init_blink_status(init_status);

    // Enter main loop.
    for (;; )
    {
         if(NRF_LOG_PROCESS() == false){
           app_sched_execute();
           power_manage();
         }
    }
}
