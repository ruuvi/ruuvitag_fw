#include "test_lis2dh12.h"

/** STDLIB **/
#include <stdbool.h>
#include <stdint.h>

/** nRF SDK **/
#include "nordic_common.h"
#include "bsp.h"
#include "app_timer_appsh.h"
#include "app_error.h"

/** Drivers **/
#include "lis2dh12.h"

/** Ruuvi libs **/
#include "ruuvi_endpoints.h"


#define NRF_LOG_MODULE_NAME "test_lis2dh12"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

static test_scale(void);
{

}

void test_lis2dh12(void);
{

// Test scaling by ramping through 2G->16G
  test_scale();

// Test resolution by ramping 8->12 bits

  test_resolution();

lis2dh12_ret_t lis2dh12_set_resolution(lis2dh12_resolution_t resolution);

/**
 *
 */
lis2dh12_ret_t lis2dh12_set_sample_rate(lis2dh12_sample_rate_t sample_rate);

/**
 *
 */
lis2dh12_ret_t lis2dh12_set_fifo_mode(lis2dh12_fifo_mode_t mode)

/**
 *
 */
lis2dh12_ret_t lis2dh12_read_samples(sensor_buffer_t* buffer, size_t count)

/**
 *
 */
lis2dh12_ret_t lis2dh12_get_fifo_sample_number(size_t* count);

/**
 *
 */
lis2dh12_ret_t lis2dh12_set_fifo_watermark(size_t* count);
}