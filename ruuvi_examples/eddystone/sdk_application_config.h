#ifndef SDK_APPLICATION_CONFIG_H
#define SDK_APPLICATION_CONFIG_H

//Define any application-specific sdk configuration overrides here
#define TIMER_ENABLED 1
#define PWM_ENABLED 1
#define PWM0_ENABLED 1
#define PWM1_ENABLED 1
#define APP_PWM_ENABLED 1
#define PPI_ENABLED 1
#define RTC_ENABLED 1
#define RTC0_ENABLED 0  // Used by softdevice, don't enable for application
#define RTC1_ENABLED 0  // Used by Eddystone libs
#define RTC2_ENABLED 1  // used by application RTC
#define APP_TIMER_KEEPS_RTC_ACTIVE 1 // Used by eddystone to keep track of time 
#define TIMER_ENABLED 0
#define TIMER0_ENABLED 0
#define TIMER1_ENABLED 0  //CSense
#define TIMER2_ENABLED 0  //CSense
#define TIMER3_ENABLED 0
#define TIMER4_ENABLED 0  //Required by NFC
#define NFC_HAL_ENABLED 1
#define CRC16_ENABLED 1
#define CRC32_ENABLED 1
#define NRF_LOG_ENABLED 1

#endif

