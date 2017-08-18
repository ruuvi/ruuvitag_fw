#include "test_environmental.h"

#include "app_timer.h" //TODO: refactor scheduler dependency out of the driver
#include "nrf_delay.h"
#include "bme280.h"

#define NRF_LOG_MODULE_NAME "TEST_ENVIRONMENTAL"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"


void test_environmental(void)
{
  NRF_LOG_INFO("Starting environmental test. Taking a few samples\r\n");
  static int32_t raw_t  = 0;
  static uint32_t raw_p = 0;
  static uint32_t raw_h = 0;
  uint32_t err_code = NRF_SUCCESS;
  //First sample
  err_code |= bme280_set_mode(BME280_MODE_FORCED);
  nrf_delay_ms(100);
  //read previous data, start next sample
  bme280_set_mode(BME280_MODE_FORCED);
  nrf_delay_ms(10);
  // Get raw environmental data
  raw_t = bme280_get_temperature();
  raw_p = bme280_get_pressure();
  raw_h = bme280_get_humidity();
  NRF_LOG_INFO("temperature: %d.%d, pressure: %d, humidity: %d\r\n", raw_t/100, raw_t%100, raw_p>>8, raw_h>>10); //Wrong decimals on negative values.
  
  err_code |= bme280_set_mode(BME280_MODE_FORCED);
  nrf_delay_ms(10);
  raw_t = bme280_get_temperature();
  raw_p = bme280_get_pressure();
  raw_h = bme280_get_humidity();
  NRF_LOG_INFO("temperature: %d.%d, pressure: %d, humidity: %d\r\n", raw_t/100, raw_t%100, raw_p>>8, raw_h>>10); //Wrong decimals on negative values.
    
  err_code |= bme280_set_mode(BME280_MODE_FORCED);
  raw_t = bme280_get_temperature();
  raw_p = bme280_get_pressure();
  raw_h = bme280_get_humidity();
  NRF_LOG_INFO("temperature: %d.%d, pressure: %d, humidity: %d\r\n", raw_t/100, raw_t%100, raw_p>>8, raw_h>>10); //Wrong decimals on negative values.  
  NRF_LOG_FLUSH();
  nrf_delay_ms(10);

  err_code |= bme280_set_mode(BME280_MODE_NORMAL);
  NRF_LOG_INFO("Ok, switching to normal mode, status %s.\r\n", (uint32_t)ERR_TO_STR(err_code));
  for(int ii = 0; ii < 15; ii++)
  {
    app_sched_execute();
    raw_t = bme280_get_temperature();
    raw_p = bme280_get_pressure();
    raw_h = bme280_get_humidity();
    NRF_LOG_INFO("temperature: %d.%d, pressure: %d.%d, humidity: %d.%d\r\n", raw_t/100, raw_t%100, raw_p>>8, ((raw_p*1000)>>8)%1000, raw_h>>10, ((raw_h*1000)>>10)%1000); //Wrong decimals on negative values.  
    NRF_LOG_FLUSH();
    nrf_delay_ms(1100);
  }
  
  err_code |= bme280_set_oversampling_hum(BME280_OVERSAMPLING_16);
  NRF_LOG_INFO("Tried to configure while not sleeping, status should not be 0: %d.\r\n", (uint32_t)err_code);
  NRF_LOG_FLUSH();
  nrf_delay_ms(10);
  err_code = bme280_set_mode(BME280_MODE_SLEEP);
  bme280_set_oversampling_hum(BME280_OVERSAMPLING_16);
  bme280_set_oversampling_temp(BME280_OVERSAMPLING_16);
  bme280_set_oversampling_press(BME280_OVERSAMPLING_16);
  NRF_LOG_INFO("Checking status register reading..\r\n");
  err_code |= bme280_set_mode(BME280_MODE_FORCED);
  int  active = bme280_is_measuring();
  NRF_LOG_INFO("BME280 is taking sample? %d\r\n", (uint32_t)active);
  err_code |= bme280_set_mode(BME280_MODE_FORCED);
  active = bme280_is_measuring();
  NRF_LOG_INFO("BME280 is taking sample? %d\r\n", (uint32_t)active);
  err_code |= bme280_set_mode(BME280_MODE_NORMAL);
  NRF_LOG_INFO("Ok, applied oversampling, status %s. Noise should be decreased. Place bme280 to fan exhaust to see the effect.\r\n", (uint32_t)ERR_TO_STR(err_code));
  for(int ii = 0; ii < 15; ii++)
  {
    app_sched_execute();
    raw_t = bme280_get_temperature();
    raw_p = bme280_get_pressure();
    raw_h = bme280_get_humidity();
    NRF_LOG_INFO("temperature: %d.%d, pressure: %d.%d, humidity: %d.%d\r\n", raw_t/100, raw_t%100, raw_p>>8, ((raw_p*1000)>>8)%1000, raw_h>>10, ((raw_h*1000)>>10)%1000); //Wrong decimals on negative values.  
    NRF_LOG_FLUSH();
    nrf_delay_ms(1100);
  }
  
  err_code |= bme280_set_mode(BME280_MODE_SLEEP);
  bme280_set_iir(BME280_IIR_16);
  err_code |= bme280_set_mode(BME280_MODE_NORMAL);
  NRF_LOG_INFO("Ok, applying IIR, status %s. Please breathe on bme280. Values should adjust slowly.\r\n", (uint32_t)ERR_TO_STR(err_code));
  for(int ii = 0; ii < 15; ii++)
  {
    app_sched_execute();
    raw_t = bme280_get_temperature();
    raw_p = bme280_get_pressure();
    raw_h = bme280_get_humidity();
    NRF_LOG_INFO("temperature: %d.%d, pressure: %d.%d, humidity: %d.%d\r\n", raw_t/100, raw_t%100, raw_p>>8, ((raw_p*1000)>>8)%1000, raw_h>>10, ((raw_h*1000)>>10)%1000); //Wrong decimals on negative values.  
    NRF_LOG_FLUSH();
    nrf_delay_ms(1100);
  }
  
  err_code |= bme280_set_mode(BME280_MODE_SLEEP);
  bme280_set_iir(BME280_IIR_OFF);
  err_code |= bme280_set_mode(BME280_MODE_NORMAL);
  NRF_LOG_INFO("Ok, Removing IIR, status %s. Please breathe on bme280. Values should adjust quickly.\r\n", (uint32_t)ERR_TO_STR(err_code));
  for(int ii = 0; ii < 15; ii++)
  {
    app_sched_execute();
    raw_t = bme280_get_temperature();
    raw_p = bme280_get_pressure();
    raw_h = bme280_get_humidity();
    NRF_LOG_INFO("temperature: %d.%d, pressure: %d.%d, humidity: %d.%d\r\n", raw_t/100, raw_t%100, raw_p>>8, ((raw_p*1000)>>8)%1000, raw_h>>10, ((raw_h*1000)>>10)%1000); //Wrong decimals on negative values.  
    NRF_LOG_FLUSH();
    nrf_delay_ms(1100);
  }
  bme280_set_mode(BME280_MODE_SLEEP);
}
