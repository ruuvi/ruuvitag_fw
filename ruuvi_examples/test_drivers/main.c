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
 * This file contains some tests to ensure the Ruuvitag drivers are working correct. The output is transmittet via
 * the J-Link RTT.
 */

#include <stdbool.h>
#include <stdint.h>
#include "nordic_common.h"
#include "softdevice_handler.h"
#include "bsp.h"
#include "app_timer.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "LIS2DH12.h"
#include "nrf_delay.h"

#include "nrf_drv_gpiote.h"

#define DEAD_BEEF                       0xDEADBEEF                        /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

#define APP_TIMER_PRESCALER             0                                 /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_OP_QUEUE_SIZE         4                                 /**< Size of timer operation queues. */

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

/**@brief Function for doing power management.
 */
static void power_manage(void)
{
    NRF_LOG_INFO("Managing power\r\n");
    //uint32_t err_code = sd_app_evt_wait();
   // APP_ERROR_CHECK(err_code);
    
}

/**
 * @brief Function for application main entry.
 */
int main(void)
{
    uint32_t err_code;
    int32_t testX, testY, testZ;
    LIS2DH12_Ret Lis2dh12RetVal;
    // Initialize.
    err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, false);
    err_code = bsp_init(BSP_INIT_LED, APP_TIMER_TICKS(100, APP_TIMER_PRESCALER), NULL);
    nrf_gpio_cfg_output(17);
    nrf_gpio_pin_clear(17);
    APP_ERROR_CHECK(err_code);
	

    NRF_LOG_INFO("LIS2DH12 init Start\r\n");

    Lis2dh12RetVal = LIS2DH12_init(LIS2DH12_POWER_LOW, LIS2DH12_SCALE2G, NULL);

    if (LIS2DH12_RET_OK == Lis2dh12RetVal)
    {
        //NRF_LOG_INFO("LIS2DH12 init Done\r\n");
    }
    else
    {
        NRF_LOG_ERROR("LIS2DH12 init Failed: Error Code: %d\r\n", (int32_t)Lis2dh12RetVal);
    }
    nrf_gpio_pin_set(17);
    NRF_LOG_INFO("LIS2DH12 init Done\r\n");

    // Enter main loop.
    for (;; )
    {
        //if (NRF_LOG_PROCESS() == false)
         NRF_LOG_INFO("Loopin'\r\n");
         nrf_gpio_pin_toggle(17);
         power_manage();
         nrf_delay_ms(100U);

         LIS2DH12_getALLmG(&testX, &testY, &testZ);
         NRF_LOG_INFO ("X-Axis: %d, Y-Axis: %d, Z-Axis: %d", testX, testY, testZ);

    }
}


/**
 * @}
 */
