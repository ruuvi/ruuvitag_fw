#ifndef SDK_APPLICATION_CONFIG_H
#define SDK_APPLICATION_CONFIG_H
#include "bluetooth_application_config.h"


// Define any application-specific sdk configuration overrides here
#define PWM_ENABLED     0
#define PWM0_ENABLED    0
#define APP_PWM_ENABLED 0
#define PPI_ENABLED     1
#define TIMER_ENABLED   1
#define TIMER0_ENABLED  1
#define TIMER1_ENABLED  1
#define TIMER2_ENABLED  1
#define TIMER3_ENABLED  1
#define TIMER4_ENABLED  0  //Required by NFC
#define NFC_HAL_ENABLED 1
#define CRC16_ENABLED   1  //CRC required by DFU
#define CRC32_ENABLED   1
#define NRF_LOG_ENABLED 1

#if APP_GATT_PROFILE_ENABLED
#define BLE_DIS_ENABLED 1  //Device information service
#define BLE_NUS_ENABLED 1  //Nordic UART Service
#define BLE_DFU_ENABLED 1  //DFU service
#endif

// Fix error if there is leftover configuration flash
#define FDS_OP_QUEUE_SIZE 10
#define FDS_CHUNK_QUEUE_SIZE 15
#define FDS_MAX_USERS 8
#define FDS_VIRTUAL_PAGES 3
#define FDS_VIRTUAL_PAGE_SIZE 1024


// WDT_CONFIG_RELOAD_VALUE - Reload value  <15-4294967295> (ms)
// Watchdog cannot be stopped even when entering bootloader, 
// so use 6 minutes to allow DFU process to complete.
#ifndef WDT_CONFIG_RELOAD_VALUE
#define WDT_CONFIG_RELOAD_VALUE 360000 
#endif

#endif
