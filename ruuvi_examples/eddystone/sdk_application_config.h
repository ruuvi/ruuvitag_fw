#ifndef SDK_APPLICATION_CONFIG_H
#define SDK_APPLICATION_CONFIG_H

//Define any application-specific sdk configuration overrides here
#define PWM_ENABLED 0
#define PWM0_ENABLED 0
#define PWM1_ENABLED 0
#define APP_PWM_ENABLED 0
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
#define TIMER4_ENABLED 0  //Reserved by NFC
#define NFC_HAL_ENABLED 1
#define CRC16_ENABLED 1   //Required by DFU
#define CRC32_ENABLED 1
#define NRF_LOG_ENABLED 0

#define APP_GATT_PROFILE_ENABLED 1
#define PEER_MANAGER_ENABLED 1
#define BLE_DIS_ENABLED 1
#define BLE_NUS_ENABLED 0
#define BLE_DFU_ENABLED 0

#define FDS_OP_QUEUE_SIZE 10
#define FDS_CHUNK_QUEUE_SIZE 15
#define FDS_MAX_USERS 8
#define FDS_VIRTUAL_PAGES 10
#define FDS_VIRTUAL_PAGE_SIZE 1024

// WDT_CONFIG_RELOAD_VALUE - Reload value  <15-4294967295> (ms)
// Watchdog cannot be stopped even when entering bootloader, 
// so use 6 minutes to allow DFU process to complete.
#ifndef WDT_CONFIG_RELOAD_VALUE
#define WDT_CONFIG_RELOAD_VALUE 360000 
#endif

#endif

