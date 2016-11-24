#ifndef INIT_H
#define INIT_H

/**
 *  Initialization functions for RuuviTag template application project.
 *  Functions return 0 on successful init, 1 on error. Main application should check
 *  the return values and notify user on failure
 */

#include <stdbool.h>
#include <stdint.h>

//Nordic SDK
#include "ble_advdata.h"
#include "nordic_common.h"
#include "softdevice_handler.h"
#include "app_scheduler.h"
#include "app_timer_appsh.h"
#include "nrf_drv_clock.h"
#include "nrf_gpio.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_delay.h"

//BSP
#include "bsp.h"

//Drivers
#include "LIS2DH12.h"
#include "bme280.h"

//Libraries
#include "bluetooth_core.h"

//Timers
#define APP_TIMER_PRESCALER             RUUVITAG_APP_TIMER_PRESCALER      /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_OP_QUEUE_SIZE         RUUVITAG_APP_TIMER_OP_QUEUE_SIZE  /**< Size of timer operation queues. */
// Scheduler settings                                         
#define SCHED_MAX_EVENT_DATA_SIZE       MAX(APP_TIMER_SCHED_EVT_SIZE, sizeof(nrf_drv_gpiote_pin_t))
#define SCHED_QUEUE_SIZE                10

#define ERROR_BLINK_INTERVAL 250u   //toggle interval of error led
#define INIT_OK_DELAY        3000u  //delay after successful init

/**
 * Initialize logging
 *
 * This function initializes logging peripherals, and must be called before using NRF_LOG().
 * Exact functionality depends on defines at sdk_config.h.
 * 
 * 
 * 
 * @return 0          Operation successful
 * @retval 1          Something went wrong
 *
 */
uint8_t init_log(void);

/**
 * Initialize ble stack
 *
 * This function initializes BLE stack, related timers and clock sources.
 * Most of the functions of RuuviTag require BLE stack initialization for timers
 * and clocks even if the do not use BLE functionality
 *  
 * 
 * @return 0          Operation successful
 * @retval 1          Something went wrong
 *
 */
uint8_t init_ble(void);

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
 * @return 0          Operation successful
 * @retval 1          Something went wrong
 *
 */
uint8_t init_timer(app_timer_id_t main_timer_id, uint32_t main_interval, void (*timer_handler)(void *));

/**
 * Initialize leds
 *
 * This function initializes GPIO for leds
 * 
 * @return 0          Operation successful
 * @retval 1          Something went wrong
 *
 */
uint8_t init_leds(void);

/**
 * Initialize buttons
 *
 * This function initializes GPIO for buttons
 * TODO: event / interrupt driven button reading 
 *
 * @return 0          Operation successful
 * @retval 1          Something went wrong
 *
 */
uint8_t init_buttons(void);

/**
 * Initialize sensors
 *
 * This function initializes the sensor drivers.
 * It should be called even if sensors are not used, 
 * since initialization will put sensors in low-power mode
 *
 * @return 0          Operation successful
 * @retval 1          Something went wrong
 *
 */
uint8_t init_sensors(void);

/**
 * Display init status
 *
 * This function checks the init status from previous
 * init operations, and blinks the led in infinite loop
 * if there was error.
 * 
 * @param init_status number of errors occured in init, 0 on successful init.
 */
void init_blink_status(uint8_t init_status);

#endif
