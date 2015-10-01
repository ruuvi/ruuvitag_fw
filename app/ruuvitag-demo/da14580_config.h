/**
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

#ifndef DA14580_CONFIG_H_
#define DA14580_CONFIG_H_

#include "da14580_stack_config.h"

#define CFG_APP  

//#define ENABLE_DEBUG_BREAK

#ifdef ENABLE_DEBUG_BREAK
#define DEBUG_BREAK() __asm("BKPT #0\n")
#else
#define DEBUG_BREAK()
#endif


#define CFG_APP_RUUVITAG
// also Define BLE_APP_RUUVITAG so we don't need to change rwip_config.h
#define BLE_APP_RUUVITAG 1

#define HW_CONFIG_RUUVITAG_A1		1
#define HW_CONFIG_RUUVITAG_B1		2

#ifndef CFG_HW_CONFIG
#define CFG_HW_CONFIG                   HW_CONFIG_RUUVITAG_A1
#endif

#if (CFG_HW_CONFIG == HW_CONFIG_RUUVITAG_A1)
  //#define CFG_SPI_FLASH
  //#define SENSOR_ACC_ADXL363_ENABLED
  #define CFG_CONNECTABLE_ADV_MODE
  #define CFG_SPOTAR_I2C_DISABLE
  #define CFG_SPOTAR_PATCH_DISABLE

  #ifdef CFG_SPI_FLASH
    // pin defines for Flash storage module
    #define SPI_GPIO_PORT GPIO_PORT_0
    #define SPI_CS_PIN    GPIO_PIN_3
    //#define CFG_PRF_SPOTAR
  #endif
  #ifdef CFG_PRF_SPOTAR
    //#define SPOTAR_PATCH_AREA   1   // Placed in the RetRAM when SPOTAR_PATCH_AREA is 0 and in SYSRAM when 1
    //#define CFG_APP_SPOTAR      1
  #endif

#endif

// Not used but NEED to be defined
#define I2C_EEPROM_SIZE	0
#define I2C_EEPROM_PAGE	128

// NVDS structure
#undef CFG_NVDS

// BLE Security
#undef CFG_APP_SEC

// Coarse calibration
#define CFG_LUT_PATCH

// Watchdog
#undef CFG_WDOG

// Sleep modes
#define nCFG_EXT_SLEEP
#undef CFG_DEEP_SLEEP  

// max user connections
#define BLE_CONNECTION_MAX_USER 1

// Build type: 0: code at OTP, 1: code via JTAG
#define DEVELOPMENT_DEBUG 1

// boot from OTP memory
#undef APP_BOOT_FROM_OTP

//
#undef READ_NVDS_STRUCT_FROM_OTP

/**
 * Low power clock selection
 * 0x00: XTAL32, 0xAA: RCX20, 0xFF: Select from OTP Header
 */
#define CFG_LP_CLK		(0xAA)

// Fab Calibration - Must be defined for calibrated devices
#define CFG_CALIBRATED_AT_FAB

// Use a default trim value for XTAL16M if a trim value has
// not been programmed in OTP
#undef CFG_USE_DEFAULT_XTAL16M_TRIM_VALUE_IF_NOT_CALIBRATED

#if defined(CFG_EXT_SLEEP) || !defined(CFG_DEEP_SLEEP)
// 0: keep in RetRAM, 1: re-init is required (set to 0 when Extended Sleep is used)
#define REINIT_DESCRIPT_BUF	0
#define USE_MEMORY_MAP		EXT_SLEEP_SETUP

#else
/* The following are valid for BLE_CONNECTION_MAX_USER == 1.
 * Please use the results of the excel tool!
 */
//0: keep in RetRAM, 1: re-init is required (set to 0 when Extended Sleep is used)
#define REINIT_DESCRIPT_BUF     0
#define USE_MEMORY_MAP          DEEP_SLEEP_SETUP
#define DB_HEAP_SZ              1024
#define ENV_HEAP_SZ             328
#define MSG_HEAP_SZ             1312
#define NON_RET_HEAP_SZ         1024
#endif

#endif // DA14580_CONFIG_H_
