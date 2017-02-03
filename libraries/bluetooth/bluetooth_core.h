#ifndef BLUETOOTH_CORE_H
#define BLUETOOTH_CORE_H

/**
 * Interface for using Nordic SDKs Bluetooth core functions
 * such as init, advertise etc.
 */

#include <stdbool.h>
#include <stdint.h>
#include "nordic_common.h"
#include "softdevice_handler.h"



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

/**@brief Function for initializing the Advertising functionality.
 *
 * @details Encodes the required advertising data and passes it to the stack.
 *          Also builds a structure to be passed to the stack when starting advertising.
 *
 * @param m_beacon_info pointer to manufacturer specific data structure, for example:
 * uint8_t m_beacon_info[APP_BEACON_INFO_LENGTH] = 
 * {
 *   APP_DEVICE_TYPE,     // Manufacturer specific information. Specifies the device type in this
 *                        // implementation.
 *
 *   APP_ADV_DATA_LENGTH, // Manufacturer specific information. Specifies the length of the
 *                        // manufacturer specific data in this implementation.
 *   APP_BEACON_UUID,     // 128 bit UUID value.
 *   APP_MAJOR_VALUE,     // Major arbitrary value that can be used to distinguish between Beacons.
 *   APP_MINOR_VALUE,     // Minor arbitrary value that can be used to distinguish between Beacons.
 *   APP_MEASURED_RSSI    // Manufacturer specific information. The Beacon's measured TX power in
 *                        // this implementation.
 *  };
 *
 * @param info_size size of manufacturer specific data to advertise
 */
void advertising_init(uint8_t *m_beacon_info, uint8_t info_size);

/**@brief Function for starting advertising.
 *
 * @param connectable
 * BLE_GAP_ADV_TYPE_ADV_IND         //Connectable, undirected, not implemented
 * BLE_GAP_ADV_TYPE_ADV_DIRECT_IND  //Connectable, directed, not implemented
 * BLE_GAP_ADV_TYPE_ADV_SCAN_IND    //Scannable, undirected, not implemented
 * BLE_GAP_ADV_TYPE_ADV_NONCONN_IND //Non-connectable, undirected, not implemented
 */
void advertising_start(uint8_t connectable);

#endif
