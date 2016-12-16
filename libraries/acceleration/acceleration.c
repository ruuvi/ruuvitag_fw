/**
@addtogroup LIS2DH12Driver LIS2DH12 Acceleration Sensor Driver
@{
@file       LIS2DH12.c

Implementation of the LIS2DH12 driver.

For a detailed description see the detailed description in @ref LIS2DH12.h

* @}
***************************************************************************************************/

/* INCLUDES ***************************************************************************************/
#include <string.h>
#include <stdlib.h>

#include "acceleration.h"
#include "LIS2DH12.h"



/* CONSTANTS **************************************************************************************/


/* MACROS *****************************************************************************************/

/* TYPES ******************************************************************************************/


/* PROTOTYPES *************************************************************************************/
static void drdyCB(void);

/* VARIABLES **************************************************************************************/
int32_t lastX;     /**< last measured X Value */
int32_t lastY;     /**< last measured Y Value */
int32_t lastZ;     /**< last measured Z Value */

uint32_t thX;      /**< threshold in milli-g for X axis */
uint32_t thY;      /**< threshold in milli-g for Y axis */
uint32_t thZ;      /**< threshold in milli-g for Z axis */
uint32_t debounce; /**< minimum time between two movement detection events */
acceleration_event_t FPaccCB = NULL;

/* EXTERNAL FUNCTIONS *****************************************************************************/
extern void acceleration_init(void)
{
    LIS2DH12_init(LIS2DH12_POWER_LOW, LIS2DH12_SCALE2G, drdyCB);
}

extern void acceleration_initMovementAlert(
    uint32_t thresLevelX,
    uint32_t thresLevelY,
    uint32_t thresLevelZ,
    uint32_t debounceTimeMs,
    acceleration_event_t accCB)
{
    FPaccCB = accCB;

    thX = thresLevelX;
    thY = thresLevelY;
    thZ = thresLevelZ;
    debounce = debounceTimeMs;
}


extern void acceleration_initMovingAverage(uint32_t numVal)
{

}

extern acceleration_Ret acceleration_getAllAvgMG(int32_t* const accX, int32_t* const accY, int32_t* const accZ)
{
    return ACCELERATION_RET_ERROR;
}

extern uint32_t acceleration_getElapsedTime()
{
    return 0;
}

/* INTERNAL FUNCTIONS *****************************************************************************/

/**
 * Data ready Callback function
 *
 * This function is called when an new data sample from the acceleration sensor is received. Then
 * the function will check the difference to the acceleration values saved last time, and generate
 * a movement alert if the defined threshold are exceeded.
 */
static void drdyCB(void)
{
    static bool firstRun = true;    /**< don't call callback when data is sampled the first time */
    int32_t newX, newY, newZ;
    uint32_t difX, difY, difZ;

    LIS2DH12_getALLmG(&newX, &newY, &newZ);

    difX = (uint32_t)abs(lastX - newX);
    difY = (uint32_t)abs(lastY - newY);
    difZ = (uint32_t)abs(lastZ - newZ);

    // todo: get elapsed time

    if (((difX > thX) || (difY > thY) || (difZ > thY)) && false == firstRun)
    {
        FPaccCB();
        // todo: save timestamp
    }

    firstRun = false; // reset first ran flag

    // save values for comparing next time
    lastX = newX;
    lastY = newY;
    lastZ = newZ;
}
