#include "dsp.h"
#include "stdev.h"
#include "ruuvi_endpoints.h"
#include "ringbuffer.h"

//Debug logging
#define NRF_LOG_MODULE_NAME "DSP"
//#define NRF_LOG_DEFAULT_LEVEL 4
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

dsp_filter_t dsp_init(ruuvi_dsp_function_t type, uint8_t dsp_parameter)
{
  dsp_filter_t filter;
  memset(&filter, 0, sizeof(filter));
  switch(type)
  {
    case DSP_STDEV:
      filter.process = dsp_process_stdev;
      filter.read = dsp_read_stdev;
      filter.dsp_parameter = dsp_parameter;
      ringbuffer_init(&filter.z, dsp_parameter, sizeof(float));
      break;
    
    default:
      NRF_LOG_ERROR("Unknown filter type\r\n");
      break;
  }
  
  return filter;
}


int dsp_is_init(dsp_filter_t* filter)
{
  return ringbuffer_is_init(&(filter->z));
}

void dsp_uninit(dsp_filter_t* filter)
{
  ringbuffer_uninit(&(filter->z));
}
