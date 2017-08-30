/**
@addtogroup LIS2DH12Driver LIS2DH12 Acceleration Sensor Driver
@{
@file       LIS2DH12.h

Hardware Driver for the LIS2DH12 Acceleration Sensor

* @}
***************************************************************************************************/
#ifndef LIS2DH12_H
#define LIS2DH12_H

/* INCLUDES ***************************************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include "app_error.h"
#include "app_scheduler.h"
#include "nordic_common.h"
#include "app_timer_appsh.h"
#include "lis2dh12_registers.h"

/* CONSTANTS **************************************************************************************/
#define LIS2DH12_FIFO_MAX_LENGTH 32

/* TYPES ******************************************************************************************/
/** Structure containing sensor data for all 3 axis */
typedef struct __attribute__((packed))
{
    int16_t x;
    int16_t y;
    int16_t z;
} acceleration_t;

/** Union to split raw data to values for each axis */
typedef union
{
  uint8_t raw[sizeof(acceleration_t)];
  acceleration_t sensor;
}lis2dh12_sensor_buffer_t;

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
} lis2dh12_ret_t;


/** Available Scales */
typedef enum{
	LIS2DH12_SCALE2G = LIS2DH12_FS_2G,  /**< Scale Selection: +/- 2g */
	LIS2DH12_SCALE4G = LIS2DH12_FS_4G,	/**< Scale Selection: +/- 4g */
	LIS2DH12_SCALE8G = LIS2DH12_FS_8G,	/**< Scale Selection: +/- 8g */
	LIS2DH12_SCALE16G = LIS2DH12_FS_16G	/**< Scale Selection: +/- 16g */
}lis2dh12_scale_t;

/** Available Resolutions */
typedef enum{
	LIS2DH12_RES8BIT = 8,		/**< 8 extra bits */
	LIS2DH12_RES10BIT = 6,		/**< 6 extra bits */
	LIS2DH12_RES12BIT = 4		/**< 4 extra bits */
}lis2dh12_resolution_t;

/** Available sample rates */
typedef enum{
	LIS2DH12_RATE_0   = 0,		/**< Power down */
	LIS2DH12_RATE_1   = 1<<4,	/**< 1 Hz */
	LIS2DH12_RATE_10  = 2<<4,	/**< 10 Hz*/
	LIS2DH12_RATE_25  = 3<<4,		
    LIS2DH12_RATE_50  = 4<<4,		
    LIS2DH12_RATE_100 = 5<<4,		
    LIS2DH12_RATE_200 = 6<<4,		
    LIS2DH12_RATE_400 = 7<<4    /** 1k+ rates not implemented */		
}lis2dh12_sample_rate_t;

/** Available modes */
typedef enum{
    LIS2DH12_MODE_BYPASS = 0,                                              /**< FIFO off */
    LIS2DH12_MODE_FIFO   = LIS2DH12_FM_FIFO,                               /**< FIFO on */
    LIS2DH12_MODE_STREAM = LIS2DH12_FM_STREAM,                             /**< Stream on */
    LIS2DH12_MODE_STREAM_TO_FIFO = (LIS2DH12_FM_FIFO | LIS2DH12_FM_STREAM) /**< Stream until trigger */
}lis2dh12_fifo_mode_t;

/** Data Ready Event Callback Type */
typedef void (*LIS2DH12_drdy_event_t)(void);

/* PROTOTYPES *************************************************************************************/

/**
 *  Initializes LIS2DH12, and puts it in sleep mode.
 *  
 */
lis2dh12_ret_t lis2dh12_init(void);

/**
 *  
 */
lis2dh12_ret_t lis2dh12_set_scale(lis2dh12_scale_t scale);

/**
 *
 */
lis2dh12_ret_t lis2dh12_set_resolution(lis2dh12_resolution_t resolution);

/**
 *
 */
lis2dh12_ret_t lis2dh12_set_sample_rate(lis2dh12_sample_rate_t sample_rate);

/**
 *
 */
lis2dh12_ret_t lis2dh12_set_fifo_mode(lis2dh12_fifo_mode_t mode);

/**
 *
 */
lis2dh12_ret_t lis2dh12_read_samples(lis2dh12_sensor_buffer_t* buffer, size_t count);

/**
 *
 */
lis2dh12_ret_t lis2dh12_get_fifo_sample_number(size_t* count);

/**
 *
 */
lis2dh12_ret_t lis2dh12_set_fifo_watermark(size_t count);

lis2dh12_ret_t lis2dh12_read_register(uint8_t address, uint8_t* const p_toRead, size_t count);
lis2dh12_ret_t lis2dh12_write_register(uint8_t address, uint8_t* const dataToWrite, size_t count);

#endif  /* LIS2DH12_H */
