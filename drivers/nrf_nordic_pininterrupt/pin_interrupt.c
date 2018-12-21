#include "pin_interrupt.h"
#include <stdbool.h>
#include "nrf.h"
#include "nrf_drv_gpiote.h"
#include "app_error.h"
#include "boards.h"

#include "ruuvi_endpoints.h"

#define NRF_LOG_MODULE_NAME "PIN_INTERRUPT"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

/**
 *  Enables GPIOTE which is required for pin interrupts
 *
 */
ret_code_t pin_interrupt_init()
{
  ret_code_t err_code = NRF_SUCCESS;
  err_code |= nrf_drv_gpiote_init();
  return err_code;
}

//Look-up table for event handlers
static message_handler pin_event_handlers[32] = {0};
static void in_pin_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
  //Call event handler with empty message TODO: invalid message add context?
  NRF_LOG_DEBUG("Handling pin event\r\n");
  ruuvi_standard_message_t message;
  message.payload[0] = pin;
  message.payload[1] = nrf_gpio_pin_read(pin);
  if (NULL != pin_event_handlers[pin]) { (pin_event_handlers[pin])(message);}
}
/**
 *  Enable interrput on pin. Pull-up is enabled on HITOLOW, pull-down is enabled on LOWTIHI
 *  Polarity can be defined:
 *  NRF_GPIOTE_POLARITY_LOTOHI
 *  NRF_GPIOTE_POLARITY_HITOLO
 *  NRF_GPIOTE_POLARITY_TOGGLE
 *
 *  Message handler is called with an empty message on event.
 */
ret_code_t pin_interrupt_enable(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t polarity, nrf_gpio_pin_pull_t pull, message_handler handler)
{
  NRF_LOG_INFO("Enabling pin interrupt\r\n");
  ret_code_t err_code = NRF_SUCCESS;
  nrf_drv_gpiote_in_config_t in_config = {
    .is_watcher = false,               \
    .hi_accuracy = false,              \
    .pull = pull,       \
    .sense = polarity                  \
  };
  pin_event_handlers[pin] = handler;
  err_code |= nrf_drv_gpiote_in_init(pin, &in_config, in_pin_handler);

  nrf_drv_gpiote_in_event_enable(pin, true);

  return err_code;
}
