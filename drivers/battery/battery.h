/* Copyright (c) 2016 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

#ifndef BATTERY_VOLTAGE_H__
#define BATTERY_VOLTAGE_H__

#include <stdint.h>

/**@brief Function for initializing the battery voltage module.
 */
void battery_voltage_init(void);


/**@brief Function for reading the battery voltage.
 *
 * If battery ADC is not already initialised, this function
 * initialises battery reading automatically. 
 *
 * @returns battery voltage in millivolts.
 */
uint16_t getBattery(void);

#endif
