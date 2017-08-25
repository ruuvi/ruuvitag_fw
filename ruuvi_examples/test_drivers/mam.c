#include "mam.h"

/** NRF LIBS **/
#include "nrf_error.h"
#include "nrf_delay.h"//XXX

/** Ruuvi libs **/
#include "ruuvi_endpoints.h"
#include "bme280.h"
#include "ble_bulk_transfer.h"

/** IOTA lib **/
#include "iota/iota.h"
#include "iota/constants.h"
#include "libiota.h"
#include "asciiToTrytes.h"

#define NRF_LOG_MODULE_NAME "MAM"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

// PoC - will be deleted later on.
void send_environmental_mam(void)
{
  ret_code_t err_code = NRF_SUCCESS;
  const char seed[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ9ABCDEFGHIJKLMNOPQRSTUVWXYZ9ABCDEFGHIJKLMNOPQRSTUVWXYZ9";
  
  static int32_t raw_t  = 0;
  static uint32_t raw_p = 0;
  static uint32_t raw_h = 0;
  err_code |= bme280_set_mode(BME280_MODE_FORCED);
  nrf_delay_ms(15);//XX
  err_code |= bme280_set_mode(BME280_MODE_FORCED);
  nrf_delay_ms(15);
  raw_t = bme280_get_temperature();
  raw_p = bme280_get_pressure();
  raw_h = bme280_get_humidity();
  
  char message[100] = {0};
  char trytes[200] = {0};  
  sprintf(message, "{T:%ld%ld,P:%lu,H:%lu}", raw_t/100, raw_t%100, raw_p>>8, raw_h>>10); //Wrong decimals on negative values.
  NRF_LOG_DEBUG("ASCII message: %s\r\n", (uint32_t)message);
  toTrytes((void*)message, trytes, strlen(message));
  NRF_LOG_INFO("Tryte message: %s\r\n", (uint32_t)trytes);

  size_t start = MAM_START;
  size_t count = MAM_COUNT;
  size_t index = MAM_INDEX;
  size_t next_start = MAM_NEXT_START;
  size_t next_count = MAM_NEXT_COUNT;
  size_t security = MAM_SECURITY;
    
  NRF_LOG_INFO("Start MAM creation.\r\n");
  //Returns dynamically allocated pointer. REMEMBER TO FREE
  char* result = (char*)mam_create(seed, trytes, start, count, index, next_start, next_count, security);
  if(result == NULL) { NRF_LOG_ERROR("MAM ERROR \r\n"); }

  
  size_t result_len = strlen(result);
  //Zero-pad with tryte on uneven result length for conversion to binary. Overwrites terminating null.
  //if(result_len%2) { result[result_len] = '9'; result_len++; }
  //uint8_t* bytes  = calloc(result_len/2, sizeof(uint8_t));
  //err_code = fromTrytes(result, bytes, result_len);
  //NRF_LOG_INFO("Binary message status %d: %s\r\n", err_code, (uint32_t)bytes);
  NRF_LOG_INFO("MAM created, start TX of %d bytes.\r\n", result_len);
  err_code = ble_bulk_transfer_asynchronous(MAM, (void*)result, result_len);
  // -> Cannot be freed until data is sent, TX frees once tx is complete free(bytes);
  //free(result);
  NRF_LOG_INFO("TX queueing status %d.\r\n", err_code);
  
}
