#ifndef SENSORTAG_H
#define SENSORTAG_H


#include <stdbool.h>
#include <stdint.h>


/*
0:   uint8_t     format;          // (0x03 = realtime sensor readings base64)
1:   uint8_t     humidity;        // one lsb is 0.5%
2-3: uint16_t    temperature;     // Signed 8.8 fixed-point notation.
4-5: uint16_t    pressure;        // (-50kPa)
6-7:   int16_t   acceleration_x;  // mg
8-9:   int16_t   acceleration_y;  // mg
10-11: int16_t   acceleration_z;  // mg
12-13: int16_t   vbat;            // mv
*/
#define SENSOR_TAG_DATA_FORMAT          0x03				  /**< raw binary, includes acceleration */
#define SENSORTAG_ENCODED_DATA_LENGTH   14            /* 14 bytes  */

#define WEATHER_STATION_URL_FORMAT      0x02				  /**< Base64 */
#define WEATHER_STATION_URL_ID_FORMAT   0x04				  /**< Base64, with ID byte */


#define EDDYSTONE_URL_MAX_LENGTH 17
#define URL_PAYLOAD_LENGTH 9

#define URL_BASE_MAX_LENGTH (EDDYSTONE_URL_MAX_LENGTH - URL_PAYLOAD_LENGTH)

// Sensor values
typedef struct 
{
uint8_t     format;         // 0x03
uint8_t     humidity;       // one lsb is 0.5%
uint16_t    temperature;    // Signed 8.8 fixed-point notation.
uint16_t    pressure;       // Pascals (pa)
int16_t     accX;           // Milli-g (mg)
int16_t     accY;
int16_t     accZ;
uint16_t    vbat;           // mv
}ruuvi_sensor_t;

/**
 *  Parses data into Ruuvi data format scale
 *  @param *data pointer to ruuvi_sensor_t object
 *  @param raw_t raw temperature as given by BME280, I.E 2-complement int32_t in celcius * 100, -2134 = 21.34
 *  @param raw_p raw pressure as given by BME280, uint32_t, multiplied by 256
 *  @param acceleration along 3 axes in milliG, X Y Z. 
 */
void parseSensorData(ruuvi_sensor_t* data, int32_t raw_t, uint32_t raw_p, uint32_t raw_h, uint16_t vbat, int32_t acc[3]);


/**
 *  Parses sensor values into RuuviTag format.
 *  @param char* data_buffer character array with length of 14 bytes
 */
void encodeToSensorDataFormat(uint8_t* data_buffer, ruuvi_sensor_t* data);


/**
 *  Encodes sensor data into given char* url. The base url must have the base of url written by caller.
 *  For example, url = {'r' 'u' 'u' '.' 'v' 'i' '/' '#' '0' '0' '0' '0' '0' '0' '0' '0' '0'}
 *  The URL may have a length of 18 bytes, 8 of which is consumed by payload. 
 *  
 *  @param url pointer to character array with max length of 18
 *  @param base_length length of base url. The payload will be written starting at index at base length
 *  @param data pointer to ruuvi_sensor_t which contains current sensor data to be encoded
 */
void encodeToUrlDataFromat(char* url, uint8_t base_length, ruuvi_sensor_t* data);

#endif
