#ifndef LIS2DH12_ACCELERATION_HANDLER_H
#define LIS2DH12_ACCELERATION_HANDLER_H

#include "ruuvi_endpoints.h"
#include "nrf_error.h"

ret_code_t lis2dh12_acceleration_handler(const ruuvi_standard_message_t message);
ret_code_t lis2dh12_int1_handler(const ruuvi_standard_message_t message);
void lis2dh12_scheduler_event_handler(void *p_event_data, uint16_t event_size);
uint32_t get_exercise(void);

#endif
