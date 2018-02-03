#ifndef RUUVI_ENDPOINTS
#define RUUVI_ENDPOINTS

#include "sdk_common.h"

// maximum DSP states for one endpoint
// 4 supports int16, but not int8.
#define MAX_DSP_STATES 4
#include "dsp.h"

typedef enum{
  PLAINTEXT_MESSAGE       = 0x10, // Plaintext data for info, debug etc
  BATTERY                 = 0x20, // Battery state message
  RNG                     = 0x21, // Random number
  RTC                     = 0x22, // Real time clock 
  TEMPERATURE             = 0x31, // Temperature message
  HUMIDITY                = 0x32,
  PRESSURE                = 0x33,
  AIR_QUALITY             = 0x34,
  ENVIRONMENTAL           = 0x3A, // Aggregate of temperature, humidity, pressure,
  ACCELERATION            = 0x40,
  MAGNETOMETER            = 0x41,
  GYROSCOPE               = 0x42,
  MOVEMENT_DETECTOR       = 0x43, 
  // endpoints 0x50 ... 0x5F are reserved for chain handlers, however they're not enumerated but rather called dynamically
  MAM                     = 0xE0  // Masked Authenticated Messaging
}ruuvi_endpoint_t;

typedef enum{
  SENSOR_CONFIGURATION           = 0x01, // Configure sensor
  ACTUATOR_CONFIGRATION          = 0x02, // Configure actuator
  ACKNOWLEDGEMENT                = 0x03, // Acknowledge message
  STATUS_QUERY                   = 0x04, // Query status from endpoint - returns current configuration
  DATA_QUERY                     = 0x05, // Query data from endpoint - returns latest data
  LOG_QUERY                      = 0x06, // Query log from endpoint - initiates bulk write
  CAPABILITY_QUERY               = 0x07, // Query endpoint capablities: samplerate, resolution, scale, log, power consumption with settings given in query
  SAMPLERATE_RESPONSE            = 0x08, // Response with allowed sample rates
  RESOLUTION_RESPONSE            = 0x09, // Response with allowed resolutions
  SCALE_RESPONSE                 = 0x10, // Response with allowed scales
  TARGET_RESPONSE                = 0x11, // Response with allowed targets
  POWER_RESPONSE                 = 0x12, // Response with allowed power levels
  TIMESTAMP                      = 0x13, // Timestamp related to next event
  UNKNOWN                        = 0x14, // Unknown, may be a reply if incoming message was not understood
  ERROR                          = 0x15, // Error, payload may contain details
  CHAIN_UPSTREAM_CONFIGURATION   = 0x16, // Configure a chain endpoint
  CHAIN_DOWNSTREAM_CONFIGURATION = 0x17, // Pass a function pointer to call with new data from actual sensor
  UINT8                          = 0x80, // Array of uint8
  INT8                           = 0x81,
  UINT16                         = 0x82,
  INT16                          = 0x83,
  UINT32                         = 0x84,
  INT32                          = 0x85,
  UINT64                         = 0x86, // Single uint64
  INT64                          = 0x87,
  ASCII                          = 0x88  // ASCII array
}ruuvi_message_type_t;

typedef enum {
  SAMPLE_RATE_STOP   = 0,
  SAMPLE_RATE_SINGLE = 251,
  SAMPLE_RATE_NO_CHANGE = 255
}ruuvi_samplerate_t;

typedef enum {
  TRANSMISSION_RATE_STOP       = 0,
  TRANSMISSION_RATE_SAMPLERATE = 251,
  TRANSMISSION_RATE_DSPRATE    = 252,
  TRANSMISSION_RATE_NO_CHANGE  = 255
}ruuvi_transmissionrate_t;

typedef enum {
  RESOLUTION_MIN        = 251,
  RESOLUTION_MAX        = 252,
  RESOLUTION_NO_CHANGE  = 255
}ruuvi_resolution_t;

typedef enum {
  SCALE_MIN        = 251,
  SCALE_MAX        = 252,
  SCALE_NO_CHANGE  = 255
}ruuvi_scale_t;

typedef enum {
  DSP_LAST      = 1,
  DSP_MIN       = 2,
  DSP_MAX       = 3,
  DSP_AVERAGE   = 4,
  DSP_STDEV     = 5,
  DSP_IMPULSE   = 6,
  DSP_LOW_PASS  = 7,
  DSP_HIGH_PASS = 8,
  DSP_VECTOR    = 128
}ruuvi_dsp_function_t;

typedef enum {
  TRANSMISSION_TARGET_STOP        = 0,    // Do not transmit any data anywhere
  TRANSMISSION_TARGET_BLE_ADV     = 1,    // Broadcast data as BLE adverisement
  TRANSMISSION_TARGET_BLE_GATT    = 2,    // Transmit data through BLE GATT
  TRANSMISSION_TARGET_BLE_MESH    = 4,    // Transmit data through BLE MESH  
  TRANSMISSION_TARGET_PROPRIETARY = 8,    // Transmit data through proprietary protocol
  TRANSMISSION_TARGET_NFC         = 16,   // Transmit data through NFC
  TRANSMISSION_TARGET_RAM         = 32,   // Store transmissions to RAM
  TRANSMISSION_TARGET_FLASH       = 64,   // Store transmission to FLASH
  TRANSMISSION_TARGET_TIMESTAMP   = 128,  // Add Timestamp (uses lot of memory / bandwidth)
  TRANSMISSION_TARGET_NO_CHANGE   = 255
}ruuvi_transmission_target_t;

typedef enum{
  ENDPOINT_SUCCESS         = 0, // ok
  ENDPOINT_NOT_IMPLEMENTED = 1, // not implememented yet
  ENDPOINT_UNKNOWN         = 2, // unknown parameter
  ENDPOINT_NOT_SUPPORTED   = 4, // not supported
  ENDPOINT_INVALID         = 8, // Invalid parameter for some reason
  ENDPOINT_HANDLER_ERROR   = 16 // Error in data handler
}ruuvi_endpoint_ret_t;

/**
 *  Configuration message for a sensor. 
 */
typedef struct __attribute__((packed)){
  uint8_t sample_rate;
  uint8_t transmission_rate;
  uint8_t resolution;
  uint8_t scale;
  uint8_t dsp_function;
  uint8_t dsp_parameter;
  uint8_t target;
  uint8_t reserved;
}ruuvi_sensor_configuration_t;

/**
 *  Configure chained channel with given upstream.
 *  Usage example: both latest and high passed acceleration is wanted.
 *  Setup accelerometer as usual
 *  Configure a chained channel using endpoint 0x50 ... 0x5F to have ACCELERATION as master endpoint, high pass as a DSP function.
 *  ACCELERATION will transmit *AFTER* its own dsp function the samples to chained channel. The samples are sent at sample
 *  rate of master channel.
 *  Chained channel will transmit the samples to next chained channel after DSP, and to data handlers at a rate given by transmit speed.
 */
typedef struct __attribute__((packed)){
  uint8_t upstream_endpoint;
  uint8_t transmission_rate; // Stop chained transmissions if TRANSMISSION_RATE_0
  uint8_t reserved0; // Scale, resolution are defined by master
  uint8_t reserved1;
  uint8_t dsp_function;
  uint8_t dsp_parameter;
  uint8_t target;
  uint8_t reserved2;
}ruuvi_chain_configuration_t;

typedef struct __attribute__((packed)){
  uint8_t destination_endpoint;
  uint8_t source_endpoint;
  uint8_t type;
  uint8_t payload[8];
}ruuvi_standard_message_t;

// Declare message handler type
typedef ret_code_t(*message_handler)(const ruuvi_standard_message_t);

/** Message handler state **/
typedef struct {
/** Data target handlers **/
  message_handler p_ble_adv_handler;
  message_handler p_ble_gatt_handler;
  message_handler p_ble_mesh_handler;
  message_handler p_proprietary_handler;
  message_handler p_nfc_handler;
  message_handler p_ram_handler;
  message_handler p_flash_handler;

/** Chain downstream **/
  message_handler p_chain_handler;
  uint8_t downstream_endpoint; // Use UINT8_t to allow dynamic endpoint selection

/** State variables **/
  ruuvi_sensor_configuration_t configuration;
  ruuvi_endpoint_t destination_endpoint;
  dsp_filter_t dsp[MAX_DSP_STATES];
}message_handler_state_t;

void ble_gatt_scheduler_event_handler(void *p_event_data, uint16_t event_size);

// pass structs by value, as they might be copied to tx buffer somewhere.
void route_message(const ruuvi_standard_message_t message);

ret_code_t unknown_handler(const ruuvi_standard_message_t message);

// Peripheral handlers
void set_temperature_handler(message_handler handler);
void set_acceleration_handler(message_handler handler);
void set_mam_handler(message_handler handler);
void set_unknown_handler(message_handler handler);

// Data transmission handlers
void set_ble_adv_handler(message_handler handler);
void set_ble_gatt_handler(message_handler handler);
void set_proprietary_handler(message_handler handler);
void set_nfc_handler(message_handler handler);
void set_reply_handler(message_handler handler);
void set_ram_handler(message_handler handler);
void set_flash_handler(message_handler handler);
void set_chain_handler(message_handler handler);

message_handler get_reply_handler(void);
message_handler get_ble_adv_handler(void);
message_handler get_ble_gatt_handler(void);
message_handler get_ble_mesh_handler(void);
message_handler get_proprietary_handler(void);
message_handler get_nfc_handler(void);
message_handler get_ram_handler(void);
message_handler get_flash_handler(void);
message_handler get_chain_handler(void);

#endif
