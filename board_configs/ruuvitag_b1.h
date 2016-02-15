/* Copyright (c) 2015 Ruuvi Innovations Ltd. All Rights Reserved.
 *
 * License: BSD 
 */
#ifndef RUUVITAG_B1_H
#define RUUVITAG_B1_H

// LEDs definitions for RUUVITAG_B1
#define LEDS_NUMBER    2

#define LED_START      17
#define LED_1          17
#define LED_2          19
#define LED_STOP       19

#define LEDS_LIST { LED_1, LED_2 }

#define BSP_LED_0      LED_1
#define BSP_LED_1      LED_2

#define BSP_LED_0_MASK (1<<BSP_LED_0)
#define BSP_LED_1_MASK (1<<BSP_LED_1)

#define LEDS_MASK      (BSP_LED_0_MASK | BSP_LED_1_MASK)
/* all LEDs are lit when GPIO is low */
#define LEDS_INV_MASK  LEDS_MASK

#define BUTTONS_NUMBER 1

#define BUTTON_START   13
#define BUTTON_1       13
#define BUTTON_STOP    13
#define BUTTON_PULL    NRF_GPIO_PIN_PULLUP

#define BUTTONS_LIST { BUTTON_1 }

#define BSP_BUTTON_0   BUTTON_1

#define BSP_BUTTON_0_MASK (1<<BSP_BUTTON_0)

#define BUTTONS_MASK   (BSP_BUTTON_0_MASK) // or 0x00002000, 13th bit

#define RX_PIN_NUMBER  8
#define TX_PIN_NUMBER  7
#define CTS_PIN_NUMBER 5 // Not connected, but defined so that ble_app_uart example project compiles
#define RTS_PIN_NUMBER 6 // Not connected, but defined so that ble_app_uart example project compiles
#define HWFC           false

#define SPIM0_SCK_PIN   29  // SPI clock GPIO pin number.
#define SPIM0_MOSI_PIN  25  // SPI Master Out Slave In GPIO pin number.
#define SPIM0_MISO_PIN  28  // SPI Master In Slave Out GPIO pin number.
#define SPIM0_SS_ACC_PIN    11  // SPI Slave Select GPIO pin number. (accelerometer)
#define SPIM0_SS_HUMI_PIN    3  // SPI Slave Select GPIO pin number. (bme280)
#define SPIM0_SS_FLASH_PIN    4  // SPI Slave Select GPIO pin number. (dataflash)

// Low frequency clock source to be used by the SoftDevice
#define NRF_CLOCK_LFCLKSRC      NRF_CLOCK_LFCLKSRC_XTAL_20_PPM

#endif // RUUVITAG_B1_H
