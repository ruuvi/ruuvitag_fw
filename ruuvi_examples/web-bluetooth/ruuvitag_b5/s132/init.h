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
#include "nrf_delay.h"

//Timers
#define APP_TIMER_PRESCALER              0                                          /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_OP_QUEUE_SIZE          4                                          /**< Size of timer operation queues. */
// Scheduler settings                                         
#define SCHED_MAX_EVENT_DATA_SIZE       MAX(APP_TIMER_SCHED_EVT_SIZE, sizeof(nrf_drv_gpiote_pin_t))
#define SCHED_QUEUE_SIZE                25

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
 * @retval 0          Operation successful
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
 * @retval 0          Operation successful
 * @retval 1          Something went wrong
 *
 */
uint8_t init_ble(void);

/**@brief Function for the GAP initialization.
 *
 * @details This function sets up all the necessary GAP (Generic Access Profile) parameters of the
 *          device including the device name, appearance, and the preferred connection parameters.
 */
void gap_params_init(void);

/**@brief Function for initializing services that will be used by the application.
 */
void services_init(void);

/**@brief Function for the Peer Manager initialization.
 *
 * @param[in] erase_bonds  Indicates whether bonding information should be cleared from
 *                         persistent storage during initialization of the Peer Manager.
 */
void peer_manager_init(bool erase_bonds);

/**@brief Function for initializing the Advertising functionality.
 */
//void advertising_init(void);

/**@brief Function for initializing the Connection Parameters module.
 */
void conn_params_init(void);

/**
 * Register event handlers
 *
 * This function registers all event handlers used in application
 *
 */
void init_register_events(void);

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
 * @retval 0          Operation successful
 * @retval 1          Something went wrong
 *
 */
uint8_t init_timer(app_timer_id_t main_timer_id, uint32_t main_interval, void (*timer_handler)(void *));

/**
 * Initialize leds
 *
 * This function initializes GPIO for leds
 * 
 * @retval 0          Operation successful
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
 * @retval 0          Operation successful
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
 * @retval 0          Operation successful
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
