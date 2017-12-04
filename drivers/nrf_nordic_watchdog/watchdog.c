#include "watchdog.h"
#include "nrf_drv_wdt.h"
#include "nrf_error.h"

#define NRF_LOG_MODULE_NAME "WATCHDOG"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

nrf_drv_wdt_channel_id m_channel_id;

// Initialize watchdog with configuration defined in sdk_config (possible sdk_application_config)
// Call wdt_event_handler on wdt event
ret_code_t watchdog_init(watchdog_event_handler_t handler)
{
  if(NULL == handler) { return NRF_ERROR_INVALID_PARAM; }
  ret_code_t err_code = NRF_SUCCESS;
  nrf_drv_wdt_config_t config = NRF_DRV_WDT_DEAFULT_CONFIG;
  err_code |= nrf_drv_wdt_init(&config, handler);
  err_code |= nrf_drv_wdt_channel_alloc(&m_channel_id);
  NRF_LOG_INFO("Watchdog init, status %d\r\n", err_code);
  return err_code;
}

//Start watchdog
void watchdog_enable(void)
{
  nrf_drv_wdt_enable();
}

//resets watchdog counter
void watchdog_feed(void)
{
  nrf_drv_wdt_channel_feed(m_channel_id);
  NRF_LOG_DEBUG("Watchdog fed\r\n");
}

// Log error and reset.
void watchdog_default_handler(void)
{
  NRF_LOG_ERROR("Watchdog triggered\r\n");

    //NOTE: The max amount of time we can spend in WDT interrupt is two cycles of 32768[Hz] clock - after that, reset occurs
}
