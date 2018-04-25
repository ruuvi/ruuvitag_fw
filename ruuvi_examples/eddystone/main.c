/* Copyright (c) 2016 Nordic Semiconductor. All Rights Reserved.
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
 * Example application: Eddystone Beacon for RuuviTag.
 * Based on /examples/ble_peripheral/ble_app_eddystone in the nRF SDK .
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "bsp.h"
#include "boards.h"
#include "bsp_board_config.h"
#include "ble_gap.h"
#include "ble_conn_params.h"
#include "ble_advertising.h"
#include "softdevice_handler.h"
#include "app_uart.h"
#include "app_timer_appsh.h"
#include "es_app_config.h"
#include "app_scheduler.h"
#include "nrf_ble_escs.h"
#include "nrf_ble_es.h"
#include "fstorage.h"
#include "nrf_delay.h"
#include "nrf_ble_es.h"

#include "event_handlers.h"

#define NRF_LOG_MODULE_NAME "MAIN"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#include "init.h"
#include "bluetooth_core.h"
#include "pin_interrupt.h"
#include "nfc.h"
#include "nfc_t2t_lib.h"

#include "ruuvi_endpoints.h"

#define DEAD_BEEF                   0xDEADBEEF       //!< Value used as error code on stack dump, can be used to identify stack location on stack unwind.
#define NON_CONNECTABLE_ADV_LED_PIN BSP_BOARD_LED_1  //!< Toggles when non-connectable advertisement is sent.
#define CONNECTED_LED_PIN           BSP_BOARD_LED_0  //!< Is on when device has connected.
#define CONNECTABLE_ADV_LED_PIN     BSP_BOARD_LED_0  //!< Is on when device is advertising connectable advertisements.

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
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}


static bool connectable = false;

/**@brief Function for handling Eddystone events.
 *
 * @param[in] evt Eddystone event to handle.
 */
static void on_es_evt(nrf_ble_es_evt_t evt)
{
    switch(evt)
    {
        case NRF_BLE_ES_EVT_ADVERTISEMENT_SENT:
            // non-connectable advertisement
        connectable = false;
        break;
        
        case NRF_BLE_ES_EVT_CONNECTABLE_ADV_STARTED:
            // connectable advertisement
        connectable = true;
        break;

        default:
        break;
    }
}

bool isConnectable(void)
{
    return connectable;
}

/**@brief Function for doing power management.
 * Turns green led on when device is connecteable.
 * Blink red led when not in sleep
 */
static void power_manage(void)
{
  if(!isConnectable()) { nrf_gpio_pin_set(LED_GREEN); }
  nrf_gpio_pin_set(LED_RED);
  uint32_t err_code = sd_app_evt_wait();
  //__WFE();
  APP_ERROR_CHECK(err_code);
  watchdog_feed();
  nrf_gpio_pin_clear(LED_RED);
  if(isConnectable()) nrf_gpio_pin_clear(LED_GREEN);
}





/**@brief Function for handling button events from app_button IRQ
 *
 */
ret_code_t button_press_handler(const ruuvi_standard_message_t message)
{ 
  nrf_ble_es_on_start_connectable_advertising();
  return NRF_SUCCESS;
}

/**
 *  Callback for NFC event
 */
static void nfc_detected_handler(void* data, uint16_t length)
{
  nrf_ble_es_on_start_connectable_advertising();
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
    app_sched_event_put (NULL, 0, nfc_detected_handler);
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

/**
 * @brief Function for application main entry.
 */
int main(void)
{
  uint32_t err_code = NRF_SUCCESS;

  // Initialize.
  err_code |= init_log(); 

  err_code |= init_ble(); 
  err_code |= bluetooth_configure_advertisement_type(BLE_GAP_ADV_TYPE_ADV_NONCONN_IND);

  err_code |= init_sensors();

  NRF_LOG_DEBUG("BLE init status: %d\r\n", err_code);

  //setup callback before initializing NFC.
  set_nfc_callback(app_nfc_callback);
  err_code |= init_nfc();


    // Start interrupts.
  err_code |= pin_interrupt_init();

    // Initialize button.
  err_code |= pin_interrupt_enable(BSP_BUTTON_0, NRF_GPIOTE_POLARITY_HITOLO, button_press_handler);

  APP_SCHED_INIT(SCHED_MAX_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE);
  APP_TIMER_APPSH_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, true);
  APP_ERROR_CHECK(err_code);

  nrf_ble_es_init(on_es_evt);



  NRF_LOG_INFO("Start!\r\n");
    // Enter main loop.
  for (;; )
  {
    app_sched_execute();
    if (NRF_LOG_PROCESS() == false)
    {
        power_manage();
    }
  }
}
