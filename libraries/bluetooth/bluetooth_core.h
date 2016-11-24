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

#include "nrf_log.h"
#include "nrf_log_ctrl.h"

/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 * @return error code from BLE stack initialization, NRF_SUCCESS if init was ok
 */
uint32_t ble_stack_init(void);

#endif
