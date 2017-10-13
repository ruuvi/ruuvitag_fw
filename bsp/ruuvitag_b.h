/* Copyright (c) Ruuvi Innovations Ltd. All Rights Reserved.
 *
 * License: BSD 
 */
#ifndef RUUVITAG_B_H
#define RUUVITAG_B_H

// LEDs definitions
#define LEDS_NUMBER     2
#define LED_START       17
#define LED_1           17
#define LED_2           19
#define LED_1           17
#define LED_STOP        19
#define LEDS_LIST       { LED_1, LED_2 }
#define BSP_LED_0       LED_1
#define BSP_LED_1       LED_2
#define BSP_LED_0_MASK  (1<<BSP_LED_0)
#define BSP_LED_1_MASK  (1<<BSP_LED_1)
#define LEDS_INV_MASK   LEDS_MASK // LEDs are lit when GPIO low
#define LED_RED         LED_1
#define LED_GREEN       LED_2
#define LEDS_ACTIVE_STATE 0

#ifndef RUUVI_BOOTLOADER_WORKAROUND
  #define BUTTONS_NUMBER 1
  #define BUTTON_START   13
  #define BUTTON_1       13
  #define BUTTON_STOP    13
  #define BUTTON_PULL    NRF_GPIO_PIN_PULLUP
  #define BUTTONS_LIST { BUTTON_1 }
  #define BSP_BUTTON_0   BUTTON_1
  #define BUTTONS_ACTIVE_STATE 0
#else
  #define BUTTONS_NUMBER 2
  #define BUTTON_START   13
  #define BUTTON_1       13
  #define BUTTON_STOP    13
  #define BUTTON_PULL    NRF_GPIO_PIN_PULLUP
  #define BUTTONS_LIST { BUTTON_1, BUTTON_1 }
  #define BSP_BUTTON_0   BUTTON_1
  #define BUTTONS_ACTIVE_STATE 0
  //For Bootloader
  #define BSP_BUTTON_3 BUTTON_1
  #define BSP_LED_2 LED_1
#endif

#define RX_PIN_NUMBER   4
#define TX_PIN_NUMBER   5
#define RTS_PIN_NUMBER  NRF_GPIO_UNUSED
#define CTS_PIN_NUMBER  NRF_GPIO_UNUSED
#define HWFC           false

#define SPIM0_SCK_PIN     29  // SPI clock
#define SPIM0_MOSI_PIN    25  // SPI Master Out Slave In
#define SPIM0_MISO_PIN    28  // SPI Master In Slave Out
#define SPIM0_SS_ACC_PIN   8  // SPI Slave Select (accelerometer)
#define SPIM0_SS_HUMI_PIN  3  // SPI Slave Select (BME280)
#define INT_ACC1_PIN       2  // Accelerometer interrupt 1
#define INT_ACC2_PIN       6  // Accelerometer interrupt 2

#define REVERSE_PROT_VOLT_DROP_MILLIVOLTS 1    //!< Typical forward voltage drop of the mosfet

// Low frequency clock source to be used by the SoftDevice
#define NRF_CLOCK_LFCLKSRC      {.source        = NRF_CLOCK_LF_SRC_XTAL,            \
                                 .rc_ctiv       = 0,                                \
                                 .rc_temp_ctiv  = 0,                                \
                                 .xtal_accuracy = NRF_CLOCK_LF_XTAL_ACCURACY_20_PPM}

//Timers for sensors and main loop - caused high power consumption and interference with softdevice.
//Use APP TIMER based on LFCLK instead unless you absolutely need these.
//#define RUUVITAG_PROGRAM_TIMER 0
//#define RUUVITAG_LIS2DH12_TIMER 1
//#define RUUVITAG_BME280_TIMER 2
//#define RUUVITAG_USER_TIMER 3

#endif // RUUVITAG_B5_H
