#include "mam.h"

/** NRF LIBS **/
#include "nrf_error.h"

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

//XXX This is for hackathon and will be removed
void send_environmental_mam(void)
{
  const char seed[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ9ABCDEFGHIJKLMNOPQRSTUVWXYZ9ABCDEFGHIJKLMNOPQRSTUVWXYZ9";
  NRF_LOG_INFO("Seed:\r\n");
  NRF_LOG_INFO("%s\r\n",(uint32_t)seed);

  //ret_code_t err_code = NRF_SUCCESS;
  char message[30] = {0};
  char trytes[60] = {0};
/*  err_code |= bme280_set_mode(BME280_MODE_FORCED);
  nrf_delay_ms(15);//XX
  err_code |= bme280_set_mode(BME280_MODE_FORCED);
  nrf_delay_ms(15);
  int32_t  raw_t = bme280_get_temperature();
  uint32_t raw_p = bme280_get_pressure();
  uint32_t raw_h = bme280_get_humidity();
*/
  sprintf(message, "hello");
  NRF_LOG_INFO("ASCII message: %s\r\n", (uint32_t)message);
  toTrytes((void*)message, trytes, strlen(message));
  NRF_LOG_INFO("Tryte message: %s\r\n", (uint32_t)trytes);



  size_t start = MAM_START;
  size_t count = MAM_COUNT;
  size_t index = MAM_INDEX;
  size_t next_start = MAM_NEXT_START;
  size_t next_count = MAM_NEXT_COUNT;
  size_t security = MAM_SECURITY;
    
  //Returns dynamically allocated pointer. REMEMBER TO FREE
  char* result = (char*)mam_create(seed, trytes, start, count, index, next_start, next_count, security);
  //size_t result_len = strlen(result);
  //char* result = merkle_keys(seed, next_start, next_count, security);
  NRF_LOG_INFO("mam done\r\n");
  size_t result_length = strlen(result); 

  //char* root = strtok(NULL, "\n");
  //Could be done with pointer arithmetic too.

  ble_bulk_transfer_asynchronous(MAM, (void*)result, result_length);
  //err_code = ble_bulk_transfer_asynchronous(MAM, (void*)masked_payload, mam_length);
  //NRF_LOG_INFO("%d\r\n", result_length);

}

ret_code_t mam_handler(const ruuvi_standard_message_t message)
{
    // Send plaintext info that MAM creation has started.
    // TODO: send ACKNOWLEDGEMENT instead 
    char ascii[] = {'I', 'N', 'I', 'T', ' ', 'M', 'A', 'M'};
    ruuvi_standard_message_t reply = {.destination_endpoint = message.source_endpoint,
                                      .source_endpoint = TEMPERATURE,
                                      .type = ASCII,
                                      .payload = {0}};
    memcpy(&(reply.payload[0]), &(ascii[0]), sizeof(reply.payload)); 
    
  message_handler p_reply_handler = get_reply_handler();
  if(p_reply_handler) 
  {  
    NRF_LOG_INFO("Sending reply from MAM\r\n");
    p_reply_handler(reply);
  }
  ble_message_queue_process();  //XXX message queue should be processed in background
  send_environmental_mam();
  return NRF_SUCCESS;
}
