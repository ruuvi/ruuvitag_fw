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
ret_code_t bluetooth_stack_init(void);

/**@brief Function for the Peer Manager initialization.
 *
 * @param[in] erase_bonds  Indicates whether bonding information should be cleared from
 *                         persistent storage during initialization of the Peer Manager.
 */
void peer_manager_init(bool erase_bonds);

ret_code_t bluetooth_set_name(const char* name_base, size_t name_length);

/**
 *  Starts advertising with previously setup data and parameters.
 *  Can be called again while advertising to update parameters
 *
 */
ret_code_t bluetooth_advertising_start(void);

/**
 */
ret_code_t bluetooth_advertising_stop(void);

/**
 * @brief Function to setsBLE transmission power
 *  
 * @details set the BLE transmission power in dBm
 * @param int8_t power power in dBm, must be one of -40, -30, -20, -16, -12, -8, -4, 0, 4
 * @return error code, 0 if operation was success.
 */
ret_code_t bluetooth_tx_power_set(int8_t power);

/**@brief Function for advertising ḿanufacturer specific data. 
 *
 * @details Initializes the BLE advertisement with given data as manufacturer specific data.
 * Company ID is included by default and doesn't need to be included.
 * Every other data filed is overwritten
 *
 * @param data pointer to data to advertise, maximum length 24 bytes
 * @param length length of data to advertise
 *
 * @return error code from BLE stack, NRF_SUCCESS if operation was ok
 */
ret_code_t bluetooth_set_manufacturer_data(uint8_t* data, size_t length);

/**
 *  Updates bluetooth configuration
 */
ret_code_t bluetooth_apply_configuration();

 /**
 * @brief Function adjusting advertising interval. 
 * @details Sets the advertising interval in program. takes effect after calling bluetooth_apply_configuration();
 * @param interval advertisement interval in milliseconds, 100 - 10 000 
 *
 * @return NRF_SUCCESS on valid interval
 */
ret_code_t bluetooth_configure_advertising_interval(uint16_t interval);

 /**
 * @brief Function for configuring advertisement type
 * @details https://infocenter.nordicsemi.com/index.jsp?topic=%2Fcom.nordic.infocenter.s132.api.v3.0.0%2Fgroup___b_l_e___g_a_p___a_d_v___t_y_p_e_s.html
 * @param type Advertisement type, 0 ... 3
 *
 * @return NRF_SUCCESS on valid interval
 */
ret_code_t bluetooth_configure_advertisement_type(uint8_t type);


/**
 * Set Eddystone URL advertisement package in advdata. Must be applied with bluetooth_apply_configuration()
 * 
 * @param url_buffer character array containing new URL. May contain eddystone
 *        shortcuts, such as 0x03: "https://"
 * @param length length of URL to transmit. must be <18. Shortcut bytes are counted as one, i.e. https://ruu.vi is 7 bytes long
 *        as long as https:// is written as 0x03
 */
ret_code_t bluetooth_set_eddystone_url(char* url_buffer, size_t length);
#endif
