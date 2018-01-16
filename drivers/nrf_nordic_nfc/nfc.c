#include "nfc.h"

#include <stdint.h>
#include "nfc_t2t_lib.h"
#include "nfc_ndef_msg.h"
#include "nfc_text_rec.h"
#include "boards.h"
#include "app_error.h"

#define NRF_LOG_MODULE_NAME "NFC"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

uint8_t m_ndef_msg_buf[256];


/**
 * @brief Callback function for handling NFC events.
 */
void nfc_callback(void * p_context, nfc_t2t_event_t event, const uint8_t * p_data, size_t data_length)
{
    (void)p_context;

    switch (event)
    {
        case NFC_T2T_EVENT_FIELD_ON:
            bsp_board_led_on(BSP_BOARD_LED_0); //TODO: Name LED GREEN / LED_RED etc or maybe configure in application config.h?
            NRF_LOG_INFO("NFC Field detected \r\n")
            break;
        case NFC_T2T_EVENT_FIELD_OFF:
            bsp_board_led_off(BSP_BOARD_LED_0);
            NRF_LOG_INFO("NFC Field lost \r\n")
            break;
        default:
            break;
    }
}

/**
 * @brief Function for encoding the initial message.
 */
void initial_msg_encode(uint8_t * p_buffer, uint32_t * p_len)
{
    NFC_NDEF_MSG_DEF(initial_msg, MAX_REC_COUNT);

    id_record_add(&NFC_NDEF_MSG(initial_msg));
    //url_record_add(&NFC_NDEF_MSG(initial_msg)); TODO: URL
    //app_record_add(&NFC_NDEF_MSG(initial_msg)); TODO: launch app

    /** @snippet [NFC text usage_2] */
    uint32_t err_code = nfc_ndef_msg_encode(&NFC_NDEF_MSG(initial_msg),
                                            p_buffer,
                                            p_len);
    APP_ERROR_CHECK(err_code);
    /** @snippet [NFC text usage_2] */
}

void nfc_init(void)
{
    uint32_t  len = sizeof(m_ndef_msg_buf);
    uint32_t  err_code;

    /* Set up NFC */
    err_code = nfc_t2t_setup(nfc_callback, NULL);
    APP_ERROR_CHECK(err_code);

    /* Encode welcome message */
    initial_msg_encode(m_ndef_msg_buf, &len);

    /* Set created message as the NFC payload */
    err_code = nfc_t2t_payload_set(m_ndef_msg_buf, len);
    APP_ERROR_CHECK(err_code);

    /* Start sensing NFC field */
    err_code = nfc_t2t_emulation_start();
    APP_ERROR_CHECK(err_code);
}

/**
 * @brief Function for creating a record.
 */
void id_record_add(nfc_ndef_msg_desc_t * p_ndef_msg_desc)
{
    /** @snippet [NFC text usage_1] */
    uint32_t             err_code;
    static const uint8_t en_payload[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8'}; //TODO: Add logic for Board ID. 
    static const uint8_t en_code[] = {'e', 'n'};

    NFC_NDEF_TEXT_RECORD_DESC_DEF(en_text_rec,
                                  UTF_8,
                                  en_code,
                                  sizeof(en_code),
                                  en_payload,
                                  sizeof(en_payload));
   /** @snippet [NFC text usage_1] */

    err_code = nfc_ndef_msg_record_add(p_ndef_msg_desc,
                                       &NFC_NDEF_TEXT_RECORD_DESC(en_text_rec));
    APP_ERROR_CHECK(err_code);
}