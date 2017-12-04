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

/** @file
 *
 * @defgroup nrf5_sdk_for_eddystone main.c
 * @{
 * @ingroup nrf5_sdk_for_eddystone
 * @brief Eddystone Beacon GATT Configuration Service + EID/eTLM sample application main file.
 *
 * This file contains the source code for an Eddystone
 * Beacon GATT Configuration Service + EID/eTLM sample application.
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

#include "event_handlers.h"

#define NRF_LOG_MODULE_NAME "MAIN"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#include "init.h"

#define DEAD_BEEF                   0xDEADBEEF       //!< Value used as error code on stack dump, can be used to identify stack location on stack unwind.

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


/**@brief Function for the application's SoftDevice event handler.
 *
 * @param[in] p_ble_evt SoftDevice event.
 */
static bool connected = false;
static void on_ble_evt(ble_evt_t * p_ble_evt)
{
    uint32_t err_code;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
            // Pairing not supported
            err_code = sd_ble_gap_sec_params_reply(p_ble_evt->evt.common_evt.conn_handle, 
                                                   BLE_GAP_SEC_STATUS_PAIRING_NOT_SUPP, 
                                                   NULL, 
                                                   NULL);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GATTS_EVT_SYS_ATTR_MISSING:
            // No system attributes have been stored.
            err_code = sd_ble_gatts_sys_attr_set(p_ble_evt->evt.common_evt.conn_handle, NULL, 0, 0);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GAP_EVT_CONNECTED:
            bsp_board_led_on(CONNECTED_LED_PIN);
            bsp_board_led_off(CONNECTABLE_ADV_LED_PIN);
            break;
        
        case BLE_GAP_EVT_DISCONNECTED:
            bsp_board_led_off(CONNECTED_LED_PIN);
            connected = false;
            break;
        
#if (NRF_SD_BLE_API_VERSION == 3)
        case BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST:
            err_code = sd_ble_gatts_exchange_mtu_reply(p_ble_evt->evt.gatts_evt.conn_handle, 
                                                       GATT_MTU_SIZE_DEFAULT);
            APP_ERROR_CHECK(err_code);
            break;
#endif
        
        default:
            // No implementation needed.
            break;
    }
}


/**@brief Function for dispatching a SoftDevice event to all modules with a SoftDevice
 *        event handler.
 *
 * @details This function is called from the SoftDevice event interrupt handler after a
 *          SoftDevice event has been received.
 *
 * @param[in] p_ble_evt  SoftDevice event.
 */
static void ble_evt_dispatch(ble_evt_t * p_ble_evt)
{
    ble_conn_params_on_ble_evt(p_ble_evt);
    on_ble_evt(p_ble_evt);
    nrf_ble_es_on_ble_evt(p_ble_evt);
    ble_advertising_on_ble_evt(p_ble_evt);
}




/**@brief Function for doing power management.
 *        Turns green led on when device exits sleep
 */
static void power_manage(void)
{
    if(!connected){ bsp_indication_set(BSP_INDICATE_IDLE); }
    bsp_board_led_off(NON_CONNECTABLE_ADV_LED_PIN);
    uint32_t err_code = sd_app_evt_wait();
    APP_ERROR_CHECK(err_code);
    bsp_board_led_on(NON_CONNECTABLE_ADV_LED_PIN);
}





/**@brief Function for handling button events from app_button IRQ
 *
 * @param[in] pin_no        Pin of the button for which an event has occured
 * @param[in] button_action Press or Release
 */
static void button_evt_handler(uint8_t pin_no, uint8_t button_action)
{
    if (button_action == APP_BUTTON_PUSH && pin_no == BUTTON_1)
    {
        connected = true;    
        nrf_ble_es_on_start_connectable_advertising();
    }
}


/**
 * @brief Function for initializing the registation button
 *
 * @retval Values returned by @ref app_button_init
 * @retval Values returned by @ref app_button_enable
 */
static void button_init(void)
{
    ret_code_t              err_code;
    const uint8_t           buttons_cnt  = 1;
    static app_button_cfg_t buttons_cfgs =
    {
        .pin_no         = BUTTON_REGISTRATION,
        .active_state   = APP_BUTTON_ACTIVE_LOW,
        .pull_cfg       = NRF_GPIO_PIN_PULLUP,
        .button_handler = button_evt_handler
    };

    err_code = app_button_init(&buttons_cfgs, buttons_cnt, APP_TIMER_TICKS(100, APP_TIMER_PRESCALER));
    APP_ERROR_CHECK(err_code);
    
    err_code = app_button_enable();
    APP_ERROR_CHECK(err_code);
}

/**
 *  @brief pull CS of sensors up to keep them powered off
 */
static void gpio_init()
{
    nrf_gpio_cfg_output	(SPIM0_SS_HUMI_PIN);
    nrf_gpio_pin_set(SPIM0_SS_HUMI_PIN);
    nrf_gpio_cfg_output	(SPIM0_SS_ACC_PIN);
    nrf_gpio_pin_set(SPIM0_SS_ACC_PIN);
}


/**
 * @brief Function for application main entry.
 */
int main(void)
{
    uint32_t err_code;

    // Initialize.
    init_log(); //TODO: Check for errors
    
    err_code = init_ble(); //TODO: Check for errors
    NRF_LOG_INFO("BLE init status: %d\r\n", err_code);
    nrf_delay_ms(10);
    
    // Subscribe for BLE events.
    err_code = softdevice_ble_evt_handler_set(ble_evt_dispatch);
    NRF_LOG_INFO("Softdevice bluetooth handler setup status: %d\r\n", err_code);
    nrf_delay_ms(10);
    
    // Subscribe for system events.
    err_code = softdevice_sys_evt_handler_set(sys_evt_dispatch);
    NRF_LOG_INFO("Softdevice system handler setup status: %d\r\n", err_code);
    nrf_delay_ms(10);

    err_code = bsp_init(BSP_INIT_LED, APP_TIMER_TICKS(100, APP_TIMER_PRESCALER), NULL);
    NRF_LOG_INFO("BSP setup status: %d\r\n", err_code);
    nrf_delay_ms(10);

    button_init();
    gpio_init();
    
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


/**
 * @}
 */
