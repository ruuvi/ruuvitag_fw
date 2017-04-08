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

#define NRF_LOG_MODULE_NAME "MAIN"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

//BSP
#include "bsp.h"

//Drivers
#include "LIS2DH12.h"
#include "bme280.h"
#include "battery_voltage.h"

//Libraries
#include "base64.h"
#include "eddystone.h"
#include "sensortag.h"


//Init
#include "init.h"

//Configuration
#include "bluetooth_config.h"


//Macros
#define swap_u32(num) ((num>>24)&0xff) | ((num<<8)&0xff0000) | ((num>>8)&0xff00) | ((num<<24)&0xff000000);
#define float2fix(a) ((int)((a)*256.0))   //Convert float to fix. a is a float

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

//Payload requires 8 characters
#define URL_BASE_LENGTH 8
static char url_buffer[16] = {'r', 'u', 'u', '.', 'v', 'i', '/', '#'};
static uint8_t data_buffer[18] = { 0 };
bool model_plus = false; //Flag for sensors available
bool highres    = false; //Flag for used mode

static ruuvi_sensor_t data;

/**@brief Function for doing power management.
 */
static void power_manage(void)
{
    if(1 == nrf_gpio_pin_read(BUTTON_1)) //leave led on button press
    {
        nrf_gpio_pin_set(LED_GREEN); 
        nrf_gpio_pin_set(LED_RED);       //Clear both leds before sleep 
    }
    uint32_t err_code = sd_app_evt_wait();
    APP_ERROR_CHECK(err_code);

    if(highres){  //signal mode by led color
      nrf_gpio_pin_clear(LED_RED); 
    }
    else {
      nrf_gpio_pin_clear(LED_GREEN);
    }
}

// Timeout handler for the repeated timer
static void main_timer_handler(void * p_context)
{
    // Get raw environmental data
    int32_t raw_t = bme280_get_temperature();
    uint32_t raw_p = bme280_get_pressure();
    uint32_t raw_h = bme280_get_humidity();
   
    NRF_LOG_DEBUG("temperature: %d, pressure: %d, humidity: %d", raw_t, raw_p, raw_h);
    
    // Get accelerometer data
    int32_t acc[3] ,accx, accy, accz;
    LIS2DH12_getALLmG(&accx, &accy, &accz);    
    acc[0] = accx;
    acc[1] = accy;
    acc[2] = accz;

    //Get battery voltage
    uint8_t vbat = getBattery();

    //Embed data into structure for parsing
    parseSensorData(&data, raw_t, raw_p, raw_h, vbat, acc);
    
    if(highres)
    {
      //Prepare bytearray to broadcast
      encodeToSensorDataFormat(data_buffer, &data);
    } 
    else 
    {
      encodeToUrlDataFromat(url_buffer, URL_BASE_LENGTH, &data);
    }

}

static void updateAdvertisement(void)
{
  if(highres){
    bluetooth_advertise_data(data_buffer, sizeof(data_buffer));
  }
  else 
  {
    eddystone_advertise_url(url_buffer, sizeof(url_buffer));
    NRF_LOG_DEBUG("Updated eddystone URL");
  }
}

/**
 * @brief Function for application main entry.
 */
int main(void)
{
    uint8_t init_status = 0; // counter, gets incremented by each failed init. It Is 0 in the end if init was ok.
    //setup leds. LEDs are active low, so setting high them turns leds off.
    init_status += init_leds(); //INIT leds first and turn RED on
    nrf_gpio_pin_clear(LED_RED);//If INIT fails at later stage, RED will stay lit.

    // Initialize log
    init_status += init_log();

    // Initialize buttons
    init_status += init_buttons();

    //Initialize BLE Stack. Required in all applications for timer operation
    init_status += init_ble();
    ble_tx_power_set(BLE_TX_POWER);

    // Initialize the application timer module.
    init_status += init_timer(main_timer_id, MAIN_LOOP_INTERVAL, main_timer_handler);

    //Initialize BME 280 and lis2dh12 
    init_status += init_sensors();
    //start accelerometer if present
    LIS2DH12_init(LIS2DH12_POWER_LOW, LIS2DH12_SCALE2G, NULL);
    //setup BME280 if present
    bme280_set_oversampling_hum(BME280_OVERSAMPLING_1);
    bme280_set_oversampling_temp(BME280_OVERSAMPLING_1);
    bme280_set_oversampling_press(BME280_OVERSAMPLING_1);
    NRF_LOG_INFO("BME280 configuration done\r\n");

    //Visually display init status. Hangs if there was an error, waits 3 seconds on success
    init_blink_status(init_status);

    nrf_gpio_pin_set(LED_RED);//Turn RED led off.
	
    //Turn green led on to signal application start
    //LED will be turned off in power_manage
    nrf_gpio_pin_clear(LED_GREEN); 

    // Enter main loop.
    for (;; )
    {
      updateAdvertisement();
      //Log is disabled in SDK_CONFIG, has no effect
      if(NRF_LOG_PROCESS() == false){
         app_sched_execute();
         power_manage();
      }
    }
}
