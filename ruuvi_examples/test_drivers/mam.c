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

void send_environmental_mam(void)
{
  const char seed[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ9ABCDEFGHIJKLMNOPQRSTUVWXYZ9ABCDEFGHIJKLMNOPQRSTUVWXYZ9";
  NRF_LOG_INFO("Seed:\r\n");
  NRF_LOG_INFO("%s\r\n",(uint32_t)seed);

  char message[] = "IAMSOMEMESSAGE9HEARMEROARMYMESSAGETOTHEWORLDYOUHEATHEN";
  //char message[54] = {0};
  //sprintf(message, "temperature: %ld, pressure: %u, humidity: %u", raw_t/100, (unsigned int)raw_p>>8, (unsigned int)raw_h>>10); //Wrong decimals on negative values.
  NRF_LOG_INFO("Message: %s\r\n",(uint32_t)message);

  size_t start = MAM_START;
  size_t count = MAM_COUNT;
  size_t index = MAM_INDEX;
  size_t next_start = MAM_NEXT_START;
  size_t next_count = MAM_NEXT_COUNT;
  size_t security = MAM_SECURITY;
    
  //Returns dynamically allocated pointer. REMEMBER TO FREE
  char* result = (char*)mam_create(seed, message, start, count, index, next_start, next_count, security);
  //char* result = merkle_keys(seed, next_start, next_count, security);
  NRF_LOG_INFO("mam done\r\n");
  NRF_LOG_FLUSH();
  nrf_delay_ms(10);

  NRF_LOG_INFO("splitting: \r\n");
  char* masked_payload = strtok((char * restrict)result, "\n");
  //char* root = strtok(NULL, "\n");
  //Could be done with pointer arithmetic too.
  size_t mam_length = strlen(masked_payload); 
  /*
  char chunk[19] = {0};  
  int ii = 0;
  for(ii = 0; (ii+1)*18 < mam_length; ii++)
  {
    for (int jj = 0; jj < 18; jj++)
    {
      chunk[jj] = masked_payload[ii*18+jj];
    }
    NRF_LOG_DEBUG("Chunk %d: %s\r\n", ii, (uint32_t)chunk);

    //NRF_LOG_FLUSH();
    //nrf_delay_ms(10);      
  }
*/
  NRF_LOG_INFO("%d\r\n", mam_length);
  free(result);
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
  NRF_LOG_INFO("1 MAM\r\n");
  send_environmental_mam();
  NRF_LOG_INFO("2 MAM\r\n");
  send_environmental_mam();
  NRF_LOG_INFO("3 MAM\r\n");
  send_environmental_mam();
  NRF_LOG_INFO("4 MAM\r\n");
  send_environmental_mam();
  return NRF_SUCCESS;
}
