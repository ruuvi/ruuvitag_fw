/**
 * @file app_ruuvitag_proj.h
 * @brief RuuviTag application header file.
 *
 * Copyright (C)2015. Offcode Ltd.
 *
 */

#ifndef APP_RUUVITAG_PROJ_H_
#define APP_RUUVITAG_PROJ_H_

//======[ INCLUDES ]===========================================================
#include "rwble_config.h"
#include "app_task.h"                  // application task
#include "gapc_task.h"                 // gap functions and messages
#include "gapm_task.h"                 // gap functions and messages
#include "app.h"                       // application definitions
#include "co_error.h"                  // error code definitions
#include "smpc_task.h"                 // error code definitions

#if (BLE_SPOTA_RECEIVER)
#include "app_spotar.h"
#include "app_spotar_task.h"
#endif 

//======[ TYPE DEFINITIONS ]===================================================


//======[ DEFINES ]============================================================

// App Messages

#define DEFINE_APP_MSG_HANDLER(msg, handler) {msg, (ke_msg_func_t) handler},

// APP mesasges can be defined here (no need to change app_api.h)
//
#define APP_RUUVITAG_MESSAGE_DEFINES \
        APP_RT_ALIVE, \
        APP_RT_MSG2,

#define APP_MESSAGE_HANDLERS \
        //DEFINE_APP_MSG_HANDLER(APP_RT_ALIVE, app_rt_alive_handler)



#define SWAP(num) ((num>>8) | (num<<8))
#define APP_DEVICE_NAME "RuuviTag"

#define EXAMPLE_IBEACON
//#define EXAMPLE_ALTBEACON

#ifdef EXAMPLE_IBEACON
// iBeacon example
#define APP_ADV_DATA            "\x1A\xFF\x4c\x00\x02\x15" \
                                "\x97\xCD\xB4\xB2\xBB\xC4\x42\x96\xBC\x9C\x19\xA3\x98\x7D\x73\xC7" \
                                "\x00\x01\x00\x02\xC7"
#define APP_ADV_DATA_LEN        (27)
#define APP_SCNRSP_DATA         "\x09\x09RuuviTag"
#define APP_SCNRSP_DATA_LENGTH  (10)

#elif defined (EXAMPLE_ALTBEACON)
// AltBeacon example
#define APP_ADV_DATA            "\x1B\xFF\xFF\xFF\xBE\xAC" \
                                "\x97\xCD\xB4\xB2\xBB\xC4\x42\x96\xBC\x9C\x19\xA3\x98\x7D\x73\xC7" \
                                "\x43\x21\x87\x65\xC7\x5A"
#define APP_ADV_DATA_LEN        (28)
#define APP_SCNRSP_DATA         "\x09\x09RuuviTag"
#define APP_SCNRSP_DATA_LENGTH  (10)

#else
// Single service test
#define APP_ADV_DATA        "\x03\x03\xF5\xFE"

//#if (BLE_SPOTA_RECEIVER == 1)
#define APP_ADV_DATA_LEN    (4)
//#else
//#define APP_ADV_DATA_LEN    (0)
//#endif

#define APP_SCNRSP_DATA         "\x07\xFF\x00\0x04\0x00\xFF\xC0\xDE"
#define APP_SCNRSP_DATA_LENGTH  (8)

#endif


/// Advertising minimum interval
#define APP_BEACON_INT_MIN   0xA0
/// Advertising maximum interval
#define APP_BEACON_INT_MAX   300	// x * 0.625 = XX ms

//SPI Flash power off timer timeout. corresponds to 5 sec = 500 * 10 ms
#define APP_SPI_POWEROFF_TIMEOUT    500

//======[ FUNCTION DECLARATIONS ]==============================================

/**
 * @brief Handles APP_FLASH_POWEROFF_TIMER message.
 * Indicates timer expiration and powers off SPI flash
 *
 * @return If the message was consumed or not.
 */                                          
int app_flash_poweroff_timer_handler(ke_msg_id_t const msgid,
				     void *param,
				     ke_task_id_t const dest_id,
				     ke_task_id_t const src_id);

int app_restart_adv_timer_handler(ke_msg_id_t const msgid,
				  void *param,
				  ke_task_id_t const dest_id,
				  ke_task_id_t const src_id);

int app_rt_alive_handler(ke_msg_id_t const msgid, void *param,
			 ke_task_id_t const dest_id, ke_task_id_t const src_id);


#endif /* APP_RUUVITAG_PROJ_H_ */
