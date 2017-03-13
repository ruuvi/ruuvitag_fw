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

//Libraries
#include "base64.h"
#include "eddystone.h"

//Init
#include "init.h"

//Configuration
#include "bluetooth_config.h"


//Macros
#define swap_u32(num) ((num>>24)&0xff) | ((num<<8)&0xff0000) | ((num>>8)&0xff00) | ((num<<24)&0xff000000);
#define float2fix(a) ((int)((a)*256.0))   //Convert float to fix. a is a float

//Constants
#define DEAD_BEEF                       0xDEADBEEF                        /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */
/*
0:   uint8_t     format;          // (0x02 = realtime sensor readings base64)
1:   uint8_t     humidity;        // one lsb is 0.5%
2-3: uint16_t    temperature;     // Signed 8.8 fixed-point notation.
4-5: uint16_t    pressure;        // (-50kPa)
*/
#define WEATHER_STATION_URL_FORMAT      0x02				  /**< Base64 */
#define EDDYSTONE_URL_BASE_LENGTH       8                                 /** ruu.vi/# */
#define ENCODED_DATA_LENGTH             8                                 /* 48 bits * (4 / 3) / 8 */

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
#define MAIN_BACK_TO_SLEEP_TIME 60000u //after 1 minute application enters deep sleep if user button has not been pressed.

//Flag to enter system off if application is not started
//to conserve power.
static bool application_started = false;

//flag for analysing sensor data
static volatile bool startRead = false;
static char url_buffer[16] = {'r', 'u', 'u', '.', 'v', 'i', '/', '#'};
// BASE64
char buffer_base64_out [16] = {0};

/**@brief Function for doing power management.

 */

static void power_manage(void)
{
    if(1 == nrf_gpio_pin_read(BUTTON_1)) //leave led on button press
    {
        nrf_gpio_pin_set(LED_GREEN); 
    }
    /* Clear exceptions and PendingIRQ from the FPU unit */
    //__set_FPSCR(__get_FPSCR()  & ~(FPU_EXCEPTION_MASK));      
    //(void) __get_FPSCR();
    //NVIC_ClearPendingIRQ(FPU_IRQn);
    uint32_t err_code = sd_app_evt_wait();
    APP_ERROR_CHECK(err_code);
    if(application_started)
    {
        nrf_gpio_pin_clear(LED_GREEN); 
    }
}

// Timeout handler for the repeated timer
static void main_timer_handler(void * p_context)
{
    static uint32_t counter = 0; //how many loops application has waited to start?
    if(!application_started)
    {
        counter++;
    }

    if((MAIN_BACK_TO_SLEEP_TIME / MAIN_LOOP_INTERVAL) <= counter) 
    {
        sd_power_system_off(); // Program is reset upon leaving OFF.
    }
    startRead = true;
}

// Sensor values
typedef struct 
{
uint8_t     format;         // does not include time
uint8_t     humidity;       // one lsb is 0.5%
uint16_t    temperature;    // Signed 8.8 fixed-point notation.
uint16_t    pressure;       // Pascals (pa)
}ruuvi_sensor_t;

static ruuvi_sensor_t sensor_values;

/**
 *  Read environmental data and update URL being broadcasted
 */
static void readData(void)
{
    // Get raw environmental data
    int32_t raw_t = bme280_get_temperature();
    uint32_t raw_p = bme280_get_pressure();
    uint32_t raw_h = bme280_get_humidity();
   
    NRF_LOG_DEBUG("temperature: %d, pressure: %d, humidity: %d", raw_t, raw_p, raw_h);

    /*
    0:   uint8_t     format;          // (0x02 = realtime sensor readings base64)
    1:   uint8_t     humidity;        // one lsb is 0.5%
    2-3: uint16_t    temperature;     // Signed 8.8 fixed-point notation.
    4-5: uint16_t    pressure;        // (-50kPa)
    */
    //Convert raw values to ruu.vi specification
    //Round values: 1 deg C, 1 hPa, 1% RH 
    sensor_values.format = WEATHER_STATION_URL_FORMAT;
    sensor_values.temperature = (raw_t < 0) ? 0x8000 : 0x0000; //Sign bit
    if(raw_t < 0) raw_t = 0-raw_t; // disrecard sign
    sensor_values.temperature |= (((raw_t * 256) / 100) & 0x7F00);//8:8 signed fixed point, Drop decimals
    sensor_values.pressure = (uint16_t)((raw_p >> 8) - 50000); //Scale into pa, Shift by -50000 pa as per Ruu.vi interface.
    sensor_values.pressure -= sensor_values.pressure % 100; //Drop decimals
    sensor_values.humidity = (uint8_t)(raw_h >> 11); 
    sensor_values.humidity <<= 2; //sensor_values.humidity = (uint8_t)((raw_h/1024) * 2);

    //serialize values into a string
    char pack[6] = {0};
    pack[0] = sensor_values.format;
    pack[1] = sensor_values.humidity;
    pack[2] = (sensor_values.temperature)>>8;
    pack[3] = (sensor_values.temperature)&0xFF;
    pack[4] = (sensor_values.pressure)>>8;
    pack[5] = (sensor_values.pressure)&0xFF;
     
    /// We've got 18-8=10 characters available. Encoding 48 bits using Base64 produces max 8 chars.
    memset(&buffer_base64_out, 0, sizeof(buffer_base64_out)); 
    base64encode(pack, sizeof(pack), buffer_base64_out, ENCODED_DATA_LENGTH);

    // Fill the URL buffer. Eddystone config contains frame type, RSSI and URL scheme.
    url_buffer[0] = 0x72; // r
    url_buffer[1] = 0x75; // u
    url_buffer[2] = 0x75; // u
    url_buffer[3] = 0x2E; // .
    url_buffer[4] = 0x76; // v
    url_buffer[5] = 0x69; // i
    url_buffer[6] = 0x2F; // /
    url_buffer[7] = 0x23; // #      
    memcpy(&url_buffer[EDDYSTONE_URL_BASE_LENGTH], &buffer_base64_out, ENCODED_DATA_LENGTH);
}

static void updateAdvertisement(void)
{
static uint8_t     humidity = 0;
static uint16_t    temperature = 0;
static uint16_t    pressure = 0;


    //Values are "thresholded" by comparing rounded values.
   if(humidity != sensor_values.humidity
     ||temperature != sensor_values.temperature
     ||pressure != sensor_values.pressure)
    {
        eddystone_advertise_url(url_buffer, EDDYSTONE_URL_BASE_LENGTH + ENCODED_DATA_LENGTH);

        humidity = sensor_values.humidity;
        temperature = sensor_values.temperature;
        pressure = sensor_values.pressure;
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

    //Visually display init status. Hangs if there was an error, waits 3 seconds on success
    init_blink_status(init_status);

    nrf_gpio_pin_set(LED_RED);//Turn RED led off.

    //Commented out to avoid confusion when devices are booted up
    //while(1 == nrf_gpio_pin_read(BUTTON_1)){ // Poll the button. Halt program here until pressed
    //    app_sched_execute(); //Avoid scheduler buffer overflow.
    //    power_manage();
    //}//user pressed button, start.
    application_started = true; //set flag
	
    //Lis2dh12RetVal = LIS2DH12_init(LIS2DH12_POWER_LOW, LIS2DH12_SCALE2G, NULL);//start accelerometer // not needed in weather station

    //setup BME280
    bme280_set_oversampling_hum(BME280_OVERSAMPLING_1);
    bme280_set_oversampling_temp(BME280_OVERSAMPLING_1);
    bme280_set_oversampling_press(BME280_OVERSAMPLING_1);
    //Start single measurement
    bme280_set_mode(BME280_MODE_FORCED);
    NRF_LOG_INFO("BME280 configuration done\r\n");


    startRead = true;
    //Turn green led on to signal application start
    //LED will be turned off in power_manage
    nrf_gpio_pin_clear(LED_GREEN); 

    // Enter main loop.
    for (;; )
    {
         //NRF_LOG_DEBUG("Loopin'\r\n");  
         if(startRead)
         {
             startRead = false;
             readData();
             bme280_set_mode(BME280_MODE_FORCED); //Start another measurement for the next time
             updateAdvertisement();
         }
         //Log is disabled in SDK_CONFIG, has no effect
         if(NRF_LOG_PROCESS() == false){
           app_sched_execute();
           power_manage();
         }
    }
}
