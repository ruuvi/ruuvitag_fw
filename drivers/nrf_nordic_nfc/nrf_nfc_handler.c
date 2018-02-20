#include "nrf_nfc_handler.h"

#include "nfc.h"
#include "ruuvi_endpoints.h"

//Do not compile RAM-consuming buffers if they're not used
#if NFC_HAL_ENABLED

static message_handler nfc_connected_handler = NULL;

/**
 * Handler gets called when NFC event occurs
 */
void nfc_connected_handler_set(message_handler handler)
{
  nfc_connected_handler = handler;
}

static void nfc_event_handle(void * p_context, nfc_t2t_event_t event, const uint8_t * p_data, size_t data_length)
{
    (void)p_context;
    ruuvi_standard_message_t message;
    memset(&message, 0, sizeof(message));
    message.source_endpoint = NFC;

    switch (event)
    {
        case NFC_T2T_EVENT_FIELD_ON:
            if(NULL != nfc_connected_handler) { nfc_connected_handler(message); }
            break;
        case NFC_T2T_EVENT_FIELD_OFF:
            break;
        case NFC_T2T_EVENT_DATA_READ:
            break;
        default:
            break;
    }
}

/**
 * Sets up callback to handler
 */
void nfc_init_handler(void)
{
  set_nfc_callback(nfc_event_handle);
}

#endif