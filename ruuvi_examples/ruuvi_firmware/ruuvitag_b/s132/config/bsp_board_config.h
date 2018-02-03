#ifndef BSP_BOARD_CONFIG_H
#define BSP_BOARD_CONFIG_H

#include "ruuvitag_b.h" // Pin numbers

// Define any board-specific BSP configuration overrides here

#define NON_CONNECTABLE_ADV_LED_PIN BSP_BOARD_LED_1  //!< Toggles when non-connectable advertisement is sent.
#define CONNECTED_LED_PIN           BSP_BOARD_LED_0  //!< Is on when device has connected.
#define CONNECTABLE_ADV_LED_PIN     BSP_BOARD_LED_0  //!< Is on when device is advertising connectable advertisements.

#endif
