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

#include "bsp_nfc.h"
#include "bsp.h"

#define BTN_ACTION_SLEEP          BSP_BUTTON_ACTION_RELEASE    /**< Button action used to put the application into sleep mode. */

ret_code_t bsp_nfc_btn_init(uint32_t sleep_button)
{
    uint32_t err_code = bsp_event_to_button_action_assign(sleep_button,
                                                          BTN_ACTION_SLEEP,
                                                          BSP_EVENT_SLEEP);
    return err_code;
}

ret_code_t bsp_nfc_btn_deinit(uint32_t sleep_button)
{
    uint32_t err_code = bsp_event_to_button_action_assign(sleep_button,
                                                          BTN_ACTION_SLEEP,
                                                          BSP_EVENT_DEFAULT);
    return err_code;
}

ret_code_t bsp_nfc_sleep_mode_prepare(void)
{
    uint32_t err_code = bsp_wakeup_nfc_set();
    return err_code;
}
