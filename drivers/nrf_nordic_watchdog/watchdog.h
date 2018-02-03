#ifndef WATCHDOG_H
#define WATCHDOG_H

#include "sdk_errors.h"
#include "nrf_error.h"

// Watchdog function takes no parameters and returns nothing.
typedef void(*watchdog_event_handler_t)(void);

// Initialize watchdog with configuration defined in sdk_config (possible sdk_application_config)
// Call wdt_event_handler on wdt event
ret_code_t watchdog_init(watchdog_event_handler_t handler);

//Start watchdog
void watchdog_enable(void);

//resets watchdog counter
void watchdog_feed(void);

// Log error and reset.
void watchdog_default_handler(void);

#endif 
