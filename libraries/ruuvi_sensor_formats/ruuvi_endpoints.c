#include "ruuvi_endpoints.h"

#define NRF_LOG_MODULE_NAME "ENDPOINTS"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

/** Sensor data handlers **/
message_handler p_unknown_handler           = NULL;
message_handler p_battery_handler           = NULL;
message_handler p_rng_handler               = NULL;
message_handler p_rtc_handler               = NULL;
message_handler p_temperature_handler       = NULL;
message_handler p_humidity_handler          = NULL;
message_handler p_pressure_handler          = NULL;
message_handler p_air_quality_handler       = NULL;
message_handler p_acceleration_handler      = NULL;
message_handler p_magnetometer_handler      = NULL;
message_handler p_gyroscope_handler         = NULL;
message_handler p_movement_detector_handler = NULL;
message_handler p_mam_handler               = NULL;

/** Data traffic handlers **/
message_handler p_reply_handler       = NULL;
message_handler p_ble_adv_handler     = NULL;
message_handler p_ble_gatt_handler    = NULL;
message_handler p_proprietary_handler = NULL;
message_handler p_nfc_handler         = NULL;
message_handler p_ram_handler         = NULL;
message_handler p_flash_handler       = NULL;

/** Routes message to appropriate endpoint handler.
 *  Messages will send data to their configured transmission points
 **/
void route_message(const ruuvi_standard_message_t message)
{
    NRF_LOG_INFO("Routing message.\r\n");
    switch(message.destination_endpoint)
    {
      case PLAINTEXT_MESSAGE:
        unknown_handler(message); // Application does not handle plain text - TODO: Not implemented hander?
        break;
      
      case MESSAGE_ACKNOWLEDGEMENT:
        unknown_handler(message); // Application does not handle acknowledgements
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
        NRF_LOG_INFO("Message is a temperature message.\r\n");
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
        if(p_pressure_handler) {p_pressure_handler(message); } 
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
  ruuvi_standard_message_t reply = { .destination_endpoint = message.source_endpoint,
                                     .source_endpoint = message.destination_endpoint,
                                     .type = UNKNOWN,
                                     .payload = {*(message.payload)}}; //TODO: Check that data gets deep copied
  if(p_reply_handler){ return p_reply_handler(reply); }
  return ENDPOINT_HANDLER_ERROR;
}
