#ifndef INIT_H
#define INIT_H

/**
 *  Initialization functions for RuuviTag project.
 *  Functions return 0 on successful init, error code otherwise. 
 *  Main application should check
 *  the return values and notify user on failure
 */

#include <stdbool.h>
#include <stdint.h>

#include "app_scheduler.h"
#include "app_timer_appsh.h"
#include "ruuvi_endpoints.h"
#include "watchdog.h"

//Timers
/** PRESCALER: will be written to the RTC1 PRESCALER register. 
 *  This determines the time resolution of the timer, and thus the amount of time it can count before it wrap around. 
 *  On the nRF52 the RTC is a 24-bit counter with a 12 bit prescaler that run on the 32.768 LFCLK. 
 *  The counter increment frequency (tick rate) fRTC [kHz] = 32.768/(PRESCALER+1). For example, a prescaler value of 15 means that the tick rate or time resolution is 
 *  32.768 kHz * 1/(15+1) = 2.048 kHz and the timer will wrap around every (2^24) * 1/2.048 kHz = 8192 s. 
 **/
#define RUUVITAG_APP_TIMER_PRESCALER 15 //App timer increments at 32.768 kHz
#define RUUVITAG_APP_TIMER_OP_QUEUE_SIZE 16 //16 ops in time queue max
#define APP_TIMER_PRESCALER             RUUVITAG_APP_TIMER_PRESCALER      /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_OP_QUEUE_SIZE         RUUVITAG_APP_TIMER_OP_QUEUE_SIZE  /**< Size of timer operation queues. */
// Scheduler settings                                         
#define SCHED_MAX_EVENT_DATA_SIZE       MAX(APP_TIMER_SCHED_EVT_SIZE, sizeof(ruuvi_standard_message_t))
#define SCHED_QUEUE_SIZE                RUUVITAG_APP_TIMER_OP_QUEUE_SIZE

#define ERROR_BLINK_INTERVAL 250u   //toggle interval of error led
#define INIT_OK_DELAY        3000u  //delay after successful init


/**
 *  Error code returned from initialization
 */
typedef enum
{
  INIT_SUCCESS             = 0,  // Ok
  INIT_ERR_SEQUENCE        = 1,  // Something else should have been initialised first
  INIT_ERR_NOT_IMPLEMENTED = 2,  // Tried to init something that is not implemented
  INIT_ERR_NO_RESPONSE     = 4,  // No response from sensor. Not populated, HW fault?
  INIT_ERR_SELFTEST        = 8,  // Self test failed
  INIT_ERR_INVALID         = 16, // Invalid configuration
  INIT_ERR_UNKNOWN         = 128 // Unknown error
}init_err_code_t;

/**
 * Initialize logging
 *
 * This function initializes logging peripherals, and must be called before using NRF_LOG().
 * Exact functionality depends on defines at sdk_config.h.
 *
 */
init_err_code_t init_log(void);

/**
 * Initialize ble stack
 *
 * This function initializes BLE stack, related timers and clock sources.
 * Most of the functions of RuuviTag require BLE stack initialization for timers
 * and clocks even if the do not use BLE functionality
 *
 */
init_err_code_t init_ble(void);

/**
 *  Initialize and enable watchdog. After calling this function
 *  watchdog_feed() must be called at interval defined by sdk_config.
 *  If NULL is given as a handler, default handler which prints error
 *  log is used.
 */
init_err_code_t init_watchdog(watchdog_event_handler_t handler);

/**
 * Initialize NFC driver
 *  
 * Puts NFC on standby, ready to transmit ID of the tag.
 * 
 * @return 0          Operation successful
 * @retval 1          Something went wrong
 *
 */
init_err_code_t init_nfc(void);

/**
 * Initialize timers
 *
 * This function initializes timers used by main application.
 * Requires low-frequency clock source initialized by BLE init
 *
 * @param main_timer_id pointer to timer data structure
 * @param main_interval Interval at which the main loop should be run in ms
 * @param timer_handler function to be called at main interval
 *
 */
init_err_code_t init_timer(app_timer_id_t main_timer_id, uint32_t main_interval, void (*timer_handler)(void *));

/**
 * Initialize leds
 *
 * This function initializes GPIO for leds
 *
 */
init_err_code_t init_leds(void);

/**
 * Initialize buttons
 *
 * This function initializes GPIO for buttons
 * TODO: event / interrupt driven button reading 
 *
 */
init_err_code_t init_buttons(void);

/**
 * Initialize sensors
 *
 * This function initializes the sensor drivers.
 * It should be called even if sensors are not used, 
 * since initialization will put sensors in low-power mode
 *
 */
init_err_code_t init_sensors(void);

/**
 *
 */
init_err_code_t init_lis2dh12(void);

/**
 * Initialise BME280. Sensor will be put to sleep mode, with all sensors 
 * in oversampling x1 mode
 *
 */
init_err_code_t init_bme280(void);

/**
 * Initialise PWM channels
 */
init_err_code_t init_pwm();

/**
 * Display init status
 *
 * This function checks the init status from previous
 * init operations, and blinks the led in infinite loop
 * if there was error.
 * 
 * @param init_status number of errors occured in init, 0 on successful init.
 */
init_err_code_t init_blink_status(uint8_t init_status);

#endif
