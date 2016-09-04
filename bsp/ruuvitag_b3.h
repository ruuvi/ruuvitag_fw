/* Copyright (c) Ruuvi Innovations Ltd. All Rights Reserved.
 *
 * License: BSD 
 */
#ifndef RUUVITAG_B3_H
#define RUUVITAG_B3_H

// LEDs definitions
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
#define LEDS_INV_MASK  LEDS_MASK // LEDs are lit when GPIO low

#define BUTTONS_NUMBER 1
#define BUTTON_START   13
#define BUTTON_1       13
#define BUTTON_STOP    13
#define BUTTON_PULL    NRF_GPIO_PIN_PULLUP
#define BUTTONS_LIST { BUTTON_1 }
#define BSP_BUTTON_0   BUTTON_1
#define BSP_BUTTON_0_MASK (1<<BSP_BUTTON_0)
#define BUTTONS_MASK   (BSP_BUTTON_0_MASK) // or 0x00002000, 13th bit

#define HWFC           false

#define SPIM0_SCK_PIN     29  // SPI clock
#define SPIM0_MOSI_PIN    25  // SPI Master Out Slave In
#define SPIM0_MISO_PIN    28  // SPI Master In Slave Out
#define SPIM0_SS_ACC_PIN   8  // SPI Slave Select (accelerometer)
#define SPIM0_SS_HUMI_PIN  3  // SPI Slave Select (BME280)
#define INT_ACC1_PIN       2  // Accelerometer interrupt 1
#define INT_ACC2_PIN       6  // Accelerometer interrupt 2

// Low frequency clock source to be used by the SoftDevice
#define NRF_CLOCK_LFCLKSRC      {.source        = NRF_CLOCK_LF_SRC_XTAL,            \
                                 .rc_ctiv       = 0,                                \
                                 .rc_temp_ctiv  = 0,                                \
                                 .xtal_accuracy = NRF_CLOCK_LF_XTAL_ACCURACY_20_PPM}

#endif // RUUVITAG_B2_H
