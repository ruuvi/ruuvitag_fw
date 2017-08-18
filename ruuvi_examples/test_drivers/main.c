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
#include "ble_event_handlers.h"
#include "rtc.h"
#include "rng.h"
#include "application_service_if.h"

#include "iota/iota.h"
#include "iota/constants.h"
#include "libiota.h"

#include  "test_rng.h"
#include  "test_rtc.h"
#include  "test_environmental.h"
#include  "test_mam.h"

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

/** Blocking BLE TX **/
uint32_t tx_data(uint8_t* chunk, uint8_t length)
{
  uint8_t data_array[BLE_NUS_MAX_DATA_LEN] = {0};
  uint32_t       err_code;
  memcpy(&data_array, chunk, length);  
  do
  {
    err_code = ble_nus_string_send(get_nus(), data_array, BLE_NUS_MAX_DATA_LEN);
  }while(NRF_SUCCESS != err_code);
  
  return err_code;
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
  nrf_delay_ms(10);  

  //Init BLE next to init timers and clocks
  err_code |= init_ble();
  NRF_LOG_INFO("BLE init status %s\r\n", (uint32_t)ERR_TO_STR(err_code));
  NRF_LOG_FLUSH();
  nrf_delay_ms(10);

  //Init RTC
  err_code |= init_rtc();
  NRF_LOG_INFO("RTC init status %s\r\n", (uint32_t)ERR_TO_STR(err_code));
  uint32_t test_start = millis();
  nrf_delay_ms(10);  
  

  //Init RNG
  err_code |= init_rng();
  NRF_LOG_INFO("RNG init status %s\r\n", (uint32_t)ERR_TO_STR(err_code));
  NRF_LOG_FLUSH();
  nrf_delay_ms(10);

  //Init LEDs 
  err_code |= init_leds();
  NRF_LOG_INFO("Led init status %s, turning LEDs on for a second\r\n", (uint32_t)ERR_TO_STR(err_code));
  NRF_LOG_FLUSH();
  nrf_delay_ms(100);
  nrf_gpio_pin_clear(LED_RED);
  nrf_gpio_pin_clear(LED_GREEN);
  nrf_delay_ms(1000);
  nrf_gpio_pin_set(LED_RED);
  nrf_gpio_pin_set(LED_GREEN);
  nrf_delay_ms(100);
  
  //Check battery reading
  uint16_t voltage = getBattery();
  NRF_LOG_INFO("Checking battery state... %d mV\r\n", (uint32_t)voltage);
  NRF_LOG_FLUSH();
  nrf_delay_ms(10);
  
  //Start BME280
  err_code |= init_bme280();
  
  NRF_LOG_INFO("BME280 init status %s\r\n", (uint32_t)ERR_TO_STR(err_code));
  
  
  test_rtc();
  test_rng();
  test_environmental();
  test_mam();
  
  bluetooth_advertising_start();  
  
  uint32_t test_end = millis();
  NRF_LOG_INFO("Automated test completed in %d milliseconds\r\n", test_end - test_start);
  nrf_delay_ms(10);  
  
  NRF_LOG_INFO("Waiting for BLE connection\r\n")
  while(!is_ble_connected())
  {
    app_sched_execute();
  }
  NRF_LOG_INFO("BLE connected, waiting for UART notifications to be registered\r\n");
  ble_nus_t* p_nus = get_nus();
  while(!(p_nus->is_notification_enabled))
  {
    app_sched_execute();
  }
  NRF_LOG_INFO("NUS connected, switching to BLE-based test.\r\n");
   
  NRF_LOG_INFO("Starting MAM test\r\n");


  while(1)
  {
    power_manage();
    app_sched_execute();
  }
}


/**
 * @}
 */
