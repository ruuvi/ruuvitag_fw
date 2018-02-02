#include "test_rtc.h"
#include <stdint.h>

#include "rtc.h"

#include "nrf_delay.h"
#define NRF_LOG_MODULE_NAME "RTC_TEST"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

void test_rtc(void)
{
  NRF_LOG_INFO("Running RTC test. Waiting for ~30 seconds.");

  uint32_t test_start = millis();

  while(millis()-test_start < 30000)
  {
    NRF_LOG_INFO("RTC is %d\r\n", millis());
    nrf_delay_ms(1000);
  }
  NRF_LOG_INFO("Test complete.");
}
