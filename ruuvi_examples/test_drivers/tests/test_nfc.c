#include "test_nfc.h"
#include "nfc.h"

#define NRF_LOG_MODULE_NAME "test_lis2dh12"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

void test_nfc(void)
{

  //TODO: Test binary data setup
  //TODO: Setup limits for data, test error reporting from NFC.
  nfc_init(NULL, 0);
  NRF_LOG_INFO("Waiting for NFC field\r\n");
  sd_app_evt_wait();

}


