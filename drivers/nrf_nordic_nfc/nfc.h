#ifndef NFC_H
#define NFC_H

#include <stdint.h>
#include "nfc_t2t_lib.h"
#include "nfc_ndef_msg.h"
#include "nfc_text_rec.h"

#define MAX_REC_COUNT      3     /**< Maximum records count. */

/**
 * Initializes NFC with ID message.
 *
 * ID message is uniques_id in UTF-8 numbers string, i.e. "032523523"
 *
 * w@return error  code from NFC init, 0 on success
 *
 */
void init_nfc(void);

/**
 * @brief Function for encoding the initial message.
 */
void initial_msg_encode(uint8_t * p_buffer, uint32_t * p_len);

/**
 * @brief Creates UTF-8 string record of device ID.
 *
 * @param p_ndef_mmmmmsg_desc pointer to array containing text records.
 */
void id_record_add(nfc_ndef_msg_desc_t * p_ndef_msg_desc);

/**
 * @brief Callback function for handling NFC events.
 */
void nfc_callback(void * p_context, nfc_t2t_event_t event, const uint8_t * p_data, size_t data_length);

#endif