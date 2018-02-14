/**
 * Handler for NFC events, allows registering callbacks from the NFC driver into application
 *
 * Usage: call nfc_init_handler() to register handler into NFC driver. 
 * Set callbacks, i.e. nfc_connected handler_set(my_handler)
 * Init NFC, i.e. nfc_init (or init_nfc if you're using Ruuvi Init driver)
 * 
 * Author: Otso Jousimaa <otso@ruuvi.com>
 * License: BSD-3
 */

#ifndef NRF_NFC_HANDLER_H
#define NRF_NFC_HANDLER_H

#include "sdk_errors.h"
#include "ruuvi_endpoints.h"

void nfc_connected_handler_set(message_handler handler);
void nfc_init_handler(void);

#endif