#include "test_led.h"

#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "boards.h"

#define NRF_LOG_MODULE_NAME "LED_TEST"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

void test_led(void)
{
  NRF_LOG_INFO("Turning LEDs on for a second\r\n");
  NRF_LOG_FLUSH();
  nrf_delay_ms(10);
  nrf_gpio_pin_clear(LED_RED);
  nrf_gpio_pin_clear(LED_GREEN);
  nrf_delay_ms(1000);
  nrf_gpio_pin_set(LED_RED);
  nrf_gpio_pin_set(LED_GREEN);
  nrf_delay_ms(10);
}
