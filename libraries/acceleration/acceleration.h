/**
@addtogroup accelerationLib Acceleration Sensor Library
@{
@file       acceleration.h

Library for Acceleration Sensor Functionalities:
 - Movement alert with configurable threshold
 - Calculate moving average for acceleration sensor values

More maybe coming up in future


* @}
***************************************************************************************************/
#ifndef ACCELERATION_H
#define ACCELERATION_H

#ifdef __cplusplus
extern "C"
{
#endif

/* INCLUDES ***************************************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include "app_error.h"

/* CONSTANTS **************************************************************************************/

/* MACROS *****************************************************************************************/

/* TYPES ******************************************************************************************/
typedef enum
{
    ACCELERATION_RET_OK,
    ACCELERATION_RET_ERROR
} acceleration_Ret;



/** Callback Type acceleration Sensor Event*/
typedef void (*acceleration_event_t)(void);

/* PROTOTYPES *************************************************************************************/

/**
 * Initialize the Acceleration Sensor Library
 *
 * This function initialize the library. During this initialization the acceleration sensor driver will
 * be initialized and configured to be used with this library.
 */
extern void acceleration_init(void);

/**
 * Initialize Movement Alert
 *
 * This function initialize the movement alert. The passed callback will be called when the measured value is above
 * the configured threshold. To avoid continous calling, a hysteresis can be configured, so the measured value have
 * to fall below a specified value (Threshold - Hysteresis) to be called again. For comperation with the threshold,
 * the absolute value of the sensor values will be used.
 *
 * @param[in] thresLevelX   Threshold Level for X Axis in mG, when to report a movement Event. Allowed Range: 0..2000
 * @param[in] thresLevelY   Threshold Level for X Axis in mG, when to report a movement Event. Allowed Range: 0..2000
 * @param[in] thresLevelZ   Threshold Level for X Axis in mG, when to report a movement Event. Allowed Range: 0..2000
 * @param[in] debounceTimeMs  Debounce time in miliseconds
 * @param[in] rtc1Prescaler Value of the RTC1 PRESCALER register (must be the same value that was passed to APP_TIMER_INIT()).
 * @param[in] accCB         Functionpointer for Movement Event
 *
 * @return ACCELERATION_RET_OK          Init successful
 * @return ACCELERATION_RET_ERROR       Level out of range
 */
extern void acceleration_initMovementAlert(
    uint32_t thresLevelX,
    uint32_t thresLevelY,
    uint32_t thresLevelZ,
    uint32_t debounceTimeMs,
    uint32_t rtc1Prescaler,
    acceleration_event_t accCB);

/**
 * Initialize Moving Average filter
 *
 * This function is needed to be called before using the function acceleration_getAllAvgMG
 *
 * @param[in] numVal    Number of values to take in account for moving average
 */
extern void acceleration_initMovingAverage(uint8_t numVal);


/**
 * Get Moving Average of acceleration
 *
 * This function returns the moving average of the acceleration sensor values.
 * Before using this function the acceleration_initMovingAverage have to be called to define the number of values
 * to take in account.
 *
 * @param[out] accX Acceleration in mG
 * @param[out] accY Acceleration in mG
 * @param[out] accZ Acceleration in mG
 *
 * @return ACCELERATION_RET_OK    No Error
 * @return ACCELERATION_RET_ERROR NULL Pointer detected or not initialized
 */
extern acceleration_Ret acceleration_getAllAvgMG(int32_t* const accX, int32_t* const accY, int32_t* const accZ);

/**
 * Get Time since last movement
 *
 * @return  Time since last movement in miliseconds
 */

extern uint32_t acceleration_getElapsedTime();

#ifdef __cplusplus
}
#endif

#endif  /* ACCELERATION_H */
