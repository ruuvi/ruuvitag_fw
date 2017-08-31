#include "ruuvi_endpoints.h"
#include "nrf_delay.h"

#define NRF_LOG_MODULE_NAME "ENDPOINTS"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

/** Sensor data handlers **/
static message_handler p_battery_handler           = NULL;
static message_handler p_rng_handler               = NULL;
static message_handler p_rtc_handler               = NULL;
static message_handler p_temperature_handler       = NULL;
static message_handler p_humidity_handler          = NULL;
static message_handler p_pressure_handler          = NULL;
static message_handler p_air_quality_handler       = NULL;
static message_handler p_acceleration_handler      = NULL;
static message_handler p_magnetometer_handler      = NULL;
static message_handler p_gyroscope_handler         = NULL;
static message_handler p_movement_detector_handler = NULL;
static message_handler p_mam_handler               = NULL;

/** Data traffic handlers **/
static message_handler p_reply_handler       = NULL;
static message_handler p_ble_adv_handler     = NULL;
static message_handler p_ble_gatt_handler    = NULL;
static message_handler p_ble_mesh_handler    = NULL;
static message_handler p_proprietary_handler = NULL;
static message_handler p_nfc_handler         = NULL;
static message_handler p_ram_handler         = NULL;
static message_handler p_flash_handler       = NULL;

/** Scheduler handler to call message router **/
void ble_gatt_scheduler_event_handler(void *p_event_data, uint16_t event_size)
{
  //TODO: Handle incoming bulk writes
  ruuvi_standard_message_t message = {};
  memcpy(&message, p_event_data, sizeof(message));
  route_message(message);
}

/** Routes message to appropriate endpoint handler.
 *  Messages will send data to their configured transmission points
 **/
void route_message(const ruuvi_standard_message_t message)
{
    NRF_LOG_INFO("Routing message. %x, %x, %x, \r\n",message.destination_endpoint, message.source_endpoint, message.type);
    switch(message.destination_endpoint)
    {
      case PLAINTEXT_MESSAGE:
        unknown_handler(message); // Application does not handle plain text - TODO: Not implemented hander?
        break;
      
      case BATTERY:
        if(p_battery_handler) {p_battery_handler(message); } 
        else {unknown_handler(message); }
        break;
        
      case RNG:
        if(p_rng_handler) {p_rng_handler(message); } 
        else {unknown_handler(message); }
        break;

      case RTC:
        if(p_rtc_handler) {p_rtc_handler(message); } 
        else {unknown_handler(message); }
        break;

      case TEMPERATURE:
        NRF_LOG_DEBUG("Message is a temperature message.\r\n");
        if(p_temperature_handler) {p_temperature_handler(message); } 
        else {unknown_handler(message); }
        break;

      case HUMIDITY:
        if(p_humidity_handler) {p_humidity_handler(message); } 
        else {unknown_handler(message); }
        break;
      
      case PRESSURE:
        if(p_pressure_handler) {p_pressure_handler(message); } 
        else {unknown_handler(message); }
        break;
      
      case AIR_QUALITY:
        if(p_air_quality_handler) {p_air_quality_handler(message); } 
        else {unknown_handler(message); }
        break;
        
      case ACCELERATION:
        if(p_acceleration_handler) {p_acceleration_handler(message); } 
        else {unknown_handler(message); }
        break;
      
      case MAGNETOMETER:
        if(p_magnetometer_handler) {p_magnetometer_handler(message); } 
        else {unknown_handler(message); }
        break;
      
      case GYROSCOPE:
        if(p_gyroscope_handler) {p_gyroscope_handler(message); } 
        else {unknown_handler(message); }
        break;
        
      case MOVEMENT_DETECTOR:
        if(p_movement_detector_handler) {p_movement_detector_handler(message); } 
        else {unknown_handler(message); }
        break;

      case MAM:
        if(p_mam_handler) {p_mam_handler(message); } 
        else {unknown_handler(message); }
        break;
    
      default:
        unknown_handler(message);
        break;

    }
}

void set_temperature_handler(message_handler handler)
{
  p_temperature_handler = handler;
}

void set_acceleration_handler(message_handler handler)
{
  p_acceleration_handler = handler;
}

void set_mam_handler(message_handler handler)
{
  p_mam_handler = handler;
}

void set_reply_handler(message_handler handler)
{
  p_reply_handler = handler;
}

void set_ble_adv_handler(message_handler handler)
{
  p_ble_adv_handler = handler;
}

void set_ble_gatt_handler(message_handler handler)
{
  p_ble_gatt_handler = handler;
}

void set_ble_mesh_handler(message_handler handler)
{
  p_ble_mesh_handler = handler;
}

void set_proprietary_handler(message_handler handler)
{
  p_proprietary_handler = handler;
}

void set_nfc_handler(message_handler handler)
{
  p_nfc_handler = handler;
}

void set_ram_handler(message_handler handler)
{
  p_ram_handler = handler;
}

void set_flash_handler(message_handler handler)
{
  p_flash_handler = handler;
}

message_handler get_reply_handler(void)
{
  return p_reply_handler;
}

message_handler get_ble_adv_handler(void)
{
  return p_ble_adv_handler;
}

message_handler get_ble_gatt_handler(void)
{
  return p_ble_gatt_handler;
}

message_handler get_ble_mesh_handler(void)
{
  return p_ble_mesh_handler;
}

message_handler get_proprietary_handler(void)
{
  return p_proprietary_handler;
}

message_handler get_nfc_handler(void)
{
  return p_nfc_handler;
}

message_handler get_ram_handler(void)
{
  return p_ram_handler;
}

message_handler get_flash_handler(void)
{
  return p_flash_handler;
}

// Send payload back to source with type "UNKNOWN"
ret_code_t unknown_handler(const ruuvi_standard_message_t message)
{
  NRF_LOG_INFO("Unknown message\r\n");
  ruuvi_standard_message_t reply = { .destination_endpoint = message.source_endpoint,
                                     .source_endpoint = message.destination_endpoint,
                                     .type = UNKNOWN,
                                     .payload = {*(message.payload)}}; //TODO: Check that data gets deep copied
  if(p_reply_handler){ return p_reply_handler(reply); }
  return ENDPOINT_HANDLER_ERROR;
}
