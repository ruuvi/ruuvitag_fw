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
 * @defgroup ble_sdk_bluetooth_template_main main.c
 * @{
 * @ingroup bluetooth_template
 * @brief bluetooth_template main file.
 *
 * This file contains a template for creating a new application using Bluetooth Developper Studio generated code.
 * It has the code necessary to wakeup from button, advertise, get a connection restart advertising on disconnect and if no new
 * connection created go back to system-off mode.
 * It can easily be used as a starting point for creating a new application, the comments identified
 * with 'YOUR_JOB' indicates where and how you can customize.
 */

#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "ble.h"
#include "ble_hci.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"
#include "boards.h"
#include "softdevice_handler.h"
#include "app_scheduler.h"
#include "app_timer_appsh.h"
#include "peer_manager.h"
#include "fds.h"
#include "fstorage.h"
#include "ble_conn_state.h"
#include "bsp.h"
#include "bsp_btn_ble.h"
#include "service_if.h"
#include "event_handlers.h"

#define NRF_LOG_MODULE_NAME "MAIN"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_ble_qwr.h"


#include "init.h"

//Libraries
#include "bluetooth_core.h"

//Drivers
#include "LIS2DH12.h"
#include "bme280.h"

#define DEAD_BEEF                        0xDEADBEEF                       /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. **/

// Scheduler settings
APP_TIMER_DEF(main_timer_id);                                             /** Creates timer id for our program **/

//milliseconds until main loop timer function is called. Other timers can bring
//application out of sleep at higher (or lower) interval
#define MAIN_LOOP_INTERVAL (1000u)                                        //interval is in milliseconds

/**@brief Callback function for asserts in the SoftDevice.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in] line_num   Line number of the failing ASSERT call.
 * @param[in] file_name  File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}

/**@brief Function for the Power manager.
 */
static void power_manage(void)
{
    //Turns led off
    nrf_gpio_pin_set(LED_GREEN);  //TODO: Use BSP Functions

    uint32_t err_code = sd_app_evt_wait();
    APP_ERROR_CHECK(err_code);

    //Turns led on
    nrf_gpio_pin_clear(LED_GREEN); //TODO: Use BSP Functions
}

static void main_timer_handler(void * p_context)
{
    //Put your periodically executed code here
}


/**@brief Function for application main entry.
 */
int main(void)
{
    // Initialize.
    uint8_t init_status = 0; // counter, gets incremented by each failed init. It Is 0 in the end if init was ok.
    // Initialize log
    init_status += init_log();

    //setup leds. LEDs are active low, so setting high them turns leds off.
    init_status += init_leds(); //INIT leds first and turn RED on
    nrf_gpio_pin_clear(LED_RED);//If INIT fails at later stage, RED will stay lit.

    // Initialize buttons
    init_status += init_buttons();

    //Initialize BLE Stack. Required in all applications for timer operation
    init_status += init_ble();

    init_timer(main_timer_id, MAIN_LOOP_INTERVAL, main_timer_handler);

    //Initialize BME 280
    init_status += init_bme280();
  
    //Visually display init status. Hangs if there was an error, waits 3 seconds on success
    init_blink_status(init_status);

    nrf_gpio_pin_set(LED_RED);//Turn RED led off.

    NRF_LOG_INFO("Execute schedule after init \r\n");
    //Clear schedule to avoid bad data at boot
    app_sched_execute();
    
    //Start sensors
    //25 Hz sample rate, read at 1 Hz
    //LIS2DH12_setPowerMode(LIS2DH12_POWER_BURST);
    bme280_set_mode(BME280_MODE_NORMAL);
    NRF_LOG_INFO("Sensors started \r\n");


    for (;;)
    {
    NRF_LOG_DEBUG("Loopin \r\n");
        if (NRF_LOG_PROCESS() == false)
        {
            //App comes out of power management when timer (such as sensor timers) are triggered. Execute schedule
            app_sched_execute();

            power_manage();// All done, back to sleep
        }
    }
}


/**
 * @}
 */
