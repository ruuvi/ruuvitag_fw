#include "application.h"
#include "ble_acceleration_service.h"
#include "LIS2DH12.h"

#define NRF_LOG_MODULE_NAME "APP"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

/**
 *  This function is called when new accelerometer sample is ready.
 */
static ble_acceleration_service_t    m_acceleration_service; 
static ble_acceleration_service_t*   p_acceleration_service = &m_acceleration_service;
static ble_acceleration_service_imu_data_t  m_imu_data;
static ble_acceleration_service_imu_data_t* p_imu_data = &m_imu_data;
static uint8_t imu_buffer[IMU_DATA_SIZE]= {0};
void accelerometer_callback(void)
{
  static uint32_t counter = 0;
  int32_t x,y,z;
  
  //Data is ready by the time this function is called.
  LIS2DH12_getALLmG(&x, &y, &z);
  
  //Populate IMU data structure
  m_imu_data.imu_array.size   = IMU_DATA_SIZE;
  m_imu_data.imu_array.p_data = imu_buffer;
  imu_buffer[IMU_FORMAT_INDEX] = IMU_FORMAT_ACCELERATION_DATA;
  imu_buffer[IMU_X_MSB_INDEX]  = (uint8_t)((x>>8) | (x<0? 0x80 : 0)); //shift MSB of X, take sign.
  imu_buffer[IMU_X_LSB_INDEX]  = (uint8_t)(x & 0x00FF);               //Take LSB of X
  imu_buffer[IMU_Y_MSB_INDEX]  = (uint8_t)((y>>8) | (y<0? 0x80 : 0)); 
  imu_buffer[IMU_Y_LSB_INDEX]  = (uint8_t)(y & 0x00FF);               
  imu_buffer[IMU_Z_MSB_INDEX]  = (uint8_t)((z>>8) | (z<0? 0x80 : 0)); 
  imu_buffer[IMU_Z_LSB_INDEX]  = (uint8_t)(z & 0x00FF);               
  
  if(!(counter % APPLICATION_ACCELERATION_DIVISOR))
  {
    //Error code is not verified, drop the packet if send fails.
    ble_acceleration_service_imu_data_send(p_acceleration_service, p_imu_data);
  }
}
