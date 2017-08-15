/* Copyright (c) 2014 Nordic Semiconductor. All Rights Reserved.
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
 * @defgroup test_drivers
 * @{
 * @ingroup test_drivers
 * @brief Test Cases for Ruuvitag drivers.
 *
 * This file contains some tests to ensure the Ruuvitag drivers are working correct. The output is transmitted via
 * the J-Link RTT.
 */

#include <stdbool.h>
#include <stdint.h>
#include "nordic_common.h"
#include "softdevice_handler.h"
#include "bsp.h"
#include "app_timer.h"
#include "app_error.h"
#include "init.h"
#include "bme280.h"
#include "LIS2DH12.h"
#include "bluetooth_core.h"
#include "rtc.h"
#include "rng.h"

#define NRF_LOG_MODULE_NAME "MAIN"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#define DEAD_BEEF                       0xDEADBEEF                        /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

/**@brief Callback function for asserts in the SoftDevice.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in]   line_num   Line number of the failing ASSERT call.
 * @param[in]   file_name  File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    NRF_LOG_ERROR("ASSERT at %s:%d", (uint32_t)p_file_name, (uint32_t)line_num);
}

/**@brief Function for doing power management.
 *
 * Waits in sleep until event happens
 */
static void power_manage(void)
{
   uint32_t err_code = sd_app_evt_wait();
   APP_ERROR_CHECK(err_code);
}

/**
 * @brief Function for application main entry.
 */
int main(void)
{
  init_err_code_t err_code = INIT_SUCCESS;
  //Start with log - note, strng representations of error codes are wrong on failures
  err_code |= init_log();
  NRF_LOG_INFO("Log init status %s\r\n", (uint32_t)ERR_TO_STR(err_code));
  NRF_LOG_FLUSH();
  nrf_delay_ms(100);  

  //Init BLE next to init timers and clocks
  err_code |= init_ble();
  NRF_LOG_INFO("BLE init status %s\r\n", (uint32_t)ERR_TO_STR(err_code));
  NRF_LOG_FLUSH();
  nrf_delay_ms(10);

  //Init RTC
  err_code |= init_rtc();
  NRF_LOG_INFO("RTC init status %s\r\n", (uint32_t)ERR_TO_STR(err_code));
  nrf_delay_ms(10);  
  uint32_t start = millis();
  /*for(int ii = 0; ii < 15; ii++)
  {
    NRF_LOG_INFO("Clock is %d\r\n", millis());
    nrf_delay_ms(500);
  }*/
  
  //Init RNG
  err_code |= init_rng();
  NRF_LOG_INFO("RNG init status %s\r\n", (uint32_t)ERR_TO_STR(err_code));
  NRF_LOG_FLUSH();
  nrf_delay_ms(10);
  /*
  NRF_LOG_INFO("Making few coordinates, check correlation\r\n");
  for(int ii = 0; ii < 100; ii++)
  {
    app_sched_execute();
    NRF_LOG_INFO(";%d;%d;\r\n", random(), random());
    NRF_LOG_FLUSH();
  }*/
  
  //Init LEDs 
  err_code |= init_leds();/*
  NRF_LOG_INFO("Led init status %s, turning LEDs on for a second\r\n", (uint32_t)ERR_TO_STR(err_code));
  NRF_LOG_FLUSH();
  nrf_delay_ms(100);
  nrf_gpio_pin_clear(LED_RED);
  nrf_gpio_pin_clear(LED_GREEN);
  nrf_delay_ms(1000);
  nrf_gpio_pin_set(LED_RED);
  nrf_gpio_pin_set(LED_GREEN);
  nrf_delay_ms(100);*/
  
  //Check battery reading
  uint16_t voltage = getBattery();
  NRF_LOG_INFO("Checking battery state... %d mV\r\n", (uint32_t)voltage);
  NRF_LOG_FLUSH();
  nrf_delay_ms(10);
  
  //Start BME280
  /*static int32_t raw_t  = 0;
  static uint32_t raw_p = 0;
  static uint32_t raw_h = 0;
  err_code |= init_bme280();
  NRF_LOG_INFO("BME280 init status %s, Taking a few samples\r\n", (uint32_t)ERR_TO_STR(err_code));
  //First sample
  err_code |= bme280_set_mode(BME280_MODE_FORCED);
  nrf_delay_ms(100);
  //read previous data, start next sample
  bme280_set_mode(BME280_MODE_FORCED);
  nrf_delay_ms(100);
  // Get raw environmental data
  raw_t = bme280_get_temperature();
  raw_p = bme280_get_pressure();
  raw_h = bme280_get_humidity();
  NRF_LOG_INFO("temperature: %d.%d, pressure: %d, humidity: %d\r\n", raw_t/100, raw_t%100, raw_p>>8, raw_h>>10); //Wrong decimals on negative values.
  
  err_code |= bme280_set_mode(BME280_MODE_FORCED);
  nrf_delay_ms(100);
  raw_t = bme280_get_temperature();
  raw_p = bme280_get_pressure();
  raw_h = bme280_get_humidity();
  NRF_LOG_INFO("temperature: %d.%d, pressure: %d, humidity: %d\r\n", raw_t/100, raw_t%100, raw_p>>8, raw_h>>10); //Wrong decimals on negative values.
    
  err_code |= bme280_set_mode(BME280_MODE_FORCED);
  raw_t = bme280_get_temperature();
  raw_p = bme280_get_pressure();
  raw_h = bme280_get_humidity();
  NRF_LOG_INFO("temperature: %d.%d, pressure: %d, humidity: %d\r\n", raw_t/100, raw_t%100, raw_p>>8, raw_h>>10); //Wrong decimals on negative values.  
  NRF_LOG_FLUSH();
  nrf_delay_ms(100);

  err_code |= bme280_set_mode(BME280_MODE_NORMAL);
  NRF_LOG_INFO("Ok, switching to normal mode, status %s.\r\n", (uint32_t)ERR_TO_STR(err_code));
  for(int ii = 0; ii < 15; ii++)
  {
    app_sched_execute();
    raw_t = bme280_get_temperature();
    raw_p = bme280_get_pressure();
    raw_h = bme280_get_humidity();
    NRF_LOG_INFO("temperature: %d.%d, pressure: %d.%d, humidity: %d.%d\r\n", raw_t/100, raw_t%100, raw_p>>8, ((raw_p*1000)>>8)%1000, raw_h>>10, ((raw_h*1000)>>10)%1000); //Wrong decimals on negative values.  
    NRF_LOG_FLUSH();
    nrf_delay_ms(1100);
  }
  
  err_code |= bme280_set_oversampling_hum(BME280_OVERSAMPLING_16);
  NRF_LOG_INFO("Tried to configure while not sleeping, status should not be 0: %d.\r\n", (uint32_t)err_code);
  NRF_LOG_FLUSH();
  nrf_delay_ms(10);
  err_code = bme280_set_mode(BME280_MODE_SLEEP);
  bme280_set_oversampling_hum(BME280_OVERSAMPLING_16);
  bme280_set_oversampling_temp(BME280_OVERSAMPLING_16);
  bme280_set_oversampling_press(BME280_OVERSAMPLING_16);
  NRF_LOG_INFO("Checking status register reading..\r\n");
  err_code |= bme280_set_mode(BME280_MODE_FORCED);
  int  active = bme280_is_measuring();
  NRF_LOG_INFO("BME280 is taking sample? %d\r\n", (uint32_t)active);
  err_code |= bme280_set_mode(BME280_MODE_FORCED);
  active = bme280_is_measuring();
  NRF_LOG_INFO("BME280 is taking sample? %d\r\n", (uint32_t)active);
  err_code |= bme280_set_mode(BME280_MODE_NORMAL);
  NRF_LOG_INFO("Ok, applied oversampling, status %s. Noise should be decreased. Place bme280 to fan exhaust to see the effect.\r\n", (uint32_t)ERR_TO_STR(err_code));
  for(int ii = 0; ii < 15; ii++)
  {
    app_sched_execute();
    raw_t = bme280_get_temperature();
    raw_p = bme280_get_pressure();
    raw_h = bme280_get_humidity();
    NRF_LOG_INFO("temperature: %d.%d, pressure: %d.%d, humidity: %d.%d\r\n", raw_t/100, raw_t%100, raw_p>>8, ((raw_p*1000)>>8)%1000, raw_h>>10, ((raw_h*1000)>>10)%1000); //Wrong decimals on negative values.  
    NRF_LOG_FLUSH();
    nrf_delay_ms(1100);
  }
  
  err_code |= bme280_set_mode(BME280_MODE_SLEEP);
  bme280_set_iir(BME280_IIR_16);
  err_code |= bme280_set_mode(BME280_MODE_NORMAL);
  NRF_LOG_INFO("Ok, applying IIR, status %s. Please breathe on bme280. Values should adjust slowly.\r\n", (uint32_t)ERR_TO_STR(err_code));
  for(int ii = 0; ii < 15; ii++)
  {
    app_sched_execute();
    raw_t = bme280_get_temperature();
    raw_p = bme280_get_pressure();
    raw_h = bme280_get_humidity();
    NRF_LOG_INFO("temperature: %d.%d, pressure: %d.%d, humidity: %d.%d\r\n", raw_t/100, raw_t%100, raw_p>>8, ((raw_p*1000)>>8)%1000, raw_h>>10, ((raw_h*1000)>>10)%1000); //Wrong decimals on negative values.  
    NRF_LOG_FLUSH();
    nrf_delay_ms(1100);
  }
  
  err_code |= bme280_set_mode(BME280_MODE_SLEEP);
  bme280_set_iir(BME280_IIR_OFF);
  err_code |= bme280_set_mode(BME280_MODE_NORMAL);
  NRF_LOG_INFO("Ok, Removing IIR, status %s. Please breathe on bme280. Values should adjust quickly.\r\n", (uint32_t)ERR_TO_STR(err_code));
  for(int ii = 0; ii < 15; ii++)
  {
    app_sched_execute();
    raw_t = bme280_get_temperature();
    raw_p = bme280_get_pressure();
    raw_h = bme280_get_humidity();
    NRF_LOG_INFO("temperature: %d.%d, pressure: %d.%d, humidity: %d.%d\r\n", raw_t/100, raw_t%100, raw_p>>8, ((raw_p*1000)>>8)%1000, raw_h>>10, ((raw_h*1000)>>10)%1000); //Wrong decimals on negative values.  
    NRF_LOG_FLUSH();
    nrf_delay_ms(1100);
  }*/
   
  uint32_t end = millis();
  NRF_LOG_INFO("Test completed in %d milliseconds\r\n", end-start);
  nrf_delay_ms(10);  
  while(1)
  {
    power_manage();
    app_sched_execute();
  }
}


/**
 * @}
 */
