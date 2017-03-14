#include "application.h"
#include "ble_acceleration_service.h"
#include "LIS2DH12.h"

#define NRF_LOG_MODULE_NAME "APP"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

/**
 *  This function is called when new accelerometer sample is ready.
 */
extern ble_acceleration_service_t    m_acceleration_service; 
static ble_acceleration_service_t*   p_acceleration_service = &m_acceleration_service;
static ble_acceleration_service_imu_data_t  m_imu_data;
static ble_acceleration_service_imu_data_t* p_imu_data = &m_imu_data;
static uint8_t imu_buffer[IMU_DATA_SIZE];
void accelerometer_callback(void)
{
  static uint32_t counter = 0;
  int32_t x,y,z;
  
  //Data is ready by the time this function is called.
  LIS2DH12_getALLmG(&x, &y, &z);
  
  //Populate IMU data structure
  p_imu_data->imu_array.size   = IMU_DATA_SIZE;
  p_imu_data->imu_array.p_data = imu_buffer;
  p_imu_data->imu_array.p_data[IMU_FORMAT_INDEX] = IMU_FORMAT_ACCELERATION_DATA;
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
