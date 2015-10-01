/*
 * RuuviTag Demo / Tester
 *
 * Copyright (c) 2015, Offcode Ltd. All rights reserved.
 * Author: Janne Rosberg <janne@offcode.fi>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *    * Redistributions of source code must retain the above copyright
 *      notice,   this list of conditions and the following disclaimer.
 *    * Redistributions in  binary form must  reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the name of the RuuviTag nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND  ANY  EXPRESS  OR  IMPLIED WARRANTIES,  INCLUDING,  BUT NOT LIMITED TO,
 * THE  IMPLIED  WARRANTIES  OF MERCHANTABILITY  AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT,  INDIRECT,  INCIDENTAL,  SPECIAL,  EXEMPLARY, OR
 * CONSEQUENTIAL  DAMAGES  (INCLUDING,  BUT  NOT  LIMITED  TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES;  LOSS OF USE, DATA, OR PROFITS;  OR BUSINESS
 * INTERRUPTION)  HOWEVER CAUSED AND  ON ANY THEORY OF LIABILITY,  WHETHER IN
 * CONTRACT,  STRICT LIABILITY,  OR TORT  (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */


//======[ INCLUDES ]===========================================================
#include "rwip_config.h"             // SW configuration
#include "co_math.h"
#include "app.h"
#include "app_api.h"

#ifdef CFG_SPI_FLASH
#include "spi_flash.h"
#include "periph_setup.h"
#endif

#include "app_ruuvitag_proj.h"
#include "periph_setup.h"
#include "da14580_config.h"

extern bool sys_startup_flag;

#if (BLE_SPOTA_RECEIVER == 1)
uint8_t spotar_status __attribute__((section("retention_mem_area0"),zero_init));
#endif 

uint8_t  app_advertise_mode	__attribute__((section("retention_mem_area0"),zero_init));
uint16_t app_advertise_counter	__attribute__((section("retention_mem_area0"),zero_init));
uint8_t  app_last_ble_event	__attribute__((section("retention_mem_area0"),zero_init));


//======[ PUBLIC FUNCTIONS ]===================================================

/**
 * @brief app_api function. Project's actions in app_init
 */
void app_init_func(void)
{
	// beacon only
#if defined(EXAMPLE_IBEACON) || defined (EXAMPLE_ALTBEACON)
	app_advertise_mode = GAPM_ADV_NON_CONN;
#else
	// services...
	app_advertise_mode = GAPM_ADV_UNDIRECT;
#endif

#if (BLE_BATT_SERVER)
	app_batt_init();
#endif

#if (BLE_SPOTA_RECEIVER)
	spotar_status = SPOTAR_END;
	app_spotar_init(app_spotar_status);
#endif


#if (EXT_SLEEP_ENABLED)
	app_set_extended_sleep();
#elif (DEEP_SLEEP_ENABLED)
	app_set_deep_sleep();
#else
	app_disable_sleep();
#endif

}

/**
 * @brief actions in app_sec_init during system initialization
 */
void app_sec_init_func(void)
{

#if (BLE_APP_SEC)
	// setup required auth mode.
	app_sec_env.auth = (GAP_AUTH_REQ_NO_MITM_BOND);
#endif

}

/**
 * @brief actions in app_connect (connection complete event from the GAP)
 *
 * @param param received gapc_connection_req_ind
 */
void app_connection_func(struct gapc_connection_req_ind const *param)
{

	return;
}

/**
 * @brief Handles reception of gapm_adv_report_ind msg.
 * @param param gapm_adv_report_ind message
 */
void app_adv_report_ind_func(struct gapm_adv_report_ind *param)
{
	(void) param;
}

/**
 * @brief Handles scan procedure completion.
 */
void app_scanning_completed_func(void)
{

}

/**
 * @brief Handles connection request failure.
 */
void app_connect_failed_func(void)
{

}


/**
 * @brief app_connect() hook.
 * called before msg is send to GAPM
 * Used in central role only.
 *
 * @param msg gapm_start_connection_cmd
 */
void app_connect_func(struct gapm_start_connection_cmd *msg)
{

}


/**
 * @brief Setup advertise data.
 * called from app_adv_start()
 *
 * @param cmd struct to fill-in
 */
void app_adv_func(struct gapm_start_advertise_cmd *cmd)
{
	//  Start advertising. Fill GAPM_START_ADVERTISE_CMD message

	//  Device Name Length
	uint8_t device_name_length;
	int8_t  device_name_avail_space;
	uint8_t device_name_temp_buf[64];

	cmd->op.code     = app_advertise_mode;
	cmd->op.addr_src = GAPM_PUBLIC_ADDR;

	cmd->intv_min    = APP_BEACON_INT_MIN;
	cmd->intv_max    = APP_BEACON_INT_MAX;
	cmd->channel_map = APP_ADV_CHMAP;
	cmd->info.host.mode = GAP_GEN_DISCOVERABLE; //General Discoverable Mode

	// set static Advertising Data
	cmd->info.host.adv_data_len = APP_ADV_DATA_LEN;
	memcpy(&cmd->info.host.adv_data[0], APP_ADV_DATA, cmd->info.host.adv_data_len);

	// Add dynamic data fill here...

	// Scan Response Data
	cmd->info.host.scan_rsp_data_len = APP_SCNRSP_DATA_LENGTH;
	memcpy(&cmd->info.host.scan_rsp_data[0], APP_SCNRSP_DATA, cmd->info.host.scan_rsp_data_len);

	// add "CompleteLocalName" 0x09 entry if free space available

	// Get remaining space in the Advertising Data - 2 bytes are used for name length/flag
	device_name_avail_space = APP_ADV_DATA_MAX_SIZE - cmd->info.host.adv_data_len - 2;

	// Check if data can be added to the Advertising data
	if (device_name_avail_space > 0) {
		// Get default Device Name (No name if not enough space)
		device_name_length = strlen(APP_DEVICE_NAME);
		memcpy(&device_name_temp_buf[0], APP_DEVICE_NAME, device_name_length);

		if(device_name_length > 0) {
			// Check available space
			device_name_length = co_min(device_name_length, device_name_avail_space);
			// Fill Length
			cmd->info.host.adv_data[cmd->info.host.adv_data_len]     = device_name_length + 1;
			// Fill Device Name Flag
			cmd->info.host.adv_data[cmd->info.host.adv_data_len + 1] = '\x09';
			// Copy device name
			memcpy(&cmd->info.host.adv_data[cmd->info.host.adv_data_len + 2], device_name_temp_buf, device_name_length);

			// Update Advertising Data Length
			cmd->info.host.adv_data_len += (device_name_length + 2);
		}
	}

	return;
}

/**
 * @brief actions in app_disconnect
 * @param task_id app task id
 * @param param Received gapc_disconnect_ind msg.
 */
void app_disconnect_func(ke_task_id_t task_id, struct gapc_disconnect_ind const *param)
{

}    

/**
 * @brief actions for profiles database initialization
 * @return true/false Indicate if more services need to be added in the database
 */
bool app_db_init_func(void)
{
	// Indicate if more services need to be added in the database
	bool end_db_create = true;


	return end_db_create;
}

/**
 * @brief project configures GAPM.
 * Called upon reset completion
 *
 * @param task_id  id of the kernel task calling this function
 * @param cmd      parameters to pass to the stack
 */
void app_configuration_func(ke_task_id_t const task_id, struct gapm_set_dev_config_cmd *cmd)
{
	// Create GAPM_SET_DEV_CONFIG message to configure BLE stack

	// set device configuration
	cmd->operation = GAPM_SET_DEV_CONFIG;
	// Device Role
	cmd->role = GAP_PERIPHERAL_SLV;

	// Device Appearance
	cmd->appearance = 0x0000;

	// Device Appearance write permission requirements for peer device
	cmd->appearance_write_perm = GAPM_WRITE_DISABLE;
	// Device Name write permission requirements for peer device
	cmd->name_write_perm = GAPM_WRITE_DISABLE;

	// Maximum trasmit unit size
	cmd->max_mtu = 23;

	// Peripheral only:
	// Slave preferred Minimum of connection interval
	cmd->con_intv_min = 8;         // 10ms (8*1.25ms)
	// Slave preferred Maximum of connection interval
	cmd->con_intv_max = 16;        // 20ms (16*1.25ms)
	// Slave preferred Connection latency
	cmd->con_latency  = 0;
	// Slave preferred Link supervision timeout
	cmd->superv_to    = 100;

	// Privacy settings bit field
	cmd->flags = 0;

	return;
}

/**
 * @brief app_api function. Called upon device's configuration completion
 *
 * Handle device configuration complete event.
 * Start required Profiles' Database procedure.
 */
void app_set_dev_config_complete_func(void)
{
	// We are now in Initialization State
	ke_state_set(TASK_APP, APP_DB_INIT);

	// Create database
	if (app_db_init()) {
		// No more service to add in the database, start application
		app_db_init_complete_func();
	}

#if 0
	// Example app message use
	board_led_on(false);
	app_timer_set(APP_RT_ALIVE, TASK_APP, 100);
#endif
	return;
}

/**
 * @brief called upon connection param's update rejection
 * @see gapc_cmp_evt_handler()
 *
 * @param status error code
 */
void app_update_params_rejected_func(uint8_t status)
{
	return;
}

/**
 * @brief called upon connection param's update completion
 * @see gapc_cmp_evt_handler()
 */
void app_update_params_complete_func(void)
{
	return;
}

/**
 * @brief Handles undirect advertising completion.
 * @param status command complete message status
 */
void app_adv_undirect_complete(uint8_t status)
{
	if (status == GAP_ERR_CANCELED) {
		app_adv_start();
	}

	return;
}

/**
 * @brief Handles direct advertising completion.
 * @param status  command complete message status
 */
void app_adv_direct_complete(uint8_t status)
{
	return;
}

/**
 * @brief handles database creation. start application.
 */
void app_db_init_complete_func(void)
{
	// Database created. Ready to start Application i.e. start advertise

	app_adv_start();

	return;
}

#if (BLE_APP_SEC)
void app_send_pairing_rsp_func(struct gapc_bond_req_ind *param)
{
	return;
}

void app_send_tk_exch_func(struct gapc_bond_req_ind *param)
{
	return;
    
}

void app_send_irk_exch_func(struct gapc_bond_req_ind *param)
{
	return;
}

void app_send_csrk_exch_func(struct gapc_bond_req_ind *param)
{
	return;
}

void app_send_ltk_exch_func(struct gapc_bond_req_ind *param)
{
	return;
}

void app_paired_func(void)
{
	return;
}

bool app_validate_encrypt_req_func(struct gapc_encrypt_req_ind *param)
{
	return true;
}

void app_sec_encrypt_ind_func(void)
{
	return;
}

void app_sec_encrypt_complete_func(void)
{
	return;
}

void app_mitm_passcode_entry_func(ke_task_id_t const src_id, ke_task_id_t const dest_id)
{
	return;
}
#endif //BLE_APP_SEC


//======[ APPICATION HANDLERS ]=================================================

#if 0
int app_rt_alive_handler(ke_msg_id_t const msgid, void *param,
			 ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
	static int _delay = 10;
	system_alive_indication();
	app_timer_set(APP_RT_ALIVE, TASK_APP, _delay);

	if (_delay == 100)
		_delay = 10;
	else
		_delay = 100;

	return (KE_MSG_CONSUMED);
}

#endif

/// @} APP
