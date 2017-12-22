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
 * Example application: NFC type 2 tag with URI message for RuuviTag.
 * Based on the examples/nfc/record_url example in the nRF5 SDK.
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
#include "nfc_t2t_lib.h"
#include "nfc_uri_msg.h"
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


// Payload URL.
static const uint8_t m_url[] =
  { 'r', 'u', 'u', 'v', 'i', '.', 'c', 'o', 'm' };

uint8_t m_ndef_msg_buf[256];


/**
 * @brief Callback function for handling NFC events.
 */
static void nfc_callback(void * p_context, nfc_t2t_event_t event, const uint8_t * p_data, size_t data_length)
{
  (void)p_context;

  switch (event)
  {
    case NFC_T2T_EVENT_FIELD_ON:
      nrf_gpio_pin_clear(LED_GREEN);
      break;

    case NFC_T2T_EVENT_FIELD_OFF:
      nrf_gpio_pin_clear(LED_RED);
      break;

    default:
      break;
  }
}


/**
 * @brief Initializing NFC type 2 tag with URI message.
 */
int init_nfc_record_url()
{
  // Keep error code. Initialization is OK when still 0 in the end.
  uint32_t err_code = 0;

  // Set up NFC.
  err_code |= nfc_t2t_setup(nfc_callback, NULL);

  // Encode URI message into buffer.
  uint32_t len = sizeof(m_ndef_msg_buf);
  err_code |= nfc_uri_msg_encode(NFC_URI_HTTPS_WWW, m_url, sizeof(m_url), m_ndef_msg_buf, &len);

  // Set created message as the NFC payload.
  err_code |= nfc_t2t_payload_set(m_ndef_msg_buf, len);
  
  // Start sensing NFC field.
  err_code |= nfc_t2t_emulation_start();

  return err_code;
}


/**
 * @brief Function for doing power management.
 */
static void power_manage(void)
{
  // Clear both leds before sleep.
  nrf_gpio_pin_set(LED_GREEN); 
  nrf_gpio_pin_set(LED_RED);       
  
  uint32_t err_code = sd_app_evt_wait();
  APP_ERROR_CHECK(err_code);
}


/**
 * @brief Function for application main entry.
 */
int main(void)
{
  // Keep error code. Initialization is OK when still 0 in the end.
  uint32_t err_code = 0;

  // Initialize log.
  err_code |= init_log();

  // Setup leds. LEDs are active low, so setting high them turns leds off.
  err_code |= init_leds();      // INIT leds first and turn RED on.
  nrf_gpio_pin_clear(LED_RED);  // If INIT fails at later stage, RED will stay lit.

  // Set up NFC type 2 tag with URI message.
  err_code |= init_nfc_record_url();
  
  // Visually display init status. Hangs if there was an error, waits 3 seconds on success.
  init_blink_status(err_code);

  // Turn RED led off.
  nrf_gpio_pin_set(LED_RED);
  // Turn GREEN led on.
  nrf_gpio_pin_clear(LED_GREEN);
  nrf_delay_ms(1000);

  // Init ok, start watchdog with default wdt event handler (reset).
  init_watchdog(NULL);

  // Enter main loop.
  for (;;)
  {
    app_sched_execute();
    power_manage();
  }
}
