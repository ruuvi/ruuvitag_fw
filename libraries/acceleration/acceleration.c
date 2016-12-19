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
/** Maximum buffer size for moving average calculation */
#define MOV_MAX_BUF 255

/* MACROS *****************************************************************************************/

/* TYPES ******************************************************************************************/


/* PROTOTYPES *************************************************************************************/
static void drdyCB(void);

/* VARIABLES **************************************************************************************/
/**
 * @addtogroup varMovementDetection Variables for movement detection
 * @{
 */
static uint32_t thX;      /**< threshold in milli-g for X axis */
static uint32_t thY;      /**< threshold in milli-g for Y axis */
static uint32_t thZ;      /**< threshold in milli-g for Z axis */
static uint32_t debounce; /**< minimum time between two movement detection events in systics */

static acceleration_event_t FPaccCB = NULL;     /**< Function pointer for callback when movement was detected */
static bool movementDetectionActive = false;    /**< Is set to true when this function is initialized */
static uint32_t timestampLastMove = 0;          /**< Systic timestamp for last movement */
static uint32_t ticsPerMs = 0;                  /**< tics per ms to calculate time elapsed in ms from elapsed ticks */
/**@}*/

/**
 * @addtogroup varMovingAverage Variables for moving average
 * @{
 */
static volatile int32_t movAvgBufX[MOV_MAX_BUF] = {0};  /**< Buffer to calculate moving average for X axis */
static volatile int32_t movAvgBufY[MOV_MAX_BUF] = {0};  /**< Buffer to calculate moving average for Y axis */
static volatile int32_t movAvgBufZ[MOV_MAX_BUF] = {0};  /**< Buffer to calculate moving average for Z axis */

static uint8_t movAvgValues = 0;            /**< Number of values to use for moving average calculation */

static bool movingAverageActive = false;    /**< Is set to true when this function is initialized */
/**@}*/

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
    uint32_t rtc1Prescaler,
    acceleration_event_t accCB)
{
    FPaccCB = accCB;

    thX = thresLevelX;
    thY = thresLevelY;
    thZ = thresLevelZ;
    debounce = APP_TIMER_TICKS(debounceTimeMs, rtc1Prescaler);
    ticsPerMs = APP_TIMER_TICKS(1, rtc1Prescaler);

    movementDetectionActive = true;
}


extern void acceleration_initMovingAverage(uint8_t numVal)
{
    movAvgValues = numVal;
    movingAverageActive = true;
}

extern acceleration_Ret acceleration_getAllAvgMG(int32_t* const accX, int32_t* const accY, int32_t* const accZ)
{
    acceleration_Ret retVal = ACCELERATION_RET_OK;
    uint32_t ii = 0; // loop counter
    int32_t sumX = 0;
    int32_t sumY = 0;
    int32_t sumZ = 0;

    if ((NULL == accX) || (NULL == accY) || (NULL == accZ))
    {
        retVal = ACCELERATION_RET_ERROR;
    }
    else
    {
        for (ii = 0; ii < movAvgValues; ii++)
        {
            sumX += movAvgBufX[ii];
            sumY += movAvgBufY[ii];
            sumZ += movAvgBufZ[ii];
        }

        *accX = sumX / movAvgValues;
        *accY = sumY / movAvgValues;
        *accZ = sumZ / movAvgValues;
    }

    return retVal;
}

extern uint32_t acceleration_getElapsedTime()
{
    uint32_t timeElapsed = 0;   // time elapsed in tics
    uint32_t timeElapsedMs = 0; //time elapsed in ms

    // if timestamp is 0 no move happened
    if (timestampLastMove > 0)
    {
        app_timer_cnt_diff_compute(app_timer_cnt_get(), timestampLastMove, &timeElapsed); // get elapsed time
    }

    if (timeElapsed > 0)
    {
        timeElapsedMs = timeElapsed / ticsPerMs;
    }
    else
    {
        timeElapsedMs = 0;
    }
    return timeElapsedMs;
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
    static bool firstRun = true;/**< don't call callback when data is sampled the first time */

    static int32_t lastX;       /* last measured X Value */
    static int32_t lastY;       /* last measured Y Value */
    static int32_t lastZ;       /* last measured Z Value */

    static uint32_t movAvgBufferPos = 0;/* Point to the possition in the buffer that will be written next time */

    int32_t newX, newY, newZ;   /* new values from acceleration sensor */
    uint32_t difX, difY, difZ;  /* calculated abs difference between old and new value */
    uint32_t timeElapsed;       /* elapsed time since last saved timestamp */

    // Read values from acceleration sensor
    LIS2DH12_getALLmG(&newX, &newY, &newZ);

    if (true == movementDetectionActive)
    {
        difX = (uint32_t)abs(lastX - newX);
        difY = (uint32_t)abs(lastY - newY);
        difZ = (uint32_t)abs(lastZ - newZ);

        app_timer_cnt_diff_compute(app_timer_cnt_get(), timestampLastMove, &timeElapsed); // get elapsed time

        if (((difX > thX) || (difY > thY) || (difZ > thY)) && false == firstRun && timeElapsed >= debounce)
        {
            FPaccCB();
            timestampLastMove = app_timer_cnt_get();
        }

        firstRun = false; // reset first ran flag

        // save values for comparing next time
        lastX = newX;
        lastY = newY;
        lastZ = newZ;
    }

    if (true == movingAverageActive)
    {
        movAvgBufX[movAvgBufferPos] = newX;
        movAvgBufY[movAvgBufferPos] = newY;
        movAvgBufZ[movAvgBufferPos] = newZ;

        movAvgBufferPos++;
        if (movAvgBufferPos >= movAvgValues)
        {
            movAvgBufferPos = 0;
        }
    }
}
