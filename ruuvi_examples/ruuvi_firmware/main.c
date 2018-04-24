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
#include "bsp.h"

// Drivers
#include "lis2dh12.h"
#include "lis2dh12_acceleration_handler.h"
#include "bme280.h"
#include "battery.h"
#include "bluetooth_core.h"
#include "eddystone.h"
#include "pin_interrupt.h"
#include "nfc.h"
#include "nfc_t2t_lib.h"
#include "rtc.h"
#include "bluetooth_core.h"

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
#define MAIN_LOOP_INTERVAL_URL   5000u 
#define ADVERTISING_INTERVAL_URL 500u 
#define MAIN_LOOP_INTERVAL_RAW   1000u
#define DEBOUNCE_THRESHOLD 250u

// Payload requires 9 characters
#define URL_BASE_LENGTH 9
#define URL_DATA_LENGTH 9
#define RAW_DATA_LENGTH 14
static char url_buffer[URL_BASE_LENGTH + URL_DATA_LENGTH] = {0x03, 'r', 'u', 'u', '.', 'v', 'i', '/', '#'};
static uint8_t data_buffer[RAW_DATA_LENGTH] = { 0 };
static bool model_plus = false;     // Flag for sensors available
static bool highres = true;        // Flag for used mode
static uint64_t debounce = false;   // Flag for avoiding double presses

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
      lis2dh12_set_sample_rate(LIS2DH12_RATE_1);
      // Reconfigure application sample rate for RAW mode
      app_timer_stop(main_timer_id);
      app_timer_start(main_timer_id, APP_TIMER_TICKS(MAIN_LOOP_INTERVAL_RAW, RUUVITAG_APP_TIMER_PRESCALER), NULL); // 1 event / 1000 ms
      bluetooth_configure_advertising_interval(MAIN_LOOP_INTERVAL_RAW); // Broadcast only updated data, assuming there is an active receiver nearby.
      bluetooth_apply_configuration();
    }
    else
    {
      // Stop accelerometer as it's not useful on URL mode.
      lis2dh12_set_sample_rate(LIS2DH12_RATE_0);
      // Reconfigure application sample rate for URL mode.
      app_timer_stop(main_timer_id);
      app_timer_start(main_timer_id, APP_TIMER_TICKS(MAIN_LOOP_INTERVAL_URL, RUUVITAG_APP_TIMER_PRESCALER), NULL); // 1 event / 5000 ms
      bluetooth_configure_advertising_interval(ADVERTISING_INTERVAL_URL); // Broadcast often to "hit" occasional background scans.
      bluetooth_apply_configuration();
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
  app_sched_event_put (NULL, 0, change_mode);

  return ENDPOINT_SUCCESS;
}

/**
 * Work around NFC data corruption bug by reinitializing NFC data after field has been lost.
 * Call this function outside of interrupt context.
 */
static void reinit_nfc(void* data, uint16_t length)
{
  init_nfc();
}

/**@brief Function for handling NFC events.
 * Schedulers call to handler.
 */
void app_nfc_callback(void* p_context, nfc_t2t_event_t event, const uint8_t* p_data, size_t data_length)
{
  NRF_LOG_INFO("NFC\r\n");
  switch (event)
  {
  case NFC_T2T_EVENT_FIELD_ON:
    NRF_LOG_INFO("NFC Field detected \r\n");
    break;
  case NFC_T2T_EVENT_FIELD_OFF:
    NRF_LOG_INFO("NFC Field lost \r\n");
    app_sched_event_put (NULL, 0, reinit_nfc);
    break;
  case NFC_T2T_EVENT_DATA_READ:
    NRF_LOG_INFO("Data read\r\n");
  default:
    break;
  }
}


/**@brief Function for doing power management.
 */
static void power_manage(void)
{
  // Clear both leds before sleep.
  nrf_gpio_pin_set(LED_GREEN);
  nrf_gpio_pin_set(LED_RED);

  uint32_t err_code = sd_app_evt_wait();
  NRF_LOG_INFO("SD_Wait status %d\r\n", err_code);
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
  int32_t  raw_t  = 0;
  uint32_t raw_p = 0;
  uint32_t raw_h = 0;
  lis2dh12_sensor_buffer_t buffer;
  int32_t acc[3] = {0};
  static bool fast_advetising = true;

  if (fast_advetising && millis() > ADVERTISING_STARTUP_PERIOD)
  {
    fast_advetising = false;
    if (highres) { bluetooth_configure_advertising_interval(ADVERTISING_INTERVAL_RAW);}
    else {bluetooth_configure_advertising_interval(ADVERTISING_INTERVAL_URL);}
  }

  // If we have all the sensors.
  if (model_plus)
  {
    // Get raw environmental data.
    bme280_read_measurements();
    raw_t = bme280_get_temperature();
    raw_p = bme280_get_pressure();
    raw_h = bme280_get_humidity();

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

<<<<<<< HEAD
    // Get battery voltage
    //static uint32_t vbat_update_counter;
    static uint16_t vbat = 0;
    vbat = getBattery();
    //NRF_LOG_INFO("temperature: , pressure: , humidity: ");
    // Embed data into structure for parsing.
    
    parseSensorData(&data, raw_t, raw_p, raw_h, vbat, acc);
    NRF_LOG_DEBUG("temperature: %d, pressure: %d, humidity: %d x: %d y: %d z: %d\r\n", raw_t, raw_p, raw_h, acc[0], acc[1], acc[2]);
    NRF_LOG_DEBUG("VBAT: %d send %d \r\n", vbat, data.vbat);
    if (highres)
    {
      // Prepare bytearray to broadcast.
      encodeToSensorDataFormat(data_buffer, &data);
    } 
    else 
    {
      encodeToUrlDataFromat(url_buffer, URL_BASE_LENGTH, &data);
    }
=======
  // Get battery voltage
  //static uint32_t vbat_update_counter;
  static uint16_t vbat = 0;
  vbat = getBattery();

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
>>>>>>> b490eae... Reinit NFC after connection, Advertise faster at boot

  updateAdvertisement();
  watchdog_feed();
}


/**
<<<<<<< HEAD
=======
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
>>>>>>> b490eae... Reinit NFC after connection, Advertise faster at boot
 * @brief Function for application main entry.
 */
int main(void)
{
<<<<<<< HEAD


  ret_code_t err_code = 0; // counter, gets incremented by each failed init. It is 0 in the end if init was ok.
  nrf_delay_ms(10);
=======
  ret_code_t err_code = NRF_SUCCESS; // counter, gets incremented by each failed init. It is 0 in the end if init was ok.

  // Initialize log.
>>>>>>> b490eae... Reinit NFC after connection, Advertise faster at boot
  err_code |= init_log();
  // Initialize log.

  // Setup leds. LEDs are active low, so setting high them turns leds off.
  err_code |= init_leds();      // INIT leds first and turn RED on.
  nrf_gpio_pin_clear(LED_RED);  // If INIT fails at later stage, RED will stay lit.
  nrf_delay_ms(10);
  
  NRF_LOG_INFO("LOG INIT \r\n");
  
  if(NRF_SUCCESS == init_sensors()) 
  { 
    NRF_LOG_INFO("SENSORS INIT \r\n");
    model_plus = true; 
  }
  nrf_delay_ms(10);

<<<<<<< HEAD
  
=======
  //Init NFC ASAP in case we're waking from deep sleep via NFC (todo)
  set_nfc_callback(app_nfc_callback);
  err_code |= init_nfc();

  if (NRF_SUCCESS == init_sensors()) { model_plus = true; }

  // Initialize BLE Stack. Required in all applications for timer operation.
  err_code |= init_ble();
  bluetooth_configure_advertisement_type(APPLICATION_ADVERTISEMENT_TYPE);
  bluetooth_tx_power_set(BLE_TX_POWER);
  bluetooth_configure_advertising_interval(ADVERTISING_INTERVAL_STARTUP);
>>>>>>> b490eae... Reinit NFC after connection, Advertise faster at boot


  //Init NFC ASAP in case we're waking from deep sleep via NFC (todo)
  err_code |= init_nfc();
  NRF_LOG_INFO("NFC INIT \r\n");
  nrf_delay_ms(10);

  // Start interrupts.
  err_code |= pin_interrupt_init();
  nrf_delay_ms(10);
  // Initialize button.
  err_code |= pin_interrupt_enable(BSP_BUTTON_0, NRF_GPIOTE_POLARITY_HITOLO, button_press_handler);
  nrf_delay_ms(10);

  // Interrupt handler is defined in lis2dh12_acceleration_handler.c, reads the buffer and passes the data onwards to application as configured.
  // Try using PROPRIETARY as a target of accelerometer to implement your own logic.
  err_code |= pin_interrupt_enable(INT_ACC1_PIN, NRF_GPIOTE_POLARITY_LOTOHI, lis2dh12_int1_handler);
  NRF_LOG_INFO("INTERRUPT INIT \r\n");
  nrf_delay_ms(10);

  // Initialize BME 280 and lis2dh12.
  if (model_plus)
  {
    // Clear memory.
    lis2dh12_reset();
    // Wait for reboot.
    nrf_delay_ms(10);
    // Enable XYZ axes.
    lis2dh12_enable();
    lis2dh12_set_scale(LIS2DH12_SCALE2G);
    // Sample rate 10 for activity detection.
<<<<<<< HEAD
    lis2dh12_set_sample_rate(LIS2DH12_RATE_1);
    lis2dh12_set_resolution(LIS2DH12_RES12BIT);
    NRF_LOG_INFO("LIS INIT \r\n");
    nrf_delay_ms(10);
=======
    lis2dh12_set_sample_rate(LIS2DH12_SAMPLERATE_RAW);
    lis2dh12_set_resolution(LIS2DH12_RESOLUTION);

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
    ctrl[0] = LIS2DH12_ACTIVITY_THRESHOLD;
    lis2dh12_write_register(LIS2DH12_INT2_THS, ctrl, 1);

    // Enable LOTOHI interrupt on nRF52.
    err_code |= pin_interrupt_enable(INT_ACC2_PIN, NRF_GPIOTE_POLARITY_LOTOHI, lis2dh12_int2_handler);

    // Enable Interrupt function 2 on LIS interrupt pin 2 (stays high for 1/ODR).
    lis2dh12_set_interrupts(LIS2DH12_I2C_INT2_MASK, 2);
>>>>>>> b490eae... Reinit NFC after connection, Advertise faster at boot

    // Setup BME280 - oversampling must be set for each used sensor.
    bme280_set_oversampling_hum(BME280_OVERSAMPLING_1);
    bme280_set_oversampling_temp(BME280_OVERSAMPLING_1);
    bme280_set_oversampling_press(BME280_OVERSAMPLING_1);
    bme280_set_iir(BME280_IIR_16);
    bme280_set_interval(BME280_STANDBY_1000_MS);
    bme280_set_mode(BME280_MODE_NORMAL);
    NRF_LOG_INFO("BME280 configuration done\r\n");
    nrf_delay_ms(10);
  }

  // Initialize BLE Stack. Required in all applications for timer operation.
  err_code |= init_ble();
  nrf_delay_ms(10);
  // Start advertising only after sensors have valid data

  err_code |= bluetooth_tx_power_set(BLE_TX_POWER);
  err_code |= bluetooth_configure_advertising_interval(MAIN_LOOP_INTERVAL_RAW);
  err_code |= bluetooth_configure_advertisement_type(BLE_GAP_ADV_TYPE_ADV_NONCONN_IND);
  NRF_LOG_INFO("BLE INIT \r\n");
  nrf_delay_ms(10);

  // Init ok, start watchdog with default wdt event handler (reset).
  init_watchdog(NULL);
  NRF_LOG_INFO("WATCHDOG INIT \r\n");
  nrf_delay_ms(10);

  // Used only for button debounce
  err_code |= init_rtc();
  NRF_LOG_INFO("RTC INIT \r\n");
  nrf_delay_ms(10);

  // Initialize the application timer module.
  err_code |= init_timer(main_timer_id, MAIN_LOOP_INTERVAL_RAW, main_timer_handler);
  NRF_LOG_INFO("TIMER INIT \r\n");
  nrf_delay_ms(10);

  // Visually display init status. Hangs if there was an error, waits 3 seconds on success.
  init_blink_status(err_code);

  nrf_gpio_pin_set(LED_RED);  // Turn RED led off.
  // Turn green led on to signal model +
  // LED will be turned off in power_manage.
  if (model_plus) { nrf_gpio_pin_clear(LED_GREEN); }

  // Delay before advertising so we get valid data on first packet
  nrf_delay_ms(MAIN_LOOP_INTERVAL_RAW + 100);

  // Enter main loop.
  bluetooth_advertising_start();
  NRF_LOG_INFO("ADVERTISING START  \r\n");
  nrf_delay_ms(10);
  for (;;)
  {
    app_sched_execute();
    power_manage();
  }
}
