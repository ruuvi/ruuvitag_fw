#include "application.h"
#include "ble_acceleration_service.h"
#include "ble_ess.h"
#include "LIS2DH12.h"
#include "bme280.h"

#define NRF_LOG_MODULE_NAME "APP"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

/**
 *  This function is called when new accelerometer sample is ready.
 */
/*extern ble_acceleration_service_t    m_acceleration_service; 
static ble_acceleration_service_t*   p_acceleration_service = &m_acceleration_service;
static ble_acceleration_service_imu_data_t  m_imu_data;
static ble_acceleration_service_imu_data_t* p_imu_data = &m_imu_data;
static uint8_t imu_buffer[IMU_DATA_SIZE];
*/
void accelerometer_callback(void)
{
  /*
  static uint32_t counter = 0;
  int32_t x,y,z;
  
  int8_t unread = LIS2DH12_getFifoDepth(); //Number of elements in FiFo
  //Read through FiFo. 
  while(unread){
    //Data is ready by the time this function is called.
    LIS2DH12_getALLmG(&x, &y, &z);
    
    unread = LIS2DH12_getFifoDepth();
  
    //Populate IMU data structure
    p_imu_data->imu_array.size   = IMU_DATA_SIZE;
    p_imu_data->imu_array.p_data = imu_buffer;
    p_imu_data->imu_array.p_data[IMU_FORMAT_INDEX] = IMU_FORMAT_ACCELERATION_DATA;
    //Add age, type indexes? Add Peak / Average? Add impulse?
    p_imu_data->imu_array.p_data[IMU_X_MSB_INDEX]  = (uint8_t)((x>>8) | (x<0? 0x80 : 0)); //shift MSB of X, take sign.
    p_imu_data->imu_array.p_data[IMU_X_LSB_INDEX]  = (uint8_t)(x & 0x00FF);               //Take LSB of X
    p_imu_data->imu_array.p_data[IMU_Y_MSB_INDEX]  = (uint8_t)((y>>8) | (y<0? 0x80 : 0)); 
    p_imu_data->imu_array.p_data[IMU_Y_LSB_INDEX]  = (uint8_t)(y & 0x00FF);               
    p_imu_data->imu_array.p_data[IMU_Z_MSB_INDEX]  = (uint8_t)((z>>8) | (z<0? 0x80 : 0)); 
    p_imu_data->imu_array.p_data[IMU_Z_LSB_INDEX]  = (uint8_t)(z & 0x00FF);               
  
    if(!(counter % APPLICATION_ACCELERATION_DIVISOR))
    {
      //Error code is not verified, drop the packet if send fails.
      uint32_t err_code;
      NRF_LOG_DEBUG("Send %d %d %d\r\n", x, y, z);
      NRF_LOG_HEXDUMP_DEBUG(p_imu_data->imu_array.p_data, p_imu_data->imu_array.size );
      err_code = ble_acceleration_service_imu_data_set(p_acceleration_service, p_imu_data);  //Set data to be send / read
      NRF_LOG_DEBUG("Set status: %d \r\n", err_code);
      err_code = ble_acceleration_service_imu_data_send(p_acceleration_service, p_imu_data); //Send data as notification
      NRF_LOG_DEBUG("Send status: %d \r\n", err_code);
    }
    counter++;
  }
  */
}

/**
 *  This function is called when new environmental sample is ready.
 */
extern ble_ess_t    m_ess; 
static ble_ess_t*   p_ess = &m_ess;
static ble_ess_humidity_t  m_humidity;
static ble_ess_humidity_t* p_humidity = &m_humidity;
static ble_ess_temperature_t m_temperature;
static ble_ess_temperature_t* p_temperature = &m_temperature;
static ble_ess_pressure_t m_pressure;
static ble_ess_pressure_t* p_pressure = &m_pressure;
static ble_ess_barometric_pressure_trend_t m_trend;
static ble_ess_barometric_pressure_trend_t* p_trend = &m_trend;
void environmental_callback(void)
{
    //Called in bme timer event.
    //bme280_read_measurements();
    //TODO: Check BLE sig standards on how values should be presented.
    uint32_t err_code;
    
    // Unit is in percent with a resolution of 0.01 percent
    // https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.characteristic.humidity.xml&u=org.bluetooth.characteristic.humidity.xml&_ga=2.230244693.836809453.1493828806-422840083.1493828806
    /**
    * Returns humidity in %RH as unsigned 32 bit integer in Q22.10 format
    * (22 integer and 10 fractional bits).
    * Output value of “50532” represents 50532/1024 = 49.356 %RH
    */
    uint32_t humidity = bme280_get_humidity();
    //Scale by 100x
    humidity *= 100;
    //Divide into 0.01 accuracy
    humidity /= 1024;
    
    // Unit is in pascals with a resolution of 0.1 Pa 
    // https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.characteristic.pressure.xml&u=org.bluetooth.characteristic.pressure.xml&_ga=2.24077043.836809453.1493828806-422840083.1493828806
    /**
    * Returns pressure in Pa as unsigned 32 bit integer in Q24.8 format
    * (24 integer bits and 8 fractional bits).
    * Output value of “24674867” represents 24674867/256 = 96386.2 Pa = 963.862 hPa
    */
    uint32_t pressure = bme280_get_pressure();
    //Scale by 10x
    pressure *= 10;
    //Divide into 0.1 Pa
    pressure /= 256;
    
    // Unit is in degrees Celsius with a resolution of 0.01 degrees 
    // https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.characteristic.temperature.xml&u=org.bluetooth.characteristic.temperature.xml&_ga=2.24077043.836809453.1493828806-422840083.1493828806
    /**
    * Returns temperature in DegC, resolution is 0.01 DegC.
    * Output value of “2134” equals 21.34 DegC.
    */
    int32_t  temperature = bme280_get_temperature();
    
    p_humidity->humidity = humidity;
    p_temperature->temperature = temperature;
    p_pressure->pressure = pressure;
    
    barometric_pressure_trend_barometric_pressure_trend_t trend;
    trend.barometric_pressure_trend = BAROMETRIC_PRESSURE_TREND_UNKNOWN; //TODO
    p_trend->barometric_pressure_trend = trend; 
    
    err_code = ble_ess_pressure_set(p_ess, p_pressure);//TODO: Check error codes, queue transmissions
    err_code |= ble_ess_pressure_send(p_ess, p_pressure);
    
    err_code |= ble_ess_temperature_set(p_ess,  p_temperature);
    err_code |= ble_ess_temperature_send(p_ess, p_temperature);
    
    err_code |= ble_ess_humidity_set (p_ess, p_humidity);
    err_code |= ble_ess_humidity_send(p_ess, p_humidity);
    
    err_code |= ble_ess_barometric_pressure_trend_set (p_ess, p_trend); 
    //APP_ERROR_CHECK(err_code); Fails if notifications are not registered
}
