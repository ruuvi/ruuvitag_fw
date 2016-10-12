/**
@addtogroup LIS2DH12Driver LIS2DH12 Acceleration Sensor Driver
@{
@file       LIS2DH12.h

Hardware Driver for the LIS2DH12 Acceleration Sensor

* @}
***************************************************************************************************/
#ifndef LIS2DH12_H
#define LIS2DH12_H

#ifdef __cplusplus
extern "C"
{
#endif

/* INCLUDES ***************************************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include "app_error.h"

#include "nrf_drv_gpiote.h"

/* CONSTANTS **************************************************************************************/

/* MACROS *****************************************************************************************/

/* TYPES ******************************************************************************************/
/** States of the module */
typedef enum
{
    LIS2DH12_RET_OK = 0,   		    /**< Ok */
	LIS2DH12_RET_NOT_SUPPORTED = 1,	/**< Feature not supported at the moment */
	LIS2DH12_RET_INVALID = 2,		/**< Returned data may be not valid, because of Power Down Mode or Data not ready */
	LIS2DH12_RET_NULL = 4,			/**< NULL Pointer detected */
	LIS2DH12_RET_ERROR_SELFTEST = 8,/**< Selftest  failed */
    LIS2DH12_RET_ERROR = 16    		/**< Not otherwise specified error */
} LIS2DH12_Ret;

/** Available Power Modes for the LIS2DH12 */
typedef enum{
	LIS2DH12_POWER_NORMAL = 0,	/**< Normal Power Mode, 10-bit resoulution, 100Hz, 20uA */
	LIS2DH12_POWER_LOW,			/**< Low Power Mode, 10-bit resolution, 1Hz, 2uA */
	LIS2DH12_POWER_FAST,		/**< Low Power Mode, 8-bit resolution, 1620Hz, 100uA */
	LIS2DH12_POWER_HIGHRES,		/**< High Power Mode, 12-bit resolution, 1344Hz, 185uA  */
	LIS2DH12_POWER_DOWN			/**< Stop Operation */
} LIS2DH12_PowerMode;

/** Available Scales */
typedef enum{
	LIS2DH12_SCALE2G = 0,		/**< Scale Selection: +/- 2g */
	LIS2DH12_SCALE4G = 1,		/**< Scale Selection: +/- 4g */
	LIS2DH12_SCALE8G = 2,		/**< Scale Selection: +/- 8g */
	LIS2DH12_SCALE16G = 3		/**< Scale Selection: +/- 16g */
}LIS2DH12_Scale;

/** Data Ready Event Callback Type */
typedef void (*LIS2DH12_drdy_event_t)(void);

/* PROTOTYPES *************************************************************************************/

/**
 * Initialize the Acceleration Sensor
 *
 * This Function initializes the Acceleration Sensor LIS2DH12 to work with the SPI Interface. The
 * SPI Interface will be initialized by this function accordingly. This Function also set the
 * requested Power mode for the LIS2DH12. All Axis(x,y,z) will be enabled.
 * The Data Ready Callback Function can be used to get notified when new Data is available. But causen,
 * the callback will be called in interrupt context. 
 *
 * @param[in] powerMode Requested Power Mode the LIS2DH12 should work with.
 * @param[in] scale 	Scale, the Sensor shall operate.
 * @param[in] drdyCB 	Data Ready Callback, optional, pass over NULL if not used
 *
 * @retval LIS2DH12_RET_OK 			Init successful
 * @retval LIS2DH12_RET_ERROR 		Something went wrong
 * @retval LIS2DH12_NOT_SUPPORTED 	Requested powerMode or scale not yet supported
 */
extern LIS2DH12_Ret LIS2DH12_init(LIS2DH12_PowerMode powerMode, LIS2DH12_Scale scale, LIS2DH12_drdy_event_t drdyCB);


/**
 * Change power mode
 *
 * This function changes the current power mode the acceleration sensor is running. In some use cases it useful for battery saving to
 * run the sensor in a low power mode to detect the start of a movement and then switch to a higher resolution.
 *
 * Note: This function only works after correct initialization.
 * Note: After changeing the power mode, it needs some time till new values are available, see datasheet for details
 *
 * @param[in] powerMode Requested Power Mode the LIS2DH12 should work with.
 *
 * @retval LIS2DH12_RET_OK          Change successful
 * @retval LIS2DH12_RET_ERROR       Something went wrong
 *
 */
extern LIS2DH12_Ret LIS2DH12_setPowerMode(LIS2DH12_PowerMode powerMode);

/**
 * Return X acceleration
 *
 * @param[out] accX Acceleration in mG
 *
 * @retval LIS2DH12_RET_OK 			Data valid
 * @retval LIS2DH12_RET_INVALID 	Data invalid because of power down or data not ready
 * @retval LIS2DH12_RET_NULL NULL 	Pointer detected
 */
extern LIS2DH12_Ret LIS2DH12_getXmG(int32_t* const accX);

/**
 * Return Y acceleration
 *
 * @param[out] accY Acceleration in mG
 *
 * @retval LIS2DH12_RET_OK 			Data valid
 * @retval LIS2DH12_RET_INVALID 	Data invalid because of power down or data not ready
 * @retval LIS2DH12_RET_NULL NULL 	Pointer detected
 */
extern LIS2DH12_Ret LIS2DH12_getYmG(int32_t* const accY);

/**
 * Return Z acceleration
 *
 * @param[out] accZ Acceleration in mG
 *
 * @retval LIS2DH12_RET_OK 			Data valid
 * @retval LIS2DH12_RET_INVALID 	Data invalid because of power down or data not ready
 * @retval LIS2DH12_RET_NULL NULL 	Pointer detected
 */
extern LIS2DH12_Ret LIS2DH12_getZmG(int32_t* const accZ);

/**
 * Return acceleration of all axis
 *
 * @param[out] accX Acceleration in mG
 * @param[out] accY Acceleration in mG
 * @param[out] accZ Acceleration in mG
 *
 * @retval LIS2DH12_RET_OK 			Data valid
 * @retval LIS2DH12_RET_INVALID 	Data invalid because of power down or data not ready
 * @retval LIS2DH12_RET_NULL NULL 	Pointer detected
 */
extern LIS2DH12_Ret LIS2DH12_getALLmG(int32_t* const accX, int32_t* const accY, int32_t* const accZ);

extern void gpiote_event_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action);

#ifdef __cplusplus
}
#endif

#endif  /* LIS2DH12_H */
