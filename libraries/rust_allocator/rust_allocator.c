#include "rust_allocator.h"
#include <stdint.h>
void* unexec_malloc(size_t usize)
{

  void* ptr = (void*) malloc(usize);
  return (void*) ptr;
}

void unexec_free(void* ptr)
{
  free(ptr);
}

void* unexec_realloc(void* ptr, size_t new_size)
{
  return (void*)realloc(ptr, new_size);
}

void rust_begin_unwind(void* args, char** file_slice, uint32_t line) 
{
  //Add your own crash handler here
}
