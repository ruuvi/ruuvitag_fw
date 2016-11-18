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

#include <stdbool.h>
#include <stdint.h>
#include "ble_advdata.h"
#include "nordic_common.h"
#include "softdevice_handler.h"
#include "bsp.h"
#include "app_timer.h"
//#include "base91.h"
#include "eddystone.h"

#define NRF_LOG_MODULE_NAME "APP"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#define swap_u32(num) ((num>>24)&0xff) | ((num<<8)&0xff0000) | ((num>>8)&0xff00) | ((num<<24)&0xff000000);
#define float2fix(a) ((int)((a)*256.0))   //Convert float to fix. a is a float

/**@brief Function for doing power management.
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
    char url[] = {'r', 'u', 'u', '.', 'v', 'i'};
    eddystone_init();
    eddystone_advertise_url(url, 6);
    NRF_LOG_INFO("Eddystone Start!\r\n");

    // Enter main loop.
    for (;; )
    {
            power_manage();
    }

}


/**
 * @}
 */
