#ifndef STDEV_H
#define STDEV_H

#include "dsp.h"

void dsp_process_stdev(ringbuffer_t* values, const uint8_t parameter, const float next);
float dsp_read_stdev(ringbuffer_t* values, const uint8_t parameter);

#endif
