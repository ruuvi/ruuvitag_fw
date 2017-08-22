#include "bme280_temperature_handler.h"
#include "ruuvi_endpoints.h"
#include "nrf_error.h"
#include "bme280.h"

/** Data target handlers **/
message_handler p_reply_handler = NULL;
message_handler p_ble_adv_handler = NULL;
message_handler p_ble_gatt_handler = NULL;
message_handler p_proprietary_handler = NULL;
message_handler p_nfc_handler = NULL;
message_handler p_ram_handler = NULL;
message_handler p_flash_handler = NULL;

//TODO: Add timer interrupt to sample rate to read samples 
/** This must be called as last function, as this function may bring BME280 out of sleep which prevents further configuration  **/
static ret_code_t set_sample_rate(uint8_t sample_rate)
{
  if(SAMPLE_RATE_STOP == message.payload.sample_rate)      { return bme280_set_mode(BME280_MODE_SLEEP); }
  if(SAMPLE_RATE_SINGLE == message.payload.sample_rate)    { return bme280_set_mode(BME280_MODE_FORCED);}
  if(SAMPLE_RATE_NO_CHANGE == message.payload.sample_rate) { return NRF_SUCCESS; }
  
  //Round sample rate down.
  ruuvi_sensor_configuration_t* payload = (ruuvi_sensor_configuration_t*)message.payload;
  ret_code_t err_code = NRF_SUCCESS;
  if(payload->sample_rate == 1){ err_code |= bme280_set_interval(BME280_STANDBY_1000_MS); }
  else if(payload->sample_rate == 2)  { err_code |= bme280_set_interval(BME280_STANDBY_500_MS); }
  else if(payload->sample_rate <= 8)  { err_code |= bme280_set_interval(BME280_STANDBY_125_MS); }
  else if(payload->sample_rate <= 16) { err_code |= bme280_set_interval(BME280_STANDBY_62_5_MS);}  
  else if(payload->sample_rate <= 200){ err_code |= bme280_set_interval(BME280_STANDBY_0_5_MS); }  
  else { err_code |= BME280_RET_ILLEGAL; }
	err_code |= bme280_set_mode(BME280_MODE_NORMAL);
	
	return err_code;

}

typedef enum {
  SCALE_MIN        = 251,
  SCALE_MAX        = 252,
  SCALE_NO_CHANGE  = 255
}ruuvi_scale_t;

static ret_code_t set_transmission_rate(uint8_t transmission_rate)
{
  return ENDPOINT_NOT_IMPLEMENTED; //TODO: implement 
}

static ret_code_t set_resolution(uint8_t resolution)
{
  // BME280 has only one resolution for each sensor, return success on valid value
  if(RESOLUTION_MIN       == resolution) { return ENDPOINT_SUCCESS; }
  if(RESOLUTION_MAX       == resolution) { return ENDPOINT_SUCCESS; }
  if(RESOLUTION_NO_CHANGE == resolution) { return ENDPOINT_SUCCESS; }
  return ENDPOINT_NOT_SUPPORTED; //Resolution cannot be changed, return error.
}

static ret_code_t set_scale(uint8_t scale)
{
  // BME280 has only one scale for each sensor, return success on valid value
  if(RESOLUTION_MIN       == resolution) { return ENDPOINT_SUCCESS; }
  if(RESOLUTION_MAX       == resolution) { return ENDPOINT_SUCCESS; }
  if(RESOLUTION_NO_CHANGE == resolution) { return ENDPOINT_SUCCESS; }
  return ENDPOINT_NOT_SUPPORTED; //Scale cannot be changed, return error
}

static ret_code_t set_dsp_function(uint8_t dsp_function)
{
  if(DSP_LAST == dsp_function){ return ENDPOINT_SUCCESS; }
  return ENDPOINT_NOT_IMPLEMENTED; //TODO
}

static ret_code_t set_dsp_parameter(uint8_t dsp_parameter)
{
  return ENDPOINT_NOT_IMPLEMENTED; //TODO
}

static ret_code_t set_target(uint8_t target)
{
  if(TRANSMISSION_TARGET_NO_CHANGE == target) { return ENDPOINT_SUCCESS; }
  if(TRANSMISSION_TARGET_STOP == target)
  {
  //Stop application timers
  
  //NULL handlers
  }
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
  
  }
  return 1; //TODO: not implemented err_code
}

static ret_code_t transmit(){

}

//prevent recursing into BME280 config functions.
static bool synch_lock = false;
static ret_code_t configure_sensor(message)
{
  synch_lock = true;
  bme280_set_mode(BME280_MODE_SLEEP); //Sleep sensor while configuring
                                      //TODO: Pass message to BME280 pressure, humidity handlers  

  

  ruuvi_standard_message_t reply = { .destination_endpoint = message.soure_endpoint,
                                     .source_endpoint      = message.destination_endpoint,
                                     .type                 = ACKNOWLEDGEMENT,
                                     .payload              = {0}};

  
  synch_lock = false;
}


ret_code_t bme280_temperature_handler(const ruuvi_standard_message_t message)
{
  if(TEMPERATURE != message.destination_endpoint){ return 1; } //TODO Proper error code

  switch(message.type)
  {
  
    case SENSOR_CONFIGURATION:
      return configure_sensor(message);
      break;
      
    case STATUS_QUERY: 
      return query_sensor(message);
      break;
      
    case DATA_QUERY:
      return read_sensor(message);
      break;
      
    case LOG_QUERY:
      return unknown_handler(message); //TODO - add not implemented?
      break;
      
    case CAPABILITY_QUERY:
      return unknown_handler(message); //TODO - add not implemented?
      break;
      
    default:
      return unknown_handler(message);
      break;
  }
  return NRF_SUCCESS; //TODO: Should not be reached
}
