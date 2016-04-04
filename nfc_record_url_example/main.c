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
 * @defgroup nfc_url_record_example_main main.c
 * @{
 * @ingroup nfc_url_record_example
 * @brief NFC URL record example application main file.
 *
 */
#include <stdint.h>
#include "nfc_t2t_lib.h"
#include "nfc_uri_msg.h"
#include "boards.h"
#include "app_error.h"

static const char url[] =
    {'n', 'o', 'r', 'd', 'i', 'c', 's', 'e', 'm', 'i', '.', 'c', 'o', 'm'}; //URL "nordicsemi.com"

uint8_t ndef_msg_buf[256];

/**
 * @brief Callback function for handling NFC events.
 */
void nfc_callback(void *context, NfcEvent event, const char *data, size_t dataLength)
{
    (void)context;

    switch (event)
    {
        case NFC_EVENT_FIELD_ON:
            LEDS_ON(BSP_LED_0_MASK);
            break;
        case NFC_EVENT_FIELD_OFF:
            LEDS_OFF(BSP_LED_0_MASK);
            break;
        default:
            break;
    }
}

/**
 * @brief Function for application main entry.
 */
int main(void)
{
    uint32_t  err_code;
    NfcRetval ret_val;

    /* Configure LED-pins as outputs */
    LEDS_CONFIGURE(BSP_LED_0_MASK);
    LEDS_OFF(BSP_LED_0_MASK);

    /* Set up NFC */
    ret_val = nfcSetup(nfc_callback, NULL);
    if (ret_val != NFC_RETVAL_OK)
    {
        APP_ERROR_CHECK((uint32_t) ret_val);
    }

    /* Provide information about available buffer size to encoding function */
    uint32_t len = sizeof(ndef_msg_buf);

    /* Encode URI message into buffer */
    err_code = nfc_uri_msg_encode( NFC_URI_HTTP_WWW,
                                   (uint8_t *) url,
                                   sizeof(url),
                                   ndef_msg_buf,
                                   &len);

    APP_ERROR_CHECK(err_code);

    /* Set created message as the NFC payload */
    ret_val = nfcSetPayload( (char*)ndef_msg_buf, len);
    if (ret_val != NFC_RETVAL_OK)
    {
        APP_ERROR_CHECK((uint32_t) ret_val);
    }

    /* Start sensing NFC field */
    ret_val = nfcStartEmulation();
    if (ret_val != NFC_RETVAL_OK)
    {
        APP_ERROR_CHECK((uint32_t) ret_val);
    }

    while(1){}
}

/** @} */
