#ifndef PACKIT_H
#define PACKIT_H

#include <stdbool.h>
#include <stdint.h>
#include "bme280.h"
#include "lis2dh12.h"

/*
0:     uint8_t   format;          // 03 = realtime sensor readings 
1:     uint8_t   humidity;        // one lsb is 0.5%
2-3:  uint16_t   temperature;     // Signed 8.8 fixed-point notation.
4-5:  uint16_t   pressure;        // (-50kPa)
6-7:   int16_t   acceleration_x;  // mg
8-9:   int16_t   acceleration_y;  // mg
10-11: int16_t   acceleration_z;  // mg
12-13: int16_t   vbat;            // mv
*/
#define SENSOR_TAG_DATA_FORMAT          0x03		  /**< raw binary, includes acceleration */
#define SENSORTAG_ENCODED_DATA_LENGTH   14

#define RAW_FORMAT_2                    0x05          /**< includes  Tx power, movement counter, Seq number and MAC
#define RAW_2_ENCODED_DATA_LENGTH       24            /**< see https://f.ruuvi.com/t/proposed-next-high-precision-data-format/692 */ 

// Sensor values
typedef struct 
{
uint8_t     format;              // 0x00 ... 0x09 for official Ruuvi applications
uint8_t     humidity;            // one lsb is 0.5%
uint16_t    temperature;         // Signed 8.8 fixed-point notation.
uint16_t    pressure;            // Pascals (pa)
int16_t     accX;                // Milli-g (mg)
int16_t     accY;
int16_t     accZ;
uint16_t    vbat;                // mv
}ruuvi_sensor_t;

/**
 *  Parses data into Ruuvi data format scale
 *  @param *data pointer to ruuvi_sensor_t object
 *  @param raw_t raw temperature as given by BME280, i.e 2s-complement int32_t in celcius * 100, -2134 = 21.34
 *  @param raw_p raw pressure as given by BME280, uint32_t, multiplied by 256
 *  @param acceleration along 3 axes in milliG, X Y Z. 
 */
void parseSensorData(ruuvi_sensor_t* data, int32_t raw_t, uint32_t raw_p, uint32_t raw_h, uint16_t vbat, int32_t acc[3]);

/**
 *  Parses sensor values into RuuviTag format.
 *  @param char* data_buffer character array with length of 14 bytes
 */
void encodeToSensorDataFormat(uint8_t* data_buffer, const ruuvi_sensor_t* data);

/**
 *  Parses sensor values into format. 
 *  @param data_buffer uint8_t array with length of 24 bytes
 *  @param environmental  Environmental data as data comes from BME280, i.e. uint32_t pressure, int32_t temperature, uint32_t humidity
 *  @param acceleration 3 x int16_t having acceleration along X-Y-Z axes in MG. Low pass and last sample are allowed DSP operations
 *  @param acceleration_events counter of acceleration events. Events are configured by application, "value exceeds 1.1 G" recommended.
 *  @param vbatt Voltage of battery in millivolts
 */
void encodeToRawFormat5(uint8_t* data_buffer, const bme280_data_t* environmental, const acceleration_t* acceleration, uint16_t acceleration_events, uint16_t vbatt, int8_t tx_pwr);

#endif
