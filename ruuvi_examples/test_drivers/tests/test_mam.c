#include "test_mam.h"
#include "libiota.h"

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

#include "nrf_delay.h"
#define NRF_LOG_MODULE_NAME "MAM_TEST"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#define MAM_LENGTH 2656 //XXX MAM has variable length
#define ROOT_LENGTH 81

void test_mam(void)
{
  const char seed[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ9ABCDEFGHIJKLMNOPQRSTUVWXYZ9ABCDEFGHIJKLMNOPQRSTUVWXYZ9";
  NRF_LOG_INFO("Seed:\r\n");
  NRF_LOG_INFO("%s\r\n",(uint32_t)seed);

  char message[] = "IAMSOMEMESSAGE9HEARMEROARMYMESSAGETOTHEWORLDYOUHEATHEN";
  //char message[54] = {0};
  //sprintf(message, "temperature: %ld, pressure: %u, humidity: %u", raw_t/100, (unsigned int)raw_p>>8, (unsigned int)raw_h>>10); //Wrong decimals on negative values.
  NRF_LOG_INFO("Message: %s\r\n",(uint32_t)message);
  size_t start = 1;
  size_t count = 2;
  size_t index = 1;
  size_t next_start = start + count;
  size_t next_count = 1;
  size_t security = 1;
    
  //Returns dynamically allocated pointer. REMEMBER TO FREE
  char* result = (char*)mam_create(seed, message, start, count, index, next_start, next_count, security);
  //char* result = merkle_keys(seed, next_start, next_count, security);
  NRF_LOG_INFO("mam done\r\n");
  NRF_LOG_FLUSH();
  nrf_delay_ms(10);

  NRF_LOG_INFO("splitting: \r\n");
  char* masked_payload = strtok((char * restrict)result, "\n");
  char* root = strtok(NULL, "\n");

  char chunk[19] = {0};  
  int ii = 0;
  for(ii = 0; (ii+1)*18 < MAM_LENGTH; ii++)
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
  for (int jj = 0; jj < MAM_LENGTH%18; jj++)
  {
    chunk[jj] = masked_payload[ii*18+jj];
  }
  NRF_LOG_INFO("Chunk %d: %s\r\n", ii, (uint32_t)chunk);

  ii = 0; 
  for(ii = 0; (ii+1)*18 < ROOT_LENGTH; ii++)
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
  for (int jj = 0; jj < ROOT_LENGTH%18; jj++)
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
  size_t start = 1;
  size_t count = 2;
  size_t index = 1;
  size_t next_start = start + count;
  size_t next_count = 1;
  size_t security = 1;
    
  //Returns dynamically allocated pointer. REMEMBER TO FREE
  char* result = (char*)mam_create(seed, message, start, count, index, next_start, next_count, security);
  free(result);
}
