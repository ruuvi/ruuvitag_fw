#ifndef RTC_H
#define RTC_H

#include "nrf_drv_rtc.h"
#include "nrf_drv_clock.h"

uint32_t init_rtc(void);

uint32_t millis(void);

#endif
