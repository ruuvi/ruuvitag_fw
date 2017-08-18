#include "test_rng.h"
#include <stdint.h>
#include "rng.h"
#define NRF_LOG_MODULE_NAME "RNG_TEST"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

void test_rng(void)
{
  NRF_LOG_INFO("Making a few coordinates, check correlation\r\n");
  for(int ii = 0; ii < 100; ii++)
  {
    //app_sched_execute(); There should not be any other task running while testing
    NRF_LOG_INFO(";%d;%d;\r\n", random(), random());
    NRF_LOG_FLUSH();
  }
}
