#ifndef PIN_INTERRUPT_HANDLER_H
#define PIN_INTERRUPT_HANDLER_H

#include "app_error.h"
#include "nrf_drv_gpiote.h"
#include "nrf_error.h"
#include "ruuvi_endpoints.h"

ret_code_t pin_interrupt_init();
ret_code_t pin_interrupt_enable(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t polarity, nrf_gpio_pin_pull_t pull, message_handler handler);

#endif