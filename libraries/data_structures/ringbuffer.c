#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "ringbuffer.h" 

//Debug logging
#define NRF_LOG_MODULE_NAME "RINGBUFFER"
//#define NRF_LOG_DEFAULT_LEVEL 4
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

// Based on https://jlmedina123.wordpress.com/2013/08/19/circular-buffer-queue/

void ringbuffer_init(ringbuffer_t *buffer, size_t  element_max, size_t element_size)
{
    buffer->element_max = element_max;
    buffer->element_size = element_size;
    buffer->start = 0;
    buffer->count = 0;
    buffer->element = malloc(element_size * element_max);
    NRF_LOG_INFO("Init ringbuffer, size of one element is %d\r\n", buffer->element_size);
}

void ringbuffer_uninit(ringbuffer_t *buffer)
{
  free(buffer->element);
  buffer->element = NULL;
}
 
int ringbuffer_full(ringbuffer_t *buffer)
{
    return (buffer->count == buffer->element_max);
}
 
int ringbuffer_empty(ringbuffer_t *buffer) 
{
    return (buffer->count == 0);
}

int ringbuffer_is_init(ringbuffer_t *buffer)
{
  return (NULL != buffer->element);
}

void ringbuffer_peek_at(ringbuffer_t* buffer, size_t index, void* element)
{
  //Calculate element position at X, relative to ringbuffer start
  size_t position = (buffer->start)+index;
  if(position >= buffer->element_max) { position = position - buffer->element_max; }
  NRF_LOG_DEBUG("Copying %d bytes to %d\r\n", buffer->element_size, (uint32_t)((buffer->element) + (position * buffer->element_size)));
  memcpy(element, (buffer->element) + (position * buffer->element_size), buffer->element_size);
}

//TODO: #define overflow behaviour.
void ringbuffer_push(ringbuffer_t* buffer, void* data)
{
  int index;
  index = buffer->start + buffer->count++;
  //Wraparound
  if (index >= buffer->element_max) 
  {
    index = index-(buffer->element_max);
  }
  void* target = buffer->element + (index * buffer->element_size);
  NRF_LOG_DEBUG("Buffer starts at %d, pushing %d bytes to address %d\r\n", (uint32_t)buffer->element, (uint32_t)buffer->element_size, (uint32_t)target);
  memcpy(target, data, buffer->element_size);
  //Buffer overflow? pop element (this is actually newest element / same as input due to overflow
  if(buffer->count > buffer->element_max){ ringbuffer_popqueue(buffer, data); }
}
 
 
void ringbuffer_popqueue(ringbuffer_t* buffer, void* element) 
{
  if (ringbuffer_empty(buffer)){ return; }

  /* FIFO implementation */
  void* source = buffer->element + ((buffer->start) * (buffer->element_size));
  memcpy(element, source, buffer->element_size);
  buffer->start++;
  buffer->count--;
  if (buffer->start == buffer->element_max) 
  {
       buffer->start = 0;
  }
}
 
void ringbuffer_popstack(ringbuffer_t *buffer, void* element) 
{
  int index;
  if (ringbuffer_empty(buffer))
  {
    return;
  } 
  /* LIFO implementation */
  index = buffer->start + buffer->count - 1;
  if (index >= buffer->element_max)
  {
    index = index - buffer->element_max;
  }      
  void* source = (buffer->element) + index * (buffer->element_size);
  buffer->count--;
  
  memcpy(element, source, buffer->element_size);
}

//Get max_elements
size_t ringbuffer_get_size(ringbuffer_t* buffer)
{
  return buffer->element_max;
}

//Get number of stored elements
size_t ringbuffer_get_count(ringbuffer_t* buffer)
{

  return buffer->count;
}

//Copy values XXX Copies popped elements too
void ringbuffer_copy_data(void* target, ringbuffer_t* source)
{
  void* start = source->element + (source->element_size * source->start);
  size_t first_block_size = source->element_size * (source->element_max - source->start);
  //Copy elements in order
  memcpy(target, start, first_block_size);
  memcpy(target + first_block_size, source->element, (source->element_size * source->element_max) - first_block_size);
}
