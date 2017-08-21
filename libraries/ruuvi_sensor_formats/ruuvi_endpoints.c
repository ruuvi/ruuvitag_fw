#include "ruuvi_endpoints.h"

#define NRF_LOG_MODULE_NAME "ENDPOINTS"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

message_handler p_unknown_handler = NULL;
message_handler p_battery_handler = NULL;
message_handler p_rng_handler = NULL;
message_handler p_rtc_handler = NULL;
message_handler p_temperature_handler = NULL;

message_handler p_data_handler = NULL;
message_handler p_ram_handler = NULL;
message_handler p_flash_handler = NULL;

/** Routes message to appropriate endpoint handler.
    Messages will send data to their configured transmission points
 **/
void route_message(const ruuvi_standard_message_t message)
{
    NRF_LOG_INFO("Routing message.\r\n");
    switch(message.destination_endpoint)
    {
      case PLAINTEXT_MESSAGE:
        unknown_handler(message); // Application does not handle plain text
        break;
      
      case MESSAGE_ACKNOWLEDGEMENT:
        unknown_handler(message); //Application does not handle acknowledgements
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
        
      default:
        unknown_handler(message);
        break;
      /*
      TODO
      HUMIDITY                = 0x32,
      PRESSURE                = 0x33,
      AIR_QUALITY             = 0x34,
      ACCELERATION            = 0x40,
      MAGNETOMETER            = 0x41,
      GYROSCOPE               = 0x42,
      MOVEMENT_DETECTOR       = 0x43,
      MAM                     = 0xE0
      */
    }
}

void set_temperature_handler(message_handler handler)
{
  p_temperature_handler = handler;
}

void set_data_handler(message_handler handler)
{
  p_data_handler = handler;
}

void set_ram_handler(message_handler handler)
{
  p_ram_handler = handler;
}

void set_flash_handler(message_handler handler)
{
  p_flash_handler = handler;
}

message_handler get_data_handler(void)
{
  return p_data_handler;
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
  if(p_data_handler){ return p_data_handler(reply); }
  return 1;//TODO: Proper error code
}
