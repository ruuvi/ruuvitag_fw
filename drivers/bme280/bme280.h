/*
 * BOSH BME280 driver.
 *
 * Copyright (c) 2016, Offcode Ltd. All rights reserved.
 * Author: Janne Rosberg <janne@offcode.fi>
 *
 * Reference: BST-BME280-DS001-11 | Revision 1.2 | October 2015
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

#include <stdint.h>
#include <stdbool.h>
#include "nrf.h"
#include "spi.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_drv_timer.h"
#include "nrf_drv_gpiote.h"
#include "bsp.h"
#include "app_timer.h"

struct comp_params {
	uint16_t dig_T1;
	int16_t  dig_T2;
	int16_t  dig_T3;
	uint16_t dig_P1;
	int16_t  dig_P2;
	int16_t  dig_P3;
	int16_t  dig_P4;
	int16_t  dig_P5;
	int16_t  dig_P6;
	int16_t  dig_P7;
	int16_t  dig_P8;
	int16_t  dig_P9;
	uint8_t  dig_H1;
	int16_t  dig_H2;
	uint8_t  dig_H3;
	int16_t  dig_H4;
	int16_t  dig_H5;
	int8_t   dig_H6;
};

struct bme280_driver {
	bool sensor_available;
	int32_t adc_h;		///< RAW humidity
	int32_t adc_t;		///< RAW temp
	int32_t adc_p;		///< RAW pressure
	int32_t t_fine;		///< calibrated temp
	struct comp_params cp;	///< calibration data
};

extern struct bme280_driver bme280;

enum BME280_MODE {
	BME280_MODE_SLEEP  = 0x00,
	BME280_MODE_FORCED = 0x01,
	BME280_MODE_NORMAL = 0x03
};

#define BME280REG_CALIB_00	(0x88)
#define BME280REG_ID		(0xD0)
#define BME280REG_RESET		(0xE0)
#define BME280REG_CALIB_26	(0xE1)
#define BME280REG_CTRL_HUM	(0xF2)
#define BME280REG_STATUS	(0xF3)
#define BME280REG_CTRL_MEAS	(0xF4)
#define BME280REG_CONFIG	(0xF5)
#define BME280REG_PRESS_MSB	(0xF7)
#define BME280REG_PRESS_LSB	(0xF8)
#define BME280REG_PRESS_XLSB	(0xF9)
#define BME280REG_TEMP_MSB	(0xFA)
#define BME280REG_TEMP_LSB	(0xFB)
#define BME280REG_TEMP_XLSB	(0xFC)
#define BME280REG_HUM_MSB	(0xFD)
#define BME280REG_HUM_LSB	(0xFE)

#define ID_VALUE		(0x60)

#define BME280_OVERSAMPLING_SKIP	(0x00)
#define BME280_OVERSAMPLING_1		(0x01)
#define BME280_OVERSAMPLING_2		(0x02)
#define BME280_OVERSAMPLING_4		(0x03)
#define BME280_OVERSAMPLING_8		(0x04)
#define BME280_OVERSAMPLING_16		(0x05)

void bme280_init();
void bme280_set_mode(enum BME280_MODE mode);
int  bme280_is_measuring(void);
void bme280_read_measurements();
void bme280_set_oversampling_hum(uint8_t os);
void bme280_set_oversampling_temp(uint8_t os);
void bme280_set_oversampling_press(uint8_t os);
int32_t  bme280_get_temperature(void);
uint32_t bme280_get_pressure(void);
uint32_t bme280_get_humidity(void);
uint8_t bme280_read_reg(uint8_t reg);
void    bme280_write_reg(uint8_t reg, uint8_t value);
void    bme280_platform_init();


