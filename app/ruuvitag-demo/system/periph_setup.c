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
#include "rwip_config.h"
#include "global_io.h"
#include "gpio.h"
#include "uart.h"

#include "periph_setup.h"
#include "app_ruuvitag_proj.h"

#ifdef CFG_SPI_FLASH
int8_t detected_spi_flash_device_index;
#include "spi.h"
#include "spi_flash.h"
extern bool sys_startup_flag;
#endif

#if (BLE_SPOTA_RECEIVER)
#include "app_spotar.h"
extern uint8_t spotar_status;
#endif

#ifdef SENSOR_ACC_ADXL363_ENABLED
#include "spi.h"
#include "adxl363_driver.h"
#endif

#ifdef SENSOR_MS5637_ENABLED
#include "i2c_bus.h"
#include "ms5637_driver.h"
#endif

#if DEVELOPMENT_DEBUG
/*
* Globally reserved GPIOs reservation
*/
void GPIO_reservations(void)
{
#if (BLE_APP_PRESENT)

#if (CFG_HW_CONFIG == HW_CONFIG_RUUVITAG_A1)
	RESERVE_GPIO( SPI_CLK,      GPIO_PORT_0, GPIO_PIN_0, PID_SPI_CLK);
	RESERVE_GPIO( ACC_INT1,     GPIO_PORT_0, GPIO_PIN_1, PID_GPIO);
	RESERVE_GPIO( SPI_ACC_CS,   GPIO_PORT_0, GPIO_PIN_2, PID_SPI_EN);
	RESERVE_GPIO( SPI_FLASH_CS, GPIO_PORT_0, GPIO_PIN_3, PID_SPI_EN);
	RESERVE_GPIO( RED_LED,      GPIO_PORT_0, GPIO_PIN_4, PID_GPIO);
	RESERVE_GPIO( SPI_DI,       GPIO_PORT_0, GPIO_PIN_5, PID_SPI_DI);
	RESERVE_GPIO( SPI_DO,       GPIO_PORT_0, GPIO_PIN_6, PID_SPI_DO);
	RESERVE_GPIO( ACC_INT2,     GPIO_PORT_0, GPIO_PIN_7, PID_GPIO);

	RESERVE_GPIO( I2C_SCL,      GPIO_PORT_1, GPIO_PIN_0, PID_I2C_SCL);
	RESERVE_GPIO( I2C_SDA,      GPIO_PORT_1, GPIO_PIN_1, PID_I2C_SDA);

#elif (CFG_HW_CONFIG == HW_CONFIG_RUUVITAG_B1)
#error Not implemented!
#endif

#endif

}
#endif //DEVELOPMENT_DEBUG

/**
 * @brief Map port pins
 *
 * set gpio port function mode
 */
void set_pad_functions(void)
{
#if (CFG_HW_CONFIG == HW_CONFIG_RUUVITAG_A1)
	GPIO_ConfigurePin(GPIO_PORT_0, GPIO_PIN_0, OUTPUT, PID_SPI_CLK, false);
	GPIO_ConfigurePin(GPIO_PORT_0, GPIO_PIN_1, INPUT,  PID_GPIO,    false);
	GPIO_ConfigurePin(GPIO_PORT_0, GPIO_PIN_2, OUTPUT, PID_GPIO,    true);
	GPIO_ConfigurePin(GPIO_PORT_0, GPIO_PIN_3, OUTPUT, PID_GPIO,    true);
	GPIO_ConfigurePin(GPIO_PORT_0, GPIO_PIN_4, OUTPUT, PID_GPIO,    false);
	GPIO_ConfigurePin(GPIO_PORT_0, GPIO_PIN_5,  INPUT, PID_SPI_DI,  false);
	GPIO_ConfigurePin(GPIO_PORT_0, GPIO_PIN_6, OUTPUT, PID_SPI_DO,  false);
	GPIO_ConfigurePin(GPIO_PORT_0, GPIO_PIN_7, INPUT,  PID_GPIO,    false);
#if 1
	GPIO_ConfigurePin(GPIO_PORT_1, GPIO_PIN_0, OUTPUT, PID_I2C_SCL, false);
	GPIO_ConfigurePin(GPIO_PORT_1, GPIO_PIN_1, OUTPUT, PID_I2C_SDA, false);
#else
	// UART1 on i2c pins (P9, P10)
	GPIO_ConfigurePin(GPIO_PORT_1, GPIO_PIN_0, OUTPUT, PID_UART1_RX, false);
	GPIO_ConfigurePin(GPIO_PORT_1, GPIO_PIN_1, OUTPUT, PID_UART1_TX, false);
#endif

#endif
}

void board_led_on(bool on)
{
	if (on) {
		SetBits16(P0_SET_DATA_REG, P0_SET, 1 << 4);
	} else {
		SetBits16(P0_RESET_DATA_REG, P0_RESET, 1<<4);
	}
}


/**
 * @brief periph_init
 *
 * Enable pad's and peripheral clocks assuming that peripherals
 * power domain is down.
 */
void periph_init(void)
{
	// Power up peripherals power domain
	SetBits16(PMU_CTRL_REG, PERIPH_SLEEP, 0);
	while (!(GetWord16(SYS_STAT_REG) & PER_IS_UP))
		;

	// patch ROM funcs
	patch_func();

	// Init pads
	set_pad_functions();

	board_led_on(true);

	// Enable the pads
	SetBits16(SYS_CTRL_REG, PAD_LATCH_EN, 1);

	// autodetect available sensors
	//probe_i2c_devices();
}

/**
 * @brief system_alive_indication
 * This is for testing only.
 * LED on takes about 2mA :-(
 */
void system_alive_indication(void)
{
	static bool toggle = false;
	if (toggle)
		toggle = false;
	else
		toggle = true;

	board_led_on(toggle);
}


//======[ SENSORS ]===========================================================

#ifdef SENSOR_ACC_ADXL363_ENABLED

static /*const*/ SPI_Pad_t adxl_pads = {
	.port = GPIO_PORT_0,
	.pin  = GPIO_PIN_2
};

uint8_t adxl_read_reg(uint8_t reg)
{
	uint8_t value;

	spi_init(&adxl_pads, SPI_MODE_8BIT, SPI_ROLE_MASTER,
		 SPI_CLK_IDLE_POL_LOW, SPI_PHA_MODE_0, SPI_MINT_DISABLE,
		 SPI_XTAL_DIV_2);

	spi_cs_low();

	spi_access(ADXL_CMD_READ_REGISTER);
	spi_access(reg);
	value = spi_access(0xFF);
	spi_release();

	return value;
}

#endif /* SENSOR_ACC_ADXL363_ENABLED */

