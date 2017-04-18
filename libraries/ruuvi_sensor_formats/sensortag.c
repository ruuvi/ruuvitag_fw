#include "sensortag.h"


#include <stdint.h>

#include "base64.h"

#define NRF_LOG_MODULE_NAME "SENSORLIB"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

/**
 *  Parses data into Ruuvi data format scale
 *  @param *data pointer to ruuvi_sensor_t object
 *  @param raw_t raw temperature as given by BME280, I.E 2-complement int32_t in celcius * 100, -2134 = 21.34
 *  @param raw_p raw pressure as given by BME280, uint32_t, multiplied by 256
 *  @param acceleration along 3 axes in milliG, X Y Z. 
 */

void parseSensorData(ruuvi_sensor_t* data, int32_t raw_t, uint32_t raw_p, uint32_t raw_h, uint16_t vbat, int32_t acc[3])
{
   
    NRF_LOG_DEBUG("temperature: %d, pressure: %d, humidity: %d\r\n", raw_t, raw_p, raw_h);

    /*
    0:   uint8_t     format;          // (0x02 = realtime sensor readings base64)
    1:   uint8_t     humidity;        // one lsb is 0.5%
    2-3: uint16_t    temperature;     // Signed 8.8 fixed-point notation.
    4-5: uint16_t    pressure;        // (-50kPa)
    */
    //Convert raw values to ruu.vi specification
    //Round values: 1 deg C, 1 hPa, 1% RH 
    data->format = 0x00; //Will be decided in encoding phase
    data->temperature = (raw_t < 0) ? 0x8000 : 0x0000; //Sign bit
    if(raw_t < 0) raw_t = 0-raw_t; // disrecard sign
    data->temperature |= (((raw_t / 100) << 8));       //raw_t is 2:2 signed fixed point in base-10, Drop decimals, scale up to next byte.
    data->temperature |= (raw_t % 100);                //take decimals.
    data->pressure = (uint16_t)((raw_p >> 8) - 50000); //Scale into pa, Shift by -50000 pa as per Ruu.vi interface.
    data->humidity = (uint8_t)(raw_h >> 9);            //scale into 0.5%

    // Set accelerometer data
    data->accX = acc[0];
    data->accY = acc[1];
    data->accZ = acc[2];
    
    data->vbat = vbat;


}

/**
 *  Parses sensor values into RuuviTag format.
 *  @param char* data_buffer character array with length of 14 bytes
 */

void encodeToSensorDataFormat(uint8_t* data_buffer, ruuvi_sensor_t* data)

{
    //serialize values into a string
    data_buffer[0] = SENSOR_TAG_DATA_FORMAT;
    data_buffer[1] = data->humidity;
    data_buffer[2] = (data->temperature)>>8;
    data_buffer[3] = (data->temperature)&0xFF;
    data_buffer[4] = (data->pressure)>>8;
    data_buffer[5] = (data->pressure)&0xFF;
    data_buffer[6] = (data->accX)>>8;
    data_buffer[7] = (data->accX)&0xFF;
    data_buffer[8] = (data->accY)>>8;
    data_buffer[9] = (data->accY)&0xFF;
    data_buffer[10] = (data->accZ)>>8;
    data_buffer[11] = (data->accZ)&0xFF;
    data_buffer[12] = (data->vbat)>>8;
    data_buffer[13] = (data->vbat)&0xFF;
}

/**
 *  Encodes sensor data into given char* url. The base url must have the base of url written by caller.
 *  For example, url = {'r' 'u' 'u' '.' 'v' 'i' '/' '#' '0' '0' '0' '0' '0' '0' '0' '0' '0'}
 *  The URL may have a length of 18 bytes, 8 of which is consumed by payload. 
 *  
 *  @param url pointer to character array with max length of 18
 *  @param base_length length of base url. The payload will be written starting at index at base length
 *  @
 */
void encodeToUrlDataFromat(char* url, uint8_t base_length, ruuvi_sensor_t* data)
{


    //Create pseudo-unique name
    unsigned int mac0 =  NRF_FICR->DEVICEID[0];
    uint8_t serial[2];
    serial[0] = mac0      & 0xFF;
    serial[1] = (mac0>>8) & 0xFF;
    
    //serialize values into a string
    char pack[8] = {0};
    pack[0] = WEATHER_STATION_URL_ID_FORMAT;
    uint8_t humidity = data->humidity / 4; //Round to 2 %
    pack[1] = humidity * 4;
    int16_t temperature = data->temperature;
    pack[2] = (temperature) >> 8;
    pack[3] = 0;          //Round off decimals
    uint16_t pressure =   data->pressure;
    pressure = pressure - (pressure % 100); //Round pressure to hPa accuracy
    pack[4] = (pressure)>>8;
    pack[5] = (pressure)&0xFF;
    pack[6] = serial[0];
  
     
    /// Encoding 48 bits using Base64 produces max 8 chars.
    memset(&(url[base_length]), 0, sizeof(URL_PAYLOAD_LENGTH));
    base64encode(pack, sizeof(pack), &(url[base_length]), URL_PAYLOAD_LENGTH);

}
