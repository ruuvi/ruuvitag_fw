#include "allocator_stub.h"
#include "app_uart.h"
#include "nrf_delay.h"

void print_stack_pointer() 
{
  uint32_t * ptr = NULL;
  printf("Stack: %d ", (int)((uint32_t)&ptr) - 0x20002128);
}

void* unexec_malloc(size_t usize)
{
  void* ptr = (void*) malloc(usize);
  printf("Malloc %d bytes, address %d ", usize, (int)((uint32_t)ptr) - 0x20002128);
    nrf_delay_ms(10);
  print_stack_pointer();
  printf("\n");
    nrf_delay_ms(10);
  return (void*) ptr;
}

void unexec_free(void* ptr)
{
  printf("Free: ");
  nrf_delay_ms(10);
  free(ptr);
  print_stack_pointer();
  printf("\n");
}

void* unexec_realloc(void* ptr, size_t new_size)
{
  printf("Realloc address %d ", (int)((uint32_t)ptr) - 0x20002128);
  nrf_delay_ms(10);
  char* new = (char*)realloc(ptr, new_size);
  //free(ptr);
  
  printf( "to %d, size %d, end %d\r\n", (int)((uint32_t)new) - 0x20002128, new_size,(int)((uint32_t)&(new[new_size])) - 0x20002128);
    nrf_delay_ms(10);
  return (void*)new;
}

void rust_begin_unwind(void* args, char** file_slice, uint32_t line) 
{
  printf("unwind!\r\n");
  //printf("unwind file %s\n", file_slice[0]);
  printf("unwind line %d\n", (int)line);
  print_stack_pointer();
  nrf_delay_ms(10);
}
