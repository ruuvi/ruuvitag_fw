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
uint32_t ble_stack_init(void);

/**
 * @brief Function to setsBLE transmission power
 *  
 * @details set the BLE transmission power in dBm
 * @param int8_t power power in dBm, must be one of -40, -30, -20, -16, -12, -8, -4, 0, 4
 * @return error code, 0 if operation was success.
 */
uint32_t ble_tx_power_set(int8_t power);

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
uint32_t bluetooth_advertise_data(uint8_t *data, uint8_t length);

#endif
