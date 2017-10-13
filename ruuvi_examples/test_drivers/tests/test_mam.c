#include "test_mam.h"
#include "libiota.h"
#include "asciiToTrytes.h"

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

#include "nrf_delay.h"
#define NRF_LOG_MODULE_NAME "MAM_TEST"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

void test_mam(void)
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
  char* root = strtok(NULL, "\n");
  //Could be done with pointer arithmetic too.
  size_t mam_length = strlen(masked_payload); 

  char chunk[19] = {0};  
  int ii = 0;
  for(ii = 0; (ii+1)*18 < mam_length; ii++)
  {
    for (int jj = 0; jj < 18; jj++)
    {
      chunk[jj] = masked_payload[ii*18+jj];
    }
    NRF_LOG_INFO("Chunk %d: %s\r\n", ii, (uint32_t)chunk);
    NRF_LOG_FLUSH();
    nrf_delay_ms(10);      
  }
  memset(chunk, 0, sizeof(chunk));
  for (int jj = 0; jj < mam_length%18; jj++)
  {
    chunk[jj] = masked_payload[ii*18+jj];
  }
  NRF_LOG_INFO("Chunk %d: %s\r\n", ii, (uint32_t)chunk);

  ii = 0; 
  for(ii = 0; (ii+1)*18 < MAM_ROOT_LENGTH; ii++)
  {
    for (int jj = 0; jj < 18; jj++)
    {
      chunk[jj] = root[ii*18+jj];
    }
    NRF_LOG_INFO("Chunk %d: %s\r\n", ii, (uint32_t)chunk);
    NRF_LOG_FLUSH();
    nrf_delay_ms(10);
  }
  memset(chunk, 0, sizeof(chunk));
  for (int jj = 0; jj < MAM_ROOT_LENGTH%18; jj++)
  {
    chunk[jj] = root[(ii * 18) + jj];
  }
  NRF_LOG_INFO("Chunk %d: %s\r\n", ii, (uint32_t)chunk);
  NRF_LOG_FLUSH();
  nrf_delay_ms(10);


  //Crashes, too little memory
  /*mam_start = millis();
  NRF_LOG_INFO("time start: %ld\r\n", mam_start);
  const char* parsed = mam_parse(masked_payload, root, index);
  mam_end = millis();
  NRF_LOG_INFO("time end: %ld\r\n", mam_start);
  NRF_LOG_INFO("time delta: %ld\r\n", mam_end - mam_start);
  NRF_LOG_INFO("Got MAM PARSE RESULT:\n%s\n", (uint32_t)parsed);
  */
  free(result);
}

/** For timing the MAM create **/
void test_mam_create_time(void)
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
  free(result);
}

void test_byte_tryte_conversion(void)
{
  NRF_LOG_INFO("Turning ascii message to trytes\r\n");
  uint8_t message[] = "IAMSOMEMESSAGE9HEARMEROARMYMESSAGETOTHEWORLDYOUHEATHEN";//Binary, not null-terminated
  char* trytes = calloc(2 * sizeof(message) + 1, sizeof(char));
  uint8_t* loopback = calloc(sizeof(message) + 1, sizeof(uint8_t)); //Reserve space for terminating null for printing
  uint32_t err_code = 0;
  err_code |= toTrytes(message, trytes, sizeof(message)-1); //C string initialization automatically appends null, do not encode the null.
  NRF_LOG_INFO("Trytes: %s\r\n", (uint32_t)trytes);
  err_code |= fromTrytes(trytes, loopback, 2 * (sizeof(message)- 1) ); //Terminating NULL is not included in message length
  NRF_LOG_INFO("Back conversion result %s, status %d\r\n", (uint32_t)loopback, err_code);
  free(trytes);
  free(loopback);
}
