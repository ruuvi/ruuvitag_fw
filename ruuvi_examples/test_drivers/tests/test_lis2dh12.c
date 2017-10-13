#include "test_lis2dh12.h"

/** STDLIB **/
#include <stdbool.h>
#include <stdint.h>

/** nRF SDK **/
#include "nordic_common.h"
#include "bsp.h"
#include "app_timer_appsh.h"
#include "app_error.h"
#include "nrf_drv_gpiote.h"
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
    lis2dh12_set_scale(LIS2DH12_SCALE2G);
    nrf_delay_ms(100);
    test_read_samples();
    lis2dh12_set_scale(LIS2DH12_SCALE4G);
    nrf_delay_ms(100);
    test_read_samples();
    lis2dh12_set_scale(LIS2DH12_SCALE8G);
    nrf_delay_ms(100);
    test_read_samples();
    lis2dh12_set_scale(LIS2DH12_SCALE16G);
    nrf_delay_ms(100);
    test_read_samples();
    lis2dh12_set_scale(LIS2DH12_SCALE2G);
    nrf_delay_ms(100);
    test_read_samples();
}

static void test_resolution(void)
{
	NRF_LOG_INFO("Setup resolution \r\n");
	NRF_LOG_FLUSH();
	nrf_delay_ms(10);
    lis2dh12_set_resolution(LIS2DH12_RES8BIT);
    nrf_delay_ms(100);  
    test_read_samples();
    lis2dh12_set_resolution(LIS2DH12_RES10BIT);
    nrf_delay_ms(100);  
    test_read_samples();
    lis2dh12_set_resolution(LIS2DH12_RES12BIT);
    nrf_delay_ms(100);  
    test_read_samples();
}

static void test_fifo_modes(void)
{
  lis2dh12_set_sample_rate(LIS2DH12_RATE_100);
  //lis2dh12_set_resolution(LIS2DH12_RES10BIT);  
  lis2dh12_set_fifo_mode(LIS2DH12_MODE_STREAM);
  size_t count = 0;
  lis2dh12_get_fifo_sample_number(&count);
  NRF_LOG_INFO("FIFO had % d samples, should be ~0 \r\n", count);
  nrf_delay_ms(250);
  lis2dh12_get_fifo_sample_number(&count);
  NRF_LOG_INFO("FIFO had % d samples, should be ~25 \r\n", count);
  lis2dh12_sensor_buffer_t buffer[32];
  memset(buffer, 0, sizeof(buffer));
  lis2dh12_read_samples(buffer, count);
  lis2dh12_get_fifo_sample_number(&count);
  NRF_LOG_INFO("FIFO had % d samples, should be ~0 \r\n", count);

  for(int ii = 0; ii < 32; ii++)
  {
  	NRF_LOG_INFO("Got X:%d, Y:%d, Z:%d\r\n", (buffer[ii]).sensor.x, (buffer[ii]).sensor.y, (buffer[ii]).sensor.z);
  	nrf_delay_ms(1);
  }

}

void test_read_samples(void)
{
    lis2dh12_sensor_buffer_t buffer; 
    lis2dh12_read_samples(&buffer, 1);
    NRF_LOG_INFO("Got X:%d, Y:%d, Z:%d\r\n", buffer.sensor.x, buffer.sensor.y, buffer.sensor.z);
    NRF_LOG_FLUSH();
}

/**
 *  Requires interrupts enabled on ACC_INT_1
 */
static void test_watermark(void)
{
  NRF_LOG_INFO("Setting watermark - interrupt coming to ACC_1\r\n");
  lis2dh12_set_sample_rate(LIS2DH12_RATE_100);
  lis2dh12_set_fifo_watermark(25);
  lis2dh12_set_interrupts(LIS2DH12_I1_WTM);
  nrf_delay_ms(400);
}

static void test_sample_rates(void)
{
  
}

void test_lis2dh12(void)
{
	//Reset fifo - TODO proper reset
	lis2dh12_set_fifo_mode(LIS2DH12_MODE_BYPASS);
    //Start sampling
    lis2dh12_set_scale(LIS2DH12_SCALE2G);
    lis2dh12_set_resolution(LIS2DH12_RES10BIT);
    lis2dh12_set_sample_rate(LIS2DH12_RATE_100);
    nrf_delay_ms(100);

    //  Test reading samples
    test_read_samples();

    // Test scaling by ramping through 2G->16G
    test_scale();

    // Test resolution by ramping 8->12 bits
    test_resolution();

    // Test FiFo modes by starting stream and reading stream out
    test_fifo_modes();

    // Test sample rates - TODO would benefit from interrupt/other synch mechanism.
    test_sample_rates();

    //Test interrupt on watermark
    test_watermark();

    //Reset fifo - TODO proper reset
	lis2dh12_set_fifo_mode(LIS2DH12_MODE_BYPASS);

    //Go to sleep
    lis2dh12_set_scale(LIS2DH12_SCALE2G);
    lis2dh12_set_resolution(LIS2DH12_RES10BIT);
    lis2dh12_set_sample_rate(LIS2DH12_RATE_0);
}