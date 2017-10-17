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

/** STDLIB **/
#include <stdbool.h>
#include <stdint.h>

/** nRF SDK **/
#include "nordic_common.h"
#include "softdevice_handler.h"
#include "bsp.h"
#include "app_timer_appsh.h"
#include "app_error.h"
#include "nrf_delay.h"

/** Ruuvi Drivers **/
#include "battery.h"
#include "init.h"
#include "bme280.h"
#include "lis2dh12.h"
#include "bluetooth_core.h"
#include "ble_event_handlers.h"
#include "ble_bulk_transfer.h"
#include "rtc.h"
#include "rng.h"
#include "pin_interrupt.h"
#include "application_service_if.h"

/** Ruuvi libs **/
#include "ruuvi_endpoints.h"

/** IOTA lib **/
#include "iota/iota.h"
#include "iota/constants.h"
#include "libiota.h"
#include "asciiToTrytes.h"

/** Application tests **/
#include  "test_led.h"
#include  "test_rng.h"
#include  "test_rtc.h"
#include  "test_environmental.h"
#include  "test_lis2dh12.h"
#include  "test_mam.h"
#include  "mam.h"

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
  nrf_gpio_pin_set(LED_RED);
  uint32_t err_code = sd_app_evt_wait();
  APP_ERROR_CHECK(err_code);
  nrf_gpio_pin_clear(LED_RED);
}

ret_code_t button_press_handler(const ruuvi_standard_message_t message)
{
    NRF_LOG_INFO("Button\r\n");
    nrf_gpio_pin_toggle(LED_GREEN);
    return NRF_SUCCESS;
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
  
  err_code |= init_leds();
  NRF_LOG_INFO("Led init status %s.\r\n", (uint32_t)ERR_TO_STR(err_code));

  //Init RTC
  err_code |= init_rtc();
  NRF_LOG_INFO("RTC init status %s\r\n", (uint32_t)ERR_TO_STR(err_code));
  nrf_delay_ms(10);  
  
  //Init RNG
  err_code |= init_rng();
  NRF_LOG_INFO("RNG init status %s\r\n", (uint32_t)ERR_TO_STR(err_code));
  NRF_LOG_FLUSH();
  nrf_delay_ms(10);
  
  //Check battery reading
  uint16_t voltage = getBattery();
  NRF_LOG_INFO("Checking battery state... %d mV\r\n", (uint32_t)voltage);
  NRF_LOG_FLUSH();
  nrf_delay_ms(10);

  //Start interrupts
  err_code |= pin_interrupt_init();

  //Start BME280
  err_code |= init_bme280();
  NRF_LOG_INFO("BME280 init status %s\r\n", (uint32_t)ERR_TO_STR(err_code));
  NRF_LOG_FLUSH();
  nrf_delay_ms(10);

  //Start LIS2DH12
  err_code |= init_lis2dh12();
  NRF_LOG_INFO("LIS2DH12 init status %s\r\n", (uint32_t)ERR_TO_STR(err_code));
  //Erase previous configuration
  lis2dh12_reset();
  //Wait for reboot
  nrf_delay_ms(10);
  lis2dh12_enable();

  
  err_code |= pin_interrupt_enable(BSP_BUTTON_0, NRF_GPIOTE_POLARITY_HITOLO, button_press_handler);

  NRF_LOG_INFO("Interrupt init status %s\r\n", (uint32_t)ERR_TO_STR(err_code));

  //XXX Leaves some setup on
  test_lis2dh12();
  /*
  NRF_LOG_INFO("Starting automated test.\r\n");    
  uint32_t test_start = millis();
  test_rtc();
  test_rng();
  test_led();
  test_environmental();
  
  test_byte_tryte_conversion();
  uint32_t test_end = millis();
  NRF_LOG_INFO("Automated test completed in %d milliseconds\r\n", test_end - test_start);
  */
  
  /*for(int ii = 0; ii < 10; ii++)
  {
  test_mam();
  NRF_LOG_INFO("MAM %d\r\n", (uint32_t) ii);
  }*/
  
  bluetooth_advertising_start();  
  
  set_mam_handler(mam_handler); //XXX POC
  
  while(1)
  {
    //Execute scheduler first
    app_sched_execute();
    //Process queue once schdule has placed new elements to queue
    ble_message_queue_process(); 
    NRF_LOG_DEBUG("Loop\r\n");
    //Might not return until there is a new BLE event
    power_manage();
  }
}


/**
 * @}
 */
