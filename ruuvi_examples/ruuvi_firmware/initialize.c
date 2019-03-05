#include <stdbool.h>
#include <stdint.h>
#include <math.h>

// Nordic SDK
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_radio_notification.h"
#include "nordic_common.h"
#include "softdevice_handler.h"
#include "app_scheduler.h"
#include "app_timer_appsh.h"
#include "nrf_drv_clock.h"
#include "nrf_gpio.h"
#include "nrf_drv_gpiote.h"
#include "nrf_delay.h"

#define NRF_LOG_MODULE_NAME "INITIALIZE"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

// BSP  Board Support Package : 
#include "bsp.h"

// Drivers
#include "flash.h"
#include "lis2dh12.h"
#include "lis2dh12_acceleration_handler.h"
#include "bme280.h"
#include "battery.h"
#include "bluetooth_core.h"
#include "eddystone.h"
#include "pin_interrupt.h"
#include "nfc.h"
#include "nfc_t2t_lib.h"
#include "rtc.h"
#include "application_config.h"

// Libraries
#include "sensortag.h"

// Init
#include "init.h"
#include  initialize.h

// Configuration
#include "bluetooth_config.h"      // including  REVision, intervals, APP_TX_POWER


// ID for main loop timer.
APP_TIMER_DEF(main_timer_id);                 // Creates timer id for our program.
APP_TIMER_DEF(reset_timer_id);                 // Creates timer id for our program.

//
// initalize "tunables" from header file values. Might be changed by NFC command:   (some day)
static uint16_t ble_tx_power                 = BLE_TX_POWER;
static uint16_t advertising_interval_highres = ADVERTISING_INTERVAL_RAW;    // why are these even different??
static uint16_t advertising_interval_raw     = ADVERTISING_INTERVAL_RAW;
static uint16_t advertising_interval_url     = ADVERTISING_INTERVAL_URL;
static uint16_t advertising_interval;
static uint32_t main_loop_interval_raw       = MAIN_LOOP_INTERVAL_RAW;
static uint32_t main_loop_interval_url       = MAIN_LOOP_INTERVAL_URL;
static uint32_t main_loop_interval;
static uint16_t battery_min_v                = BATTERY_MIN_V;



static uint16_t init_status      =       0;   // combined status of all initalizations.  Zero when all are complete if no errors occured.
#define LOG_FAILED_INIT             0x0002
#define ACCELEROMETER_FAILED_INIT   0x0004
#define TEMP_HUM_PRESS_FAILED_INIT  0x0008
#define NFC_FAILED_INIT             0x0010
#define BLE_FAILED_INIT             0x0020
#define TIMER_FAILED_INIT           0x0040
#define RTC_FAILED_INIT             0x0080
#define PIN_ENA_FAILED_INIT         0x0200
#define ACCEL_INT_FAILED_INIT       0x0400
#define ACC_INT_FAILED_INIT         0x0800
#define BATTERY_FAILED_INIT         0x1000
#define BUTTON_FAILED_INIT          0x2000
#define BME_FAILED_INIT             0x4000

// File and record for app mode
#define FDS_FILE_ID 1
#define FDS_RECORD_ID 1

// define unconfusing macros for LEDs
#define RED_LED_ON    nrf_gpio_pin_clear(LED_RED)
#define RED_LED_OFF   nrf_gpio_pin_set(LED_RED)
#define GREEN_LED_ON  nrf_gpio_pin_clear(LED_GREEN)
#define GREEN_LED_OFF nrf_gpio_pin_set(LED_GREEN)

static uint8_t data_buffer[RAWv2_DATA_LENGTH] = { 0 };
static bool model_plus = false;          // Flag for sensors available
static bool fast_advertising = true;     // Connectable mode
static uint64_t fast_advertising_start = 0;  // Timestamp of when tag became connectable
static uint64_t debounce = 0;        // Flag for avoiding double presses
static uint16_t acceleration_events = 0; // Number of times accelerometer has triggered
static volatile uint16_t vbat = 0; //Update in interrupt after radio activity.
static uint64_t last_battery_measurement = 0; // Timestamp of VBat update.
static ruuvi_sensor_t data;
static uint8_t advertisement_delay = 0; //Random, static delay to reduce collisions.
static uint8_t NFC_message[100];   // NFC message buffer has 4 records, up to 128 bytes each minus some overhead for NFC NDEF data keeping. 
static size_t NFC_message_length = sizeof(NFC_message);

// Possible modes of the app
#define RAWv1 0
#define RAWv2_FAST 1
#define RAWv2_SLOW 2

// Must be UINT32_T as flash storage operated in 4-byte chunks
// Will get loaded from flash, this is default.
static uint32_t tag_mode __attribute__ ((aligned (4))) = RAWv1;
// Rates of advertising. These must match the tag mode enum.
static const uint16_t advertising_rates[] = {
  ADVERTISING_INTERVAL_RAW,
  ADVERTISING_INTERVAL_RAW,
  ADVERTISING_INTERVAL_RAW_SLOW
};
// Rates of advertising. These must match the tag mode enum.
static const uint16_t advertising_sizes[] = {
  RAWv1_DATA_LENGTH,
  RAWv2_DATA_LENGTH,
  RAWv2_DATA_LENGTH
};

// Prototype declaration
static void main_timer_handler(void * p_context);

int  initialize(void)
{
   // LEDs first (they're easy and cannot fail)  drivers/init/init.c
  init_leds();
  RED_LED_ON;

  if( init_log() ) { init_status |=LOG_FAILED_INIT; }
  else { NRF_LOG_INFO("LOG initalized \r\n"); } // subsequent initalizations assume log is working

 
  // tuneables retained in UICR                                  start at 4 since some Nordic guys use 1...
 // is UICR appropriate for this version of the program; if not go with factory defaults defined in .h 
  int version[3]; sscanf(INIT_FWREV, "%1d%1c%1d%1c%1d", &version[0],&x,&version[1],&x, &version[2]); 
  if ( ( NRF_FICR->UICR[0]&& 0xFFFF00)  == version[0] **24 + version[1] **16 + version[2] **8 )  // any minor-minor verion
    {
     password                 =
     packet_format            =  NRF_FICR->UICR[4] & 0xF0000000 >> 28; // 0..15 Example:  03 , 05
     ble_tx_power             =  NRF_FICR->UICR[4] & 0x0F000000 >> 24; // 0 .. 16     ( -xxdB ??)
     main_loop_interval       =  NRF_FICR->UICR[4] & 0x00FFFFFF ;      // 1 .. 16,777.215 seconds aka    279 minutes  Example: 3E8 1,000ms
     advertising_interval     =  NRF_FICR->UICR[5] & 0x00FFFFFF ;      // 1 ..                    aka 4.66 hours               7DA 2,010ms
     accel_sample_rate        =  NRF_FICR->UICR[5] & 0xF0000000 >>24;  // 0 .. 7 LIS2DH12_RATE_400 = 7<<4 see drivers/lis2dh12/lis2dh12.h
     accel_resolution         =  NRF_FICR->UICR[5] & 0x0F000000 >>24;  // 0 .. F 8,6,4
     humidity_oversampling    =  NRF_FICR->UICR[6] & 0xFF000000 >>24;  //           uint_8
     temperature_oversampling =  NRF_FICR->UICR[6] & 0x00FF0000 >>16;
     pressure_oversampling    =  NRF_FICR->UICR[6] & 0x0000FF00 >> 8;
     bme280_IIR               =  NRF_FICR->UICR[7] & 0xFF000000 >>24;  //           uint_8
     bme280_delay             =  NRF_FICR->UICR[7] & 0x00FF0000 >>24;  //           uint_8
     battery_min_v            = (NRF_FICR->UICR[6] & 0x0000000F) * 100 +2000;  // 2.000-2.800   Example 2.100  = 01 *100 +2000
                                                                               //                       2.200  = 02 *100 +2000
                                                                               //                       3.000  = 0A *100 +2000
//     available                                6      000000F0
//     available                                7      0000FFFF
    }

//   example  540007DA 360007DA 0A0A0A08         
//  fmt 5, Tx 4, loop 2.01       Hos=Tos=Pos=10
//                     acc_samp 3      batMin 2.800
//                       acc_reso 6
//                         advertis 2.100


  // start watchdog now in case program hangs up.
  // watchdog_default_handler logs error and resets the tag.
  init_watchdog(NULL);

  // Battery voltage initialization cannot fail under any reasonable circumstance.
  battery_voltage_init(); 
  vbat = getBattery();

  if( vbat < BATTERY_MIN_V ) { init_status |=BATTERY_FAILED_INIT; }
  else NRF_LOG_INFO("BATTERY initalized \r\n"); 

  if(init_sensors() == NRF_SUCCESS )
  {
    model_plus = true;
    NRF_LOG_INFO("Sensors initialized \r\n");  
  }

  // Init NFC ASAP in case we're waking from deep sleep via NFC (todo)
  // outputs ID:DEVICEID ,MAC:DEVICEADDR, SW:REVision
  set_nfc_callback(app_nfc_callback);
  if( init_nfc() ) { init_status |= NFC_FAILED_INIT; } 
  else { NRF_LOG_INFO("NFC init \r\n"); }

  pin_interrupt_init(); 

  if( pin_interrupt_enable(BSP_BUTTON_0, NRF_GPIOTE_POLARITY_TOGGLE, NRF_GPIO_PIN_PULLUP, button_press_handler) ) 
  {
    init_status |= BUTTON_FAILED_INIT;
  }

  // Initialize BLE Stack. Starts LFCLK required for timer operation.
  if( init_ble() ) { init_status |= BLE_FAILED_INIT; }
  bluetooth_configure_advertisement_type(STARTUP_ADVERTISEMENT_TYPE);
  bluetooth_tx_power_set(BLE_TX_POWER);
  bluetooth_configure_advertising_interval(ADVERTISING_INTERVAL_STARTUP);
  advertisement_delay = NRF_FICR->DEVICEID[0]&0x0F;

  // Priorities 2 and 3 are after SD timing critical events. 
  // 6, 7 after SD non-critical events.
  // Triggers ADC, so use 3. 
  ble_radio_notification_init(3,
                              NRF_RADIO_NOTIFICATION_DISTANCE_800US,
                              on_radio_evt);

  // If GATT is enabled BLE init inits peer manager which uses flash.
  // BLE init should handle insufficient space gracefully (i.e. erase flash and proceed). 
  // Flash must be initialized after softdevice. 
  if(flash_init())
  {
    NRF_LOG_ERROR("Failed to init flash \r\n");
  }
  size_t flash_space_remaining = 0;
  flash_free_size_get(&flash_space_remaining);
  NRF_LOG_INFO("Largest continuous space remaining %d bytes\r\n", flash_space_remaining);
  if(4000 > flash_space_remaining)
  {
    NRF_LOG_INFO("Flash space is almost used, running gc\r\n")
    flash_gc_run();
    flash_free_size_get(&flash_space_remaining);
    NRF_LOG_INFO("Continuous space remaining after gc %d bytes\r\n", flash_space_remaining);
  }
  else if (flash_record_get(FDS_FILE_ID, FDS_RECORD_ID, sizeof(tag_mode), &tag_mode))
  {
   NRF_LOG_INFO("Did not find mode in flash, is this first boot? \r\n");
  }
  else 
  {
    NRF_LOG_INFO("Loaded mode %d from flash\r\n", tag_mode);
  }

  if( init_rtc() ) { init_status |= RTC_FAILED_INIT; }
  else { NRF_LOG_INFO("RTC initialized \r\n"); }

  // Initialize lis2dh12 and BME280 - TODO: Differentiate LIS2DH12 and BME280 
  if (model_plus)    
  {
    lis2dh12_reset(); // Clear memory.
    
    // Enable Low-To-Hi rising edge trigger interrupt on nRF52 to detect acceleration events.
    if (pin_interrupt_enable(INT_ACC2_PIN, NRF_GPIOTE_POLARITY_LOTOHI, NRF_GPIO_PIN_NOPULL, lis2dh12_int2_handler) )
    {
      init_status |= ACC_INT_FAILED_INIT;
    }
    
    nrf_delay_ms(10); // Wait for LIS reboot.
    // Enable XYZ axes.
    lis2dh12_enable();
    lis2dh12_set_scale(LIS2DH12_SCALE);
    lis2dh12_set_sample_rate(LIS2DH12_SAMPLERATE_RAWv1);
    lis2dh12_set_resolution(LIS2DH12_RESOLUTION);

    lis2dh12_set_activity_interrupt_pin_2(LIS2DH12_ACTIVITY_THRESHOLD);
    NRF_LOG_INFO("Accelerometer configuration done \r\n");

    // oversampling must be set for each used sensor.
    bme280_set_oversampling_hum  (BME280_HUMIDITY_OVERSAMPLING);
    bme280_set_oversampling_temp (BME280_TEMPERATURE_OVERSAMPLING);
    bme280_set_oversampling_press(BME280_PRESSURE_OVERSAMPLING);
    bme280_set_iir(BME280_IIR);
    bme280_set_interval(BME280_DELAY);
    bme280_set_mode(BME280_MODE_NORMAL);
    NRF_LOG_INFO("BME280 configuration done \r\n");
  }

  // Enter stored mode after boot - or default mode if store mode was not found
  app_sched_event_put (&tag_mode, sizeof(&tag_mode), change_mode);
  
  // Initialize repeated timer for sensor read and single-shot timer for button reset
  if( init_timer(main_timer_id, APP_TIMER_MODE_REPEATED, MAIN_LOOP_INTERVAL_RAW, main_timer_handler) )
  {
    init_status |= TIMER_FAILED_INIT;
  }
  
  if( init_timer(reset_timer_id, APP_TIMER_MODE_SINGLE_SHOT, BUTTON_RESET_TIME, reboot) )
  {
    init_status |= TIMER_FAILED_INIT;
  }
  // Init starts timers, stop the reset
  app_timer_stop(reset_timer_id);

  // Log errors, add a note to NFC, blink RED to visually indicate the problem
  if (init_status)
  { 
    snprintf((char* )NFC_message, NFC_message_length, "Error: %X", init_status);
    NRF_LOG_WARNING (" -- Initalization error :  %X \r\n", init_status);
    for ( int16_t i=0; i<13; i++)
    { 
      RED_LED_ON;
      nrf_delay_ms(500u);
      RED_LED_OFF;
      nrf_delay_ms(500u); 
    }
  }
  
  // Turn green led on if model+ with no errors.
  // Power manage turns led off
  if (model_plus & !init_status)
  {
    GREEN_LED_ON;
  }

  // Turn off red led, leave green on to signal model+ without errors
  RED_LED_OFF;

  nrf_delay_ms( sample_interval * 2);        // Wait for sensors to take first sample
  // Get first sample from sensors
  app_sched_event_put (NULL, 0, main_sensor_task);
  app_sched_execute();

  // Start advertising 
  bluetooth_advertising_start(); 
  NRF_LOG_INFO("Advertising started\r\n");

}
