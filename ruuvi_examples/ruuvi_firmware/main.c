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
#include "ble_advertising.h"
#include "nordic_common.h"
#include "softdevice_handler.h"
#include "app_scheduler.h"
#include "app_timer_appsh.h"
#include "nrf_drv_clock.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"

#define NRF_LOG_MODULE_NAME "MAIN"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

//BSP
//#define BSP_SIMPLE
#include "bsp.h"

//Drivers
#include "lis2dh12.h"
#include "bme280.h"
#include "battery.h"
#include "bluetooth_core.h"
#include "eddystone.h"
#include "pin_interrupt.h"

//Libraries
#include "base64.h"
#include "sensortag.h"

//Init
#include "init.h"

//Configuration
#include "bluetooth_config.h"

//Constants
#define DEAD_BEEF                       0xDEADBEEF                        /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

//Timers
#define APP_TIMER_PRESCALER             RUUVITAG_APP_TIMER_PRESCALER      /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_OP_QUEUE_SIZE         RUUVITAG_APP_TIMER_OP_QUEUE_SIZE  /**< Size of timer operation queues. */
// Scheduler settings
APP_TIMER_DEF(main_timer_id);                                             /** Creates timer id for our program **/

//milliseconds until main loop timer function is called. Other timers can bring
//application out of sleep at higher (or lower) interval
#define MAIN_LOOP_INTERVAL_URL 5000u 
#define MAIN_LOOP_INTERVAL_RAW 1000u

//Payload requires 8 characters
#define URL_BASE_LENGTH 8
static char url_buffer[17] = {'r', 'u', 'u', '.', 'v', 'i', '/', '#'};
static uint8_t data_buffer[18] = { 0 };
bool model_plus = false; //Flag for sensors available
bool highres    = true; //Flag for used mode
bool debounce   = true;

static ruuvi_sensor_t data;

void main_timer_handler(void * p_context);


/**@brief Function for handling bsp events.
 * Detects and handles button press
 */
ret_code_t button_press_handler(const ruuvi_standard_message_t message)
{
  NRF_LOG_INFO("Button\r\n");
  //Change mode on button press
  highres = !highres;
  if(model_plus)
  {
    if(highres)
    {
      lis2dh12_set_sample_rate(LIS2DH12_RATE_1);
      app_timer_stop(main_timer_id);
      app_timer_start(main_timer_id, APP_TIMER_TICKS(MAIN_LOOP_INTERVAL_RAW, RUUVITAG_APP_TIMER_PRESCALER), NULL); // 1 event / 1000 ms
      bluetooth_configure_advertising_interval(MAIN_LOOP_INTERVAL_RAW); //Broadcast only updated data, assuming there is an active receiver nearby.
    }
    else
    {
      lis2dh12_set_sample_rate(LIS2DH12_RATE_0);
      app_timer_stop(main_timer_id);
      app_timer_start(main_timer_id, APP_TIMER_TICKS(MAIN_LOOP_INTERVAL_URL, RUUVITAG_APP_TIMER_PRESCALER), NULL); // 1 event / 5000 ms
      bluetooth_configure_advertising_interval(MAIN_LOOP_INTERVAL_URL / 10); //Broadcast often to "hit" occasional background scans.
    }
  }
  void *ptr = NULL;
  main_timer_handler(ptr); //Call timer handler to update data. Will also apply updated advertisement interval

  return ENDPOINT_SUCCESS;
}

/**@brief Function for doing power management.
 */
static void power_manage(void)
{
  if(1 == nrf_gpio_pin_read(BUTTON_1)) //leave leds on button press
  {
    nrf_gpio_pin_set(LED_GREEN); 
    nrf_gpio_pin_set(LED_RED);       //Clear both leds before sleep 
    debounce = true;        
  }
      
  uint32_t err_code = sd_app_evt_wait();
  APP_ERROR_CHECK(err_code);

  if(highres)
  {  //signal mode by led color
    nrf_gpio_pin_clear(LED_RED); 
  }
  else 
  {
    nrf_gpio_pin_clear(LED_GREEN);
  }
}

static void updateAdvertisement(void)
{
  if(highres){
    bluetooth_set_manufacturer_data(data_buffer, sizeof(data_buffer));
    bluetooth_apply_configuration();
  }
  else 
  {
    eddystone_advertise_url(url_buffer, sizeof(url_buffer));
    bluetooth_apply_configuration();
    NRF_LOG_DEBUG("Updated eddystone URL\r\n");
  }
}


// Timeout handler for the repeated timer
void main_timer_handler(void * p_context)
{

    static int32_t  raw_t  = 0;
    static uint32_t raw_p = 0;
    static uint32_t raw_h = 0;
    static lis2dh12_sensor_buffer_t buffer;
    static int32_t acc[3] = {0};

    //If we have all the sensors
    if(model_plus)
    {
      
      // Get raw environmental data
      raw_t = bme280_get_temperature();
      raw_p = bme280_get_pressure();
      raw_h = bme280_get_humidity();
      
      //Start sensor read for next pass
      bme280_set_mode(BME280_MODE_FORCED);
   
      //NRF_LOG_INFO("temperature: %d, pressure: %d, humidity: %d\r\n", raw_t, raw_p, raw_h);
    
      // Get accelerometer data
      lis2dh12_read_samples(&buffer, 1);  
      acc[0] = buffer.sensor.x;
      acc[1] = buffer.sensor.y;
      acc[2] = buffer.sensor.z;
    }
    
    // If only temperature sensor is present
    else 
    {
      int32_t temp;                                        // variable to hold temp reading
      (void)sd_temp_get(&temp);                            // get new temperature
      temp *= 25;                                          // SD return temp * 4. Ruuvi format expects temp * 100. 4*25 = 100.
      raw_t = (int32_t) temp;
    }

    //Get battery voltage
    static uint16_t vbat = 0;
    vbat = getBattery();
    //NRF_LOG_INFO("temperature: , pressure: , humidity: ");
    //Embed data into structure for parsing
    parseSensorData(&data, raw_t, raw_p, raw_h, vbat, acc);
    NRF_LOG_DEBUG("temperature: %d, pressure: %d, humidity: %d x: %d y: %d z: %d\r\n", raw_t, raw_p, raw_h, acc[0], acc[1], acc[2]);
    NRF_LOG_DEBUG("VBAT: %d send %d \r\n", vbat, data.vbat);
    if(highres)
    {
      //Prepare bytearray to broadcast
      encodeToSensorDataFormat(data_buffer, &data);
    } 
    else 
    {
      encodeToUrlDataFromat(url_buffer, URL_BASE_LENGTH, &data);
    }
    updateAdvertisement();
    watchdog_feed();
}


/**
 * @brief Function for application main entry.
 */
int main(void)
{
  ret_code_t err_code = 0; // counter, gets incremented by each failed init. It Is 0 in the end if init was ok.

    
  // Initialize log
  err_code |= init_log();

  //setup leds. LEDs are active low, so setting high them turns leds off.    
  err_code |= init_leds(); //INIT leds first and turn RED on
  nrf_gpio_pin_clear(LED_RED);//If INIT fails at later stage, RED will stay lit.
    
  //Initialize BLE Stack. Required in all applications for timer operation
  err_code |= init_ble();
  bluetooth_tx_power_set(BLE_TX_POWER);
    
  // Initialize the application timer module.
  err_code |= init_timer(main_timer_id, MAIN_LOOP_INTERVAL_URL, main_timer_handler);

  // Initialize button
  //Start interrupts
  err_code |= pin_interrupt_init();
  err_code |= pin_interrupt_enable(BSP_BUTTON_0, NRF_GPIOTE_POLARITY_HITOLO, button_press_handler);    

  //Initialize BME 280 and lis2dh12. Requires timer running.
  if(NRF_SUCCESS == init_sensors())
  {
    model_plus = true;
    //init accelerometer if present
    lis2dh12_init();
    lis2dh12_reset();
    nrf_delay_ms(10);
    lis2dh12_enable();
    lis2dh12_set_scale(LIS2DH12_SCALE2G);
    lis2dh12_set_sample_rate(LIS2DH12_RATE_1);
    lis2dh12_set_resolution(LIS2DH12_RES10BIT);
      
    //setup BME280 while LIS2DH12 is rebooting
    bme280_set_oversampling_hum(BME280_OVERSAMPLING_1);

    bme280_set_oversampling_temp(BME280_OVERSAMPLING_1);
    bme280_set_oversampling_press(BME280_OVERSAMPLING_1);
    bme280_set_mode(BME280_MODE_FORCED);
    NRF_LOG_DEBUG("BME280 configuration done\r\n");
  }
        
  //Take another measurement to let BME 280 filters settle
  if(model_plus){bme280_set_mode(BME280_MODE_FORCED);}        

  //Visually display init status. Hangs if there was an error, waits 3 seconds on success
  init_blink_status(err_code);

  nrf_gpio_pin_set(LED_RED);//Turn RED led off.
  //Turn green led on to signal model +
  //LED will be turned off in power_manage
  nrf_gpio_pin_clear(LED_GREEN); 
    
  if(model_plus){bme280_set_mode(BME280_MODE_FORCED);}
  //delay for model plus, basic will not show green.
  if(model_plus) nrf_delay_ms(1000);
  
  //Init ok, start watchdog with default wdt event handler
  init_watchdog(NULL);

  // Enter main loop.
  for (;; )
  {
    //if(NRF_LOG_PROCESS() == false){
    app_sched_execute();
    power_manage();
  
  }
}
