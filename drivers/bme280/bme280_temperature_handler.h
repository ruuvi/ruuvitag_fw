#ifndef BME280_TEMPERATURE_HANDLER_H
#define BME280_TEMPERATURE_HANDLER_H
#include "ruuvi_endpoints.h"
#include "nrf_error.h"
ret_code_t bme280_temperature_handler(const ruuvi_standard_message_t message);
#endif
