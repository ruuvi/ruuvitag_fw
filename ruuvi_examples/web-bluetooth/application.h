#ifndef APPLICATION_H
#define APPLICATION_H

#define IMU_FORMAT_INDEX 0
#define IMU_X_MSB_INDEX  1
#define IMU_X_LSB_INDEX  2
#define IMU_Y_MSB_INDEX  3
#define IMU_Y_LSB_INDEX  4
#define IMU_Z_MSB_INDEX  5
#define IMU_Z_LSB_INDEX  6
#define IMU_DATA_SIZE    7
#define IMU_FORMAT_ACCELERATION_DATA 0

#define APPLICATION_ACCELERATION_DIVISOR 10

/**
 *  This function is called when new accelerometer sample is ready.
 */
void accelerometer_callback(void);

#endif
