#include "stdev.h"
#include "math.h"

void dsp_process_stdev(ringbuffer_t* values, const uint8_t parameter, const float next)
{
  //Only push latest sample to buffer
  ringbuffer_push(values, (void*)&next);
}

float dsp_read_stdev(ringbuffer_t* values, const uint8_t parameter)
{
  // Calculate mean
  float mean = 0.0f;
  //Store peeked values for variance
  float samples[parameter];
  for(size_t ii = 0; ii < parameter; ii ++)
  {
    float value;
    ringbuffer_peek_at(values, ii, &value);
    samples[ii] = value;
    mean += value;
  }
  mean/=parameter;

  // Calculate variance
  float variance = 0.0f;
  for(int ii = 0; ii < parameter; ii++)
  {
    float difference = samples[ii] - mean;
    variance += difference*difference;
  }

  variance /= parameter;

  return sqrt(variance);
}
