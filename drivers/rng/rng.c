#include "rng.h"

#include <stdint.h>

#include "nrf_drv_rng.h"

uint32_t init_rng()
{
  return nrf_drv_rng_init(NULL);
}

uint32_t random()
{
  //RNG produces bytes
  uint8_t barray[4];
  
  // uint32_t pointer to start of byte array
  uint32_t* rand = (uint32_t*)&barray[0];
  nrf_drv_rng_block_rand(barray, 4);
  
  //return value of byte array as uint32_t
  return *rand;
}

uint8_t randoms_available()
{
  uint8_t available = 0;
  nrf_drv_rng_bytes_available(&available);
  return available;
}

