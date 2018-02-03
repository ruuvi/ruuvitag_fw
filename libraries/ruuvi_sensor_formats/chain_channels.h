#ifndef CHAIN_CHANNELS_H
#define CHAIN_CHANNELS_H

#define NUM_CHAIN_CHANNELS 16
#define ENDPOINT_CHAIN_OFFSET 0x50

#include "ruuvi_endpoints.h"

ret_code_t chain_handler(const ruuvi_standard_message_t message);

//Initializes application timers, required for transmitting data
ret_code_t chain_handler_init(void);

#endif
