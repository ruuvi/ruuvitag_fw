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

/**
 * Firmware for the RuuviTag B with weather-station functionality.
 */

// STDLIB
#include <stdbool.h>
#include <stdint.h>
#include <math.h>

// Nordic SDK
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

// BSP
//#define BSP_SIMPLE
#include "bsp.h"

// Drivers
#include "lis2dh12.h"
#include "bme280.h"
#include "battery.h"
#include "bluetooth_core.h"
#include "eddystone.h"
#include "pin_interrupt.h"
#include "rtc.h"

// Libraries
#include "base64.h"
#include "sensortag.h"

// Init
#include "init.h"

// Configuration
#include "bluetooth_config.h"

// Constants
#define DEAD_BEEF               0xDEADBEEF    //!< Value used as error code on stack dump, can be used to identify stack location on stack unwind.

// ID for main loop timer.
APP_TIMER_DEF(main_timer_id);                 // Creates timer id for our program.

// milliseconds until main loop timer function is called. Other timers can bring
// application out of sleep at higher (or lower) interval.
#define MAIN_LOOP_INTERVAL_URL 5000u 
#define MAIN_LOOP_INTERVAL_RAW APPLICATION_ADV_INTERVAL
#define DEBOUNCE_THRESHOLD 250u

// Payload requires 8 characters
#define URL_BASE_LENGTH 9
static char url_buffer[17] = {0x03, 'r', 'u', 'u', '.', 'v', 'i', '/', '#'};
static uint8_t data_buffer[24] = { 0 };
static bool model_plus = false;     // Flag for sensors available
static bool highres = false;        // Flag for used mode
static uint64_t debounce = false;   // Flag for avoiding double presses
static uint16_t acceleration_events = 0;

static ruuvi_sensor_t data;

static void main_timer_handler(void * p_context);


/**@brief Handler for button press.
 * Called in scheduler, out of interrupt context.
 */
void change_mode(void* data, uint16_t length)
{
  // Avoid double presses
  if ((millis() - debounce) < DEBOUNCE_THRESHOLD) { return; }
  debounce = millis();
  highres = !highres;
  if (model_plus)
  {
    if (highres)
    {
      //TODO: #define sample rate for application
      lis2dh12_set_sample_rate(LIS2DH12_RATE_10);
      // Reconfigure application sample rate for RAW mode
      app_timer_stop(main_timer_id);
      app_timer_start(main_timer_id, APP_TIMER_TICKS(MAIN_LOOP_INTERVAL_RAW, RUUVITAG_APP_TIMER_PRESCALER), NULL); // 1 event / 1000 ms
      bluetooth_configure_advertising_interval(APPLICATION_ADV_INTERVAL); // Broadcast only updated data, assuming there is an active receiver nearby.
    }
    else
    {
      // Stop accelerometer as it's not useful on URL mode.
      lis2dh12_set_sample_rate(LIS2DH12_RATE_0);
      // Reconfigure application sample rate for URL mode.
      app_timer_stop(main_timer_id);
      app_timer_start(main_timer_id, APP_TIMER_TICKS(MAIN_LOOP_INTERVAL_URL, RUUVITAG_APP_TIMER_PRESCALER), NULL); // 1 event / 5000 ms
      bluetooth_configure_advertising_interval(APPLICATION_ADV_INTERVAL / 2); // Broadcast often to "hit" occasional background scans.
    }
  }
  NRF_LOG_INFO("Updating in %d mode\r\n", (uint32_t) highres);
  main_timer_handler(NULL);
}


/**@brief Function for handling button events.
 * Schedulers call to handler.
 */
ret_code_t button_press_handler(const ruuvi_standard_message_t message)
{
  NRF_LOG_INFO("Button\r\n");
  nrf_gpio_pin_clear(LED_RED);
  nrf_gpio_pin_clear(LED_GREEN);
  //Change mode on button press
  //Use scheduler, do not use peripherals in interrupt conext (SPI write halts)
  app_sched_event_put	(NULL, 0, change_mode);

  return ENDPOINT_SUCCESS;
}


/**@brief Function for doing power management.
 */
static void power_manage(void)
{
  // Clear both leds before sleep.
  nrf_gpio_pin_set(LED_GREEN); 
  nrf_gpio_pin_set(LED_RED);       
  
  uint32_t err_code = sd_app_evt_wait();
  APP_ERROR_CHECK(err_code);

  // Signal mode by led color.
  if (highres) { nrf_gpio_pin_clear(LED_RED); }
  else { nrf_gpio_pin_clear(LED_GREEN); }
}


static void updateAdvertisement(void)
{
  ret_code_t err_code = NRF_SUCCESS;
  if (highres) { err_code |= bluetooth_set_manufacturer_data(data_buffer, sizeof(data_buffer)); }
  else { err_code |= bluetooth_set_eddystone_url(url_buffer, sizeof(url_buffer)); }
  NRF_LOG_DEBUG("Updating data, data status %d\r\n", err_code);
}


/**@brief Timeout handler for the repeated timer
 */
void main_timer_handler(void * p_context)
{
    static int32_t  raw_t  = 0;
    static uint32_t raw_p = 0;
    static uint32_t raw_h = 0;
    static lis2dh12_sensor_buffer_t buffer;
    static int32_t acc[3] = {0};

    // If we have all the sensors.
    if (model_plus)
    {      
      // Get raw environmental data.
      raw_t = bme280_get_temperature();
      raw_p = bme280_get_pressure();
      raw_h = bme280_get_humidity();
      
      // Start next measurement - causes up to URL_LOOP_INTERVAL latency in measurements.
      //bme280_set_mode(BME280_MODE_FORCED);

      // Get accelerometer data.
      lis2dh12_read_samples(&buffer, 1);  
      acc[0] = buffer.sensor.x;
      acc[1] = buffer.sensor.y;
      acc[2] = buffer.sensor.z;
    }
    // If only temperature sensor is present.
    else
    {
      int32_t temp;                                        // variable to hold temp reading
      (void)sd_temp_get(&temp);                            // get new temperature
      temp *= 25;                                          // SD returns temp * 4. Ruuvi format expects temp * 100. 4*25 = 100.
      raw_t = (int32_t) temp;
    }

    // Get battery voltage every 30.th cycle
    static uint32_t vbat_update_counter;
    static uint16_t vbat = 0;
    if(!(vbat_update_counter++%30)) { vbat = getBattery(); }
    //NRF_LOG_INFO("temperature: , pressure: , humidity: ");
    // Embed data into structure for parsing.
    parseSensorData(&data, raw_t, raw_p, raw_h, vbat, acc);
    NRF_LOG_DEBUG("temperature: %d, pressure: %d, humidity: %d x: %d y: %d z: %d\r\n", raw_t, raw_p, raw_h, acc[0], acc[1], acc[2]);
    NRF_LOG_DEBUG("VBAT: %d send %d \r\n", vbat, data.vbat);
    if (highres)
    {
      // Prepare bytearray to broadcast.
      bme280_data_t environmental;
      environmental.temperature = raw_t;
      environmental.humidity = raw_h;
      environmental.pressure = raw_p;
      encodeToRawFormat5(data_buffer, &environmental, &buffer.sensor, acceleration_events, vbat, BLE_TX_POWER);
    } 
    else 
    {
      encodeToUrlDataFromat(url_buffer, URL_BASE_LENGTH, &data);
    }

    updateAdvertisement();
    watchdog_feed();
}


/**
 * @brief Handle interrupt from lis2dh12.
 * Never do long actions, such as sensor reads in interrupt context.
 * Using peripherals in interrupt is also risky,
 * as peripherals might require interrupts for their function.
 *
 *  @param message Ruuvi message, with source, destination, type and 8 byte payload. Ignore for now.
 **/
ret_code_t lis2dh12_int2_handler(const ruuvi_standard_message_t message)
{
    NRF_LOG_DEBUG("Accelerometer interrupt to pin 2\r\n");
    acceleration_events++;
    /*
    app_sched_event_put ((void*)(&message),
                         sizeof(message),
                         lis2dh12_scheduler_event_handler);
    */
    return NRF_SUCCESS;
}


/**
 * @brief Function for application main entry.
 */
int main(void)
{
  ret_code_t err_code = 0; // counter, gets incremented by each failed init. It is 0 in the end if init was ok.

  // Initialize log.
  err_code |= init_log();

  // Setup leds. LEDs are active low, so setting high them turns leds off.
  err_code |= init_leds();      // INIT leds first and turn RED on.
  nrf_gpio_pin_clear(LED_RED);  // If INIT fails at later stage, RED will stay lit.

  err_code |= init_nfc();

  // Initialize BLE Stack. Required in all applications for timer operation.
  err_code |= init_ble();
  bluetooth_advertising_stop();
  bluetooth_tx_power_set(BLE_TX_POWER);

  // Initialize the application timer module.
  err_code |= init_timer(main_timer_id, MAIN_LOOP_INTERVAL_RAW, main_timer_handler);

  // Initialize RTC.
  err_code |= init_rtc();

  // Start interrupts.
  err_code |= pin_interrupt_init();
  // Initialize button.
  err_code |= pin_interrupt_enable(BSP_BUTTON_0, NRF_GPIOTE_POLARITY_HITOLO, button_press_handler);

  // Initialize BME 280 and lis2dh12. Requires timer running.
  if (NRF_SUCCESS == init_sensors())
  {
    model_plus = true;

    // Clear memory.
    lis2dh12_reset();
    // Wait for reboot.
    nrf_delay_ms(10);
    // Enable XYZ axes.
    lis2dh12_enable();
    lis2dh12_set_scale(LIS2DH12_SCALE2G);
    // Sample rate 10 for activity detection.
    lis2dh12_set_sample_rate(LIS2DH12_RATE_10);
    lis2dh12_set_resolution(LIS2DH12_RES10BIT);

    //XXX If you read this, I'm sorry about line below.
    #include "lis2dh12_registers.h"
    // Configure activity interrupt - TODO: Implement in driver, add tests.
    uint8_t ctrl[1];
    // Enable high-pass for Interrupt function 2.
    //CTRLREG2 = 0x02
    ctrl[0] = LIS2DH12_HPIS2_MASK;
    lis2dh12_write_register(LIS2DH12_CTRL_REG2, ctrl, 1);
    
    // Enable interrupt 2 on X-Y-Z HI/LO.
    //INT2_CFG = 0x7F
    ctrl[0] = 0x7F;
    lis2dh12_write_register(LIS2DH12_INT2_CFG, ctrl, 1);    
    // Interrupt on 64 mg+ (highpassed, +/-).
    //INT2_THS= 0x04 // 4 LSB = 64 mg @2G scale
    ctrl[0] = 0x04;
    lis2dh12_write_register(LIS2DH12_INT2_THS, ctrl, 1);
        
    // Enable LOTOHI interrupt on nRF52.
    err_code |= pin_interrupt_enable(INT_ACC2_PIN, NRF_GPIOTE_POLARITY_LOTOHI, lis2dh12_int2_handler);
    
    // Enable Interrupt function 2 on LIS interrupt pin 2 (stays high for 1/ODR).
    lis2dh12_set_interrupts(LIS2DH12_I2C_INT2_MASK, 2);

    // Setup BME280 - oversampling must be set for each used sensor.
    bme280_set_oversampling_hum(BME280_OVERSAMPLING_1);
    bme280_set_oversampling_temp(BME280_OVERSAMPLING_1);
    bme280_set_oversampling_press(BME280_OVERSAMPLING_1);
    bme280_set_iir(BME280_IIR_16);
    bme280_set_interval(BME280_STANDBY_1000_MS);
    bme280_set_mode(BME280_MODE_NORMAL);
    NRF_LOG_DEBUG("BME280 configuration done\r\n");
    highres = true;
  }

  // Visually display init status. Hangs if there was an error, waits 3 seconds on success.
  init_blink_status(err_code);

  nrf_gpio_pin_set(LED_RED);  // Turn RED led off.
  // Turn green led on to signal model +
  // LED will be turned off in power_manage.
  nrf_gpio_pin_clear(LED_GREEN); 

  // Delay for model plus, basic will not show green.
  if (model_plus) nrf_delay_ms(1000);

  // Init ok, start watchdog with default wdt event handler (reset).
  init_watchdog(NULL);
  bluetooth_advertising_start();

  // Enter main loop.
  for (;;)
  {
    app_sched_execute();
    power_manage();
  }
}
