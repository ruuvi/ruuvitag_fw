#include "rtc.h"

#include <stdint.h>

#include "nrf_drv_rtc.h"
#include "nrf_drv_clock.h"

#define NRF_LOG_MODULE_NAME "RTC"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#ifndef APP_RTC_INSTANCE
#define APP_RTC_INSTANCE 2
#endif

const nrf_drv_rtc_t rtc = NRF_DRV_RTC_INSTANCE(APP_RTC_INSTANCE); /**< Declaring an instance of nrf_drv_rtc for RTC2. */

static void rtc_handler(nrf_drv_rtc_int_type_t int_type)
{
    if (int_type == NRF_DRV_RTC_INT_COMPARE0)
    {
      NRF_LOG_DEBUG("Compare event\r\n");
    }
    else if (int_type == NRF_DRV_RTC_INT_TICK)
    {
      NRF_LOG_DEBUG("Tick\r\n");
    }
}

uint32_t init_rtc(void)
{
  uint32_t err_code;

  //Initialize RTC instance
  nrf_drv_rtc_config_t config = NRF_DRV_RTC_DEFAULT_CONFIG;
  config.prescaler = 32;
  err_code = nrf_drv_rtc_init(&rtc, &config, rtc_handler);
  APP_ERROR_CHECK(err_code);

  //Enable tick event & interrupt - consumes horrendous amount of power on quick ticks
  //nrf_drv_rtc_tick_enable(&rtc,true);

  //Set compare channel to trigger interrupt after COMPARE_COUNTERTIME seconds
  //err_code = nrf_drv_rtc_cc_set(&rtc,0, COMPARE_COUNTERTIME * 8,true);
  //APP_ERROR_CHECK(err_code);

  //Power on RTC instance
  nrf_drv_rtc_enable(&rtc);
  
  return err_code;
}

uint32_t millis(void)
{
  uint64_t ms = nrf_drv_rtc_counter_get(&rtc);
  //Compensate tick roundoff
  ms*=32000;
  ms/=32768;
  return (uint32_t)ms;
}
