#ifndef RUST_ALLOCATOR_H
#define RUST_ALLOCATOR_H

/**
 *  Memory allocator interface for rust library.
 *
 */

#include <stdlib.h>

void* unexec_malloc(size_t usize);

void unexec_free(void* ptr);

void* unexec_realloc(void* old_ptr, size_t new_size);

#endif
