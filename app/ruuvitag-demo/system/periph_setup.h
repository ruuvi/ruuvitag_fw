/*
 * RuuviTag peripheral setup.
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
#include "global_io.h"
#include "arch.h"

#ifdef CFG_SPI_FLASH
#include "spi_flash.h"
#include "spi.h"
#endif

//======[ DEFINES ]============================================================

#ifdef SENSOR_ACC_ADXL363_ENABLED
#include "spi.h"
#include "adxl363_driver.h"
#endif

#ifdef SENSOR_ACC_LIS331D_ENABLED

#endif

// SPI Flash configuration
#ifdef CFG_SPI_FLASH

// SPI Flash options
#if (CFG_HW_CONFIG == HW_CONFIG_RUUVITAG_A1)
// A1 - W25Q80
#define SPI_FLASH_SIZE 0x40000	// SPI Flash memory size in bytes
#define SPI_FLASH_PAGE 256	// SPI Flash memory page size in bytes
#endif

// for app_config_storage.c
#define SPI_SECTOR_SIZE         4096

#endif /* CFG_SPI_FLASH */


#if (CFG_HW_CONFIG == HW_CONFIG_RUUVITAG_A1)
//#define WKUP_ENABLED
#endif

#define SPI_FLASH_CS_PORT	GPIO_PORT_0
#define SPI_FLASH_CS_PIN	GPIO_PIN_3

//======[ FUNCTION DECLARATIONS ]==============================================

void periph_init(void);
void GPIO_reservations(void);
void board_led_on(bool on);
