#include "rust_allocator.h"
#include <stdint.h>

#define NRF_LOG_MODULE_NAME "RUST_ALLOC"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

void* unexec_malloc(size_t usize)
{
  NRF_LOG_DEBUG("Allocating\r\n");
  void* ptr = (void*) malloc(usize);
  return (void*) ptr;
}

void unexec_free(void* ptr)
{
  NRF_LOG_DEBUG("Freeing\r\n");
  free(ptr);
}

void* unexec_realloc(void* ptr, size_t new_size)
{
  NRF_LOG_DEBUG("Reallocating\r\n");
  return (void*)realloc(ptr, new_size);
}

void rust_begin_unwind(void* args, char** file_slice, uint32_t line) 
{
  NRF_LOG_INFO("Rust stack unwind. Excessive memory usage? Invalid parameters to Rust?\r\n");
  //Add your own crash handler here
}
