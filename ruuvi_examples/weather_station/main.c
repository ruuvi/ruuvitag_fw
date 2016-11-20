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

//BSP
#include "bsp.h"

//Drivers
#include "LIS2DH12.h"
#include "bme280.h"

//Libraries
#include "base91.h"
#include "eddystone.h"


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


//flag for analysing sensor data
static volatile bool startRead = false;
// BASE91
static struct basE91 b91;
static char url_buffer[16] = {'r', 'u', 'u', '.', 'v', 'i', '#'};
char buffer_base91_out [16] = {0};
size_t enc_data_len = 0;

/**@brief Function for doing power management.

 */

static void power_manage(void)
{
    nrf_gpio_pin_set(17);
      /* Clear exceptions and PendingIRQ from the FPU unit */
      //__set_FPSCR(__get_FPSCR()  & ~(FPU_EXCEPTION_MASK));      
      //(void) __get_FPSCR();
      //NVIC_ClearPendingIRQ(FPU_IRQn);
    uint32_t err_code = sd_app_evt_wait();
    APP_ERROR_CHECK(err_code);
    nrf_gpio_pin_clear(17); 

    //TODO: add counter to unset the pin after ~ten blinks or so
}

// Timeout handler for the repeated timer

static void main_timer_handler(void * p_context)
{
    startRead = true;
}

// Sensor values
typedef struct 
{
uint8_t     format;         // Includes time format
uint8_t     humidity;       // one lsb is 0.5%
uint16_t    temperature;    // Signed 8.8 fixed-point notation.
uint16_t    pressure;       // Todo
uint16_t    time;           // Seconds, minutes or hours from last movement
}ruuvi_sensor_t;

static ruuvi_sensor_t sensor_values;

/* Quick'n'Dirty FIR high pass
 * @param uX acceleration in X-direction (mg)
 * @param uY acceleration in Y-direction (mg)
 * @param uZ acceleration in Z-direction (mg)
 * return true if high-passed acceleration amlitude exceeds defined threshold
 *//*
static bool detectMovement(int16_t uX, int16_t uY, int16_t uZ)
{
  static int16_t aX = 0;
  static int16_t aY = 0;
  static int16_t aZ = 0;

  int32_t yX = uX - aX;
  int32_t yY = uY - aY;
  int32_t yZ = uZ - aZ;

  aX = uX;
  aY = uY;
  aZ = uZ;

  int32_t amplitude = sqrt(yX*yX + yY*yY + yZ*yZ);

  return (amplitude > 200);  

}*/

static void readData(void)
{
   //int32_t accX, accY, accZ;
   //Get raw accelerometer values
   //LIS2DH12_getALLmG(&accX, &accY, &accZ);
   //NRF_LOG_DEBUG ("X-Axis: %d, Y-Axis: %d, Z-Axis: %d", accX, accY, accZ);
   //Detect movement
   //bool moving = detectMovement(accX, accY, accZ);
  
   //if(moving){
   //    sensor_values.time = 0;
   //}
   //else{
   //    sensor_values.time += 5; //TODO: Use actual RTC values to avoid drift.
   //}
   //NRF_LOG_DEBUG ("Time: %d", sensor_values.time);

   // Get raw environmental data
   int32_t raw_t = bme280_get_temperature();
   uint32_t raw_p = bme280_get_pressure();
   uint32_t raw_h = bme280_get_humidity();
   
   NRF_LOG_DEBUG("temperature: %d, pressure: %d, humidity: %d", raw_t, raw_p, raw_h);

   //Convert raw values to ruu.vi specification
   sensor_values.temperature = raw_t * 256 / 100;
   sensor_values.pressure = (uint16_t)((raw_p/256) - 50000);
   sensor_values.humidity = (uint8_t)((raw_h/1024) * 2);

   //Base91 encode
   memset(&buffer_base91_out, 0, sizeof(buffer_base91_out)); 
   enc_data_len = basE91_encode(&b91, &sensor_values, sizeof(sensor_values), buffer_base91_out);
   enc_data_len += basE91_encode_end(&b91, buffer_base91_out + enc_data_len);
   memset(&b91, 0, sizeof(b91));

   // Fill the URL buffer. Eddystone config contains frame type, RSSI and URL scheme.
   url_buffer[0] = 0x72; // r
   url_buffer[1] = 0x75; // u
   url_buffer[2] = 0x75; // u
   url_buffer[3] = 0x2e; // .
   url_buffer[4] = 0x76; // v
   url_buffer[5] = 0x69; // i
   url_buffer[6] = 0x23; // #        

   /// We've got 18-7=11 characters available. Encoding 64 bits using Base91 produces max 9 value. All good.
   
   memcpy(&url_buffer[7], &buffer_base91_out, enc_data_len);
}

static void updateAdvertisement(void)
{
    //static uint32_t prev_p = 1;
    //static prev_t = 0;
    //static prev_h = 0;


    //TODO: threshold
    if(1)
    {
        //prev_p = 0; //sensor_values.raw_p
        //prev_t = sensor_values.raw_t
        //prev_h = sensor_values.raw_h

        eddystone_advertise_url(url_buffer, 6 + enc_data_len);

        NRF_LOG_DEBUG("Updated eddystone URL");
    }

}

/**
 * @brief Function for application main entry.
 */
int main(void)
{
    uint32_t err_code;
    
    //Initialize variables
    sensor_values.format = 1;
    sensor_values.time = 0;

    //basE91_init(&b91);


    // Initialize log
    err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    //Initialize Eddystone
    eddystone_init();
    NRF_LOG_INFO("Eddystone init\r\n");

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
    err_code = app_timer_start(main_timer_id, APP_TIMER_TICKS(5000u, APP_TIMER_PRESCALER), NULL); // 1 event / 5000 ms
    APP_ERROR_CHECK(err_code);

    //setup leds. LEDs are active low, so setting them turns leds off.
    nrf_gpio_cfg_output	(17);
    nrf_gpio_pin_set(17);
    nrf_gpio_cfg_output	(19);
    nrf_gpio_pin_set(19);

/*    LIS2DH12_Ret Lis2dh12RetVal;
	
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
    }*/

    NRF_LOG_INFO("BME280 init Start\r\n");
    // Read calibration
    bme280_init();
    //setup sensor readings
    bme280_set_oversampling_hum(BME280_OVERSAMPLING_1);
    bme280_set_oversampling_temp(BME280_OVERSAMPLING_1);
    bme280_set_oversampling_press(BME280_OVERSAMPLING_1);
    //Start single measurement
    bme280_set_mode(BME280_MODE_FORCED);

    NRF_LOG_INFO("BME280 init done\r\n");


    startRead = true;

    // Enter main loop.
    for (;; )
    {
         //NRF_LOG_DEBUG("Loopin'\r\n");
         

         
         if(startRead)
         {
             startRead = false;
             readData();
             bme280_set_mode(BME280_MODE_FORCED); //Take another measurement for the next time
             updateAdvertisement();
         }
         //Log is disabled in SDK_CONFIG, has no effect
         if(NRF_LOG_PROCESS() == false){
           app_sched_execute();
           power_manage();
         }
    }
}


/**
 * @}
 */
