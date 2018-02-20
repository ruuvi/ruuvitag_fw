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
#include "rtc.h"
#include "pin_interrupt.h"

/** Ruuvi libs **/
#include "ruuvi_endpoints.h"


#define NRF_LOG_MODULE_NAME "test_lis2dh12"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

//Allowed sample rates
uint8_t samplerates[] = { LIS2DH12_RATE_0,
                          LIS2DH12_RATE_1,
                          LIS2DH12_RATE_10,
                          LIS2DH12_RATE_25,
                          LIS2DH12_RATE_50,
                          LIS2DH12_RATE_100,
                          LIS2DH12_RATE_200,
                          LIS2DH12_RATE_400};

// time to take 8 samples + some margin. First should timeout.
uint64_t sampling_timeouts[] = { 10000, 10000, 1000, 500, 250, 100, 50, 25};
static bool interrupted1 = false;
static bool interrupted2 = false;


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
  NRF_LOG_INFO("Setting watermark - waiting for interrupt to ACC_1\r\n");
  lis2dh12_set_sample_rate(LIS2DH12_RATE_100);
  lis2dh12_set_fifo_watermark(25);
  lis2dh12_set_interrupts(LIS2DH12_I1_WTM, 1); // Only I1 has WTM
  interrupted1 = false;
  lis2dh12_set_fifo_mode(LIS2DH12_MODE_STREAM);
  while(!interrupted1);
  NRF_LOG_INFO("Watermark test pass\r\n");
}

/** 
 *  Handle interrupt from lis2dh12
 *  Never do long actions, such as sensor reads in interrupt context.
 *  Using peripherals in interrupt is also risky, as peripherals might require interrupts for their function.
 *
 *  @param message Ruuvi message, with source, destination, type and 8 byte payload. Ignore for now. 
 **/
static ret_code_t lis2dh12_int1_handler(const ruuvi_standard_message_t message)
{
  NRF_LOG_DEBUG("Accelerometer interrupt to pin 1\r\n");
  interrupted1 = true;
  return NRF_SUCCESS;
}

static ret_code_t lis2dh12_int2_handler(const ruuvi_standard_message_t message)
{
  NRF_LOG_INFO("Accelerometer interrupt to pin 2\r\n");
  interrupted2 = true;
  return NRF_SUCCESS;
}

static void test_sample_rates(void)
{
  NRF_LOG_INFO("Start sample rate test\r\n");
  size_t count = 0; // nuber of samples in buffer
  lis2dh12_sensor_buffer_t buffer[32]; // buffer to read samples into
  
  //Set watermark to 8 samples to get test done in reasonable time
  lis2dh12_set_fifo_watermark(8);
  //Loop through allowed values
  for(uint8_t ii = 0; ii < sizeof(samplerates); ii++){
    //Store start
    uint64_t start = millis();
    //Setup sample rate
    lis2dh12_set_sample_rate(samplerates[ii]);
    //Clear FIFO
    lis2dh12_get_fifo_sample_number(&count);
    lis2dh12_read_samples(buffer, count);

    //reset interrupt status
    interrupted1 = false;

    //Wait for interrupt
    while(!interrupted1 && ((millis() - start) < sampling_timeouts[ii]));

    //Store time of interrupt
    uint64_t stop = millis();

    //Check we're in range
    uint64_t time = stop-start;
    if(sampling_timeouts[ii] <= time || sampling_timeouts[ii]/2 > time) {NRF_LOG_ERROR("Sample rate nbr %d took %d ms to read 8 samples\r\n", ii, time);}

  }
  NRF_LOG_INFO("Sample rate test complete, above log should have one error at sample rate 0\r\n");
}

/*
static void test_activity_detection(void)
{
  //Configure activity interrupt
  //Enable high-pass for Interrupt function 2
  //CTRLREG2 = 0x02
  lis2dh12_set_highpass(LIS2DH12_HPIS2_MASK);
    
  //Enable interrupt 2 on X-Y-Z HI
  //INT2_CFG = 0x2A
  lis2dh12_set_interrupt_configuration(0x2A, 2);    
  //Interrupt on 64 mg+ (highpassed, +/-)
  //INT2_THS= 0x04 // 4 LSB = 64 mg @2G scale
   lis2dh12_set_threshold(0x04, 2);

  lis2dh12_set_scale(LIS2DH12_SCALE2G);
  lis2dh12_set_resolution(LIS2DH12_RES10BIT);
  lis2dh12_set_sample_rate(LIS2DH12_RATE_100);
    
  //Enable Interrupt function 2 on LIS interrupt pin 2 (stays high for 1/ODR)
  lis2dh12_set_interrupts(LIS2DH12_I2C_INT2_MASK, 2);
  NRF_LOG_INFO("Testing activity detection - please give your tag a knock\r\n");
  while(!interrupted2);
  NRF_LOG_INFO("Activity detected\r\n");  
}
*/
void test_lis2dh12(void)
{
  // Enable LOTOHI interrupt on nRF52 -- note: this uses local handler, not the one defined in acceleration_handler.
  // TODO: Test the acceleration_handler logic?
  pin_interrupt_enable(INT_ACC1_PIN, NRF_GPIOTE_POLARITY_LOTOHI, lis2dh12_int1_handler);
  pin_interrupt_enable(INT_ACC2_PIN, NRF_GPIOTE_POLARITY_LOTOHI, lis2dh12_int2_handler);
  
  //Reset lis
  //Clear memory
  lis2dh12_reset();
  //Wait for reboot
  nrf_delay_ms(10);
  lis2dh12_enable();
  lis2dh12_set_interrupts(0, 1);  
  lis2dh12_set_interrupts(0, 2);

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

  //Test interrupt on watermark
  test_watermark();
  
  // Test sample rates - Depends on FIFO, watermark, RTC, pin interrupt
  test_sample_rates();
  
  //Test activity detection
  //test_activity_detection();

  //Reset lis
  //Clear memory
  lis2dh12_reset();
  //Wait for reboot
  nrf_delay_ms(10);
  lis2dh12_enable();
}
