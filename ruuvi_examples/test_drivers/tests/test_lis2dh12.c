#include "test_lis2dh12.h"

/** STDLIB **/
#include <stdbool.h>
#include <stdint.h>

/** nRF SDK **/
#include "nordic_common.h"
#include "bsp.h"
#include "app_timer_appsh.h"
#include "app_error.h"
#include "nrf_delay.h"

/** Drivers **/
#include "lis2dh12.h"

/** Ruuvi libs **/
#include "ruuvi_endpoints.h"


#define NRF_LOG_MODULE_NAME "test_lis2dh12"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

static void test_scale(void)
{

}

static void test_resolution(void)
{

}

static void test_fifo_modes(void)
{

}

static void test_fifo_depth_reading(void)
{

}

static void test_read_samples(void)
{
  lis2dh12_sensor_buffer_t buffer;
  lis2dh12_read_samples(&buffer, 1);
  NRF_LOG_INFO("Got X:%d, Y:%d, Z:%d\r\n", buffer.sensor.x, buffer.sensor.y, buffer.sensor.z);
}

static void test_watermark(void)
{

}

static void test_sample_rates(void)
{

}

void test_lis2dh12(void)
{
  //Start sampling
  lis2dh12_set_sample_rate(LIS2DH12_RATE_100);

  //  Test reading samples
  nrf_delay_ms(11);
  test_read_samples();

  // Test scaling by ramping through 2G->16G
  test_scale();

  // Test resolution by ramping 8->12 bits
  test_resolution();

  // Test FiFo modes by starting stream
  test_fifo_modes();

  // Test reading nbr of samples in FIFOs
  test_fifo_depth_reading();

  // Test sample rates - TODO requires interrupt/other synch mechanism.
  test_sample_rates();

  //Test interrupt on watermark
  test_watermark();
}