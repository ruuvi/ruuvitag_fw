#ifndef DSP_H
#define DSP_H
#include <stdlib.h>
#include <math.h>

#include "ringbuffer.h"

/** DSP functions. Process: handles next sample. Does not necessarily calculate new state (i.e. FIR only cycles values) **/
/** ringbuffer: previous values, uint8_t DSP parameter (i.e. dsp_filter_t.parameter, float: new value **/
typedef void(*dsp_process)(ringbuffer_t* const, const uint8_t, const float);

// Read returns current value, Calculates new state if necessary
// float* values, uint8_t DSP parameter
typedef float(*dsp_read)(ringbuffer_t*, uint8_t);

typedef struct{
  ringbuffer_t z;
  uint8_t dsp_parameter;
  dsp_process process;
  dsp_read    read;
}dsp_filter_t;

/**
 * Initialises filter of given type. 
 * Return initialized filter
 **/
dsp_filter_t dsp_init(uint8_t type, uint8_t dsp_parameter);

int dsp_is_init();

/**
 *  Releases resources allocated for the DSP filter
 */
void dsp_uninit(dsp_filter_t* filter);

#endif
