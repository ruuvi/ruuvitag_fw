#ifndef BLUETOOTH_CORE_H
#define BLUETOOTH_CORE_H

/**
 * Interface for using Nordic SDKs Bluetooth core functions
 * such as init, advertise etc.
 */

#include <stdbool.h>
#include <stdint.h>
#include "ble_advdata.h"
#include "bluetooth_config.h"
#include "nordic_common.h"
#include "softdevice_handler.h"
#include "app_timer.h"
#include "bsp.h"

/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 * @return error code from BLE stack initialization, NRF_SUCCESS if init was ok
 */
uint32_t bluetooth_stack_init(void);

/**@brief Function for the Peer Manager initialization.
 *
 * @param[in] erase_bonds  Indicates whether bonding information should be cleared from
 *                         persistent storage during initialization of the Peer Manager.
 */
void peer_manager_init(bool erase_bonds);

uint32_t bluetooth_set_name(const char* name_base, size_t name_length);

/**
 *  Starts advertising with previously setup data and parameters.
 *  Can be called again while advertising to update parameters
 *
 */
uint32_t bluetooth_advertising_start(void);

/**
 */
uint32_t bluetooth_advertising_stop(void);

/**
 * @brief Function to setsBLE transmission power
 *  
 * @details set the BLE transmission power in dBm
 * @param int8_t power power in dBm, must be one of -40, -30, -20, -16, -12, -8, -4, 0, 4
 * @return error code, 0 if operation was success.
 */
uint32_t ble_tx_power_set(int8_t power);

/**@brief Initialize advertising parameters. Parameters can be adjusted by calling this function again. 
 *
 * @details Initializes the BLE advertisement 
 *
 * @return error code from BLE stack initialization, NRF_SUCCESS if init was ok
 */
uint32_t bluetooth_advertising_init(void);

/**@brief Function for advertising data. 
 *
 * @details Initializes the BLE advertisement with given data as manufacturer specific data.
 * Companyt ID is included by default and doesn't need to be included.  
 *
 * @param data pointer to data to advertise, maximum length 24 bytes
 * @param length length of data to advertise
 *
 * @return error code from BLE stack initialization, NRF_SUCCESS if init was ok
 */
uint32_t bluetooth_advertise_data();

/**@brief Function adjusting advertising interval. 
 *
 * @details Sets the advertising interval in program.
 *          Does not have any effect until the next call to
 *          bluetooth_advertise_data (TODO). Default interval is used if
 *          this has not been called before 
 *
 * @param interval advertisement interval in milliseconds, 100 - 10 000 
 *
 * @return 0 if value was updated, 1 if value was outside acceptable range
 */
uint32_t set_advertising_interval(uint16_t interval);
#endif
