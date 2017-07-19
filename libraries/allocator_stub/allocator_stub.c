#include "allocator_stub.h"
#include "app_uart.h"

void* unexec_malloc(size_t usize)
{
  return (void*) malloc(usize);
}

void unexec_free(void* ptr)
{
  free(ptr);
}

void* unexec_realloc(void* old_ptr, size_t new_size)
{
  return (void*) realloc(old_ptr, new_size);
}

void rust_begin_unwind(void* a, void* b, void*c) {
  printf("unwind!!\n");
}
