/*
 * application sleep hooks.
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

#include <stdint.h>
#include "arch.h"
#include "app.h"
#include "gpio.h"
#include "ke_event.h"
#include "app.h"
#include "gapm_task.h"
#include "app.h"

// for ble_advtim_set()
#include <reg_blecore.h>

extern uint8_t app_last_ble_event;
extern uint16_t app_advertise_counter;

/**
 * @brief Used for sending messages to kernel tasks generated from
 * asynchronous events that have been processed in app_asynch_proc
 *
 * @return true to force calling of schedule(), else false
 */
static inline bool app_asynch_trm(void)
{
	bool ret = false;
	uint8_t temp_last_ble_event;

	temp_last_ble_event = app_last_rwble_evt_get();

	if (app_last_ble_event != temp_last_ble_event) {
		app_last_ble_event = temp_last_ble_event;

		if (app_last_ble_event == BLE_EVT_END) {
			// we came here after BLE is done it's things...
			// ie after advertise send
			app_advertise_counter++;
			if (app_advertise_counter == 1) {
				system_alive_indication();
			} else if (app_advertise_counter == 2) {
				system_alive_indication();
			}

			if (app_advertise_counter == 10)
				app_advertise_counter = 0;
		}
	}

	return ret;
}


/**
 * @brief Used for processing of asynchronous events at user level.
 *
 * The corresponding ISRs should be kept as short as possible and the
 * remaining processing should be done at this point.
 *
 * @return true to force calling of schedule(), else false
 */
static inline bool app_asynch_proc(void)
{
	bool ret = false;

	return ret;
}


/**
 * @brief Used for updating the state of the application just before
 * sleep checking starts.
 */
static inline void app_asynch_sleep_proc(void)
{
	return;
}


/**
 * @brief Used to disallow extended or deep sleep based on the current
 * application state. BLE and Radio are still powered off.

 * @param sleep_mode Sleep Mode
 */
static inline void app_sleep_prepare_proc(sleep_mode_t *sleep_mode)
{
	return;
}


/**
 * @brief Used for application specific tasks just before entering
 * the low power mode.
 *
 * @param sleep_mode
 */
static inline void app_sleep_entry_proc(sleep_mode_t *sleep_mode)
{
	return;
}


/**
 * @brief Used for application specific tasks immediately after exiting
 * the low power mode.
 *
 * @param sleep_mode
 */
static inline void app_sleep_exit_proc(sleep_mode_t sleep_mode)
{
	return;
}

