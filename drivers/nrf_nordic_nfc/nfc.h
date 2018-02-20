#ifndef NFC_H
#define NFC_H

#include <stdint.h>
#include "nfc_t2t_lib.h"
#include "nfc_ndef_msg.h"
#include "nfc_text_rec.h"
#include "sdk_errors.h"

#define MAX_REC_COUNT      4     /**< Maximum records count. */

/**
 * Initializes NFC with ID , address and data message.
 *
 * ID message is uniques_id in UTF-8 numbers string, i.e. "032523523"
 *
 * @return error  code from NFC init, 0 on success
 *
 */
ret_code_t nfc_init(uint8_t* data, uint32_t data_length);

/**
 * @brief Function for encoding the NFC message.
 */
void nfc_msg_encode(nfc_ndef_msg_desc_t* nfc_msg, uint8_t * p_buffer, uint32_t * p_len);

/**
 * @brief Function for creating a record.
 */
void id_record_add(nfc_ndef_msg_desc_t* nfc_msg);

/**
 * @brief Function for creating a record.
 */
void address_record_add(nfc_ndef_msg_desc_t* nfc_msg);

/**
 * @brief Function for creating a record.
 */
void version_record_add(nfc_ndef_msg_desc_t* nfc_msg);

/**
 * Update NFC payload with given data. Data is converted to hex and printed as a string.
 * https://infocenter.nordicsemi.com/index.jsp?topic=%2Fcom.nordic.infocenter.sdk5.v12.0.0%2Fnfc_ndef_format_dox.html
 */
void data_record_add(nfc_ndef_msg_desc_t* nfc_msg, uint8_t* data, uint32_t data_length);

/**
 * @brief Callback function for handling NFC events.
 */
void nfc_callback(void * p_context, nfc_t2t_event_t event, const uint8_t * p_data, size_t data_length);

/*
 * Set callback which will be called once NFC event is received. NULL to disable callback
 * Takes effect after NFC init
 */
typedef void(*nfc_callback_t)(void*, nfc_t2t_event_t, const uint8_t*, size_t);
void set_nfc_callback(nfc_callback_t callback);



#endif
