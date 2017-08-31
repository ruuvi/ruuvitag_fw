#include "bme280_temperature_handler.h"
#include "ruuvi_endpoints.h"
#include "nrf_error.h"
#include "bme280.h"
#include "nrf_delay.h"

#define NRF_LOG_MODULE_NAME "BME280_TEMPERATURE_HANDLER"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

/** Data target handlers **/
static message_handler p_ble_adv_handler = NULL;
static message_handler p_ble_gatt_handler = NULL;
static message_handler p_ble_mesh_handler = NULL;
static message_handler p_proprietary_handler = NULL;
static message_handler p_nfc_handler = NULL;
static message_handler p_ram_handler = NULL;
static message_handler p_flash_handler = NULL;

/** State variables **/
static ruuvi_sensor_configuration_t m_configuration = {0};
static ruuvi_endpoint_t m_destination_endpoint = PLAINTEXT_MESSAGE; //TODO: use something 

//TODO: Add timer interrupt to sample rate to read samples 
/**     This must be called as last function, as this function may bring BME280 out of sleep which prevents further configuration  **/
static ret_code_t set_sample_rate(uint8_t sample_rate)
{
  ret_code_t err_code = BME280_RET_OK;
  if(SAMPLE_RATE_NO_CHANGE == sample_rate) { return ENDPOINT_SUCCESS; }
  else if(SAMPLE_RATE_STOP == sample_rate)
  { 
    err_code |= bme280_set_mode(BME280_MODE_SLEEP); 
    if(BME280_RET_OK == err_code) { m_configuration.sample_rate = sample_rate; }
    return err_code;
  }
  else if(SAMPLE_RATE_SINGLE == sample_rate)
  { 
    err_code |= bme280_set_mode(BME280_MODE_FORCED); 
    if(BME280_RET_OK == err_code) { m_configuration.sample_rate = SAMPLE_RATE_STOP; } //Sampling stops after one-shot
    return err_code;
  }
  
  //Round sample rate down.
  if(sample_rate == 1){ err_code |= bme280_set_interval(BME280_STANDBY_1000_MS); }
  else if(sample_rate == 2)  { err_code |= bme280_set_interval(BME280_STANDBY_500_MS); }
  else if(sample_rate <= 8)  { err_code |= bme280_set_interval(BME280_STANDBY_125_MS); }
  else if(sample_rate <= 16) { err_code |= bme280_set_interval(BME280_STANDBY_62_5_MS);}  
  else if(sample_rate <= 200){ err_code |= bme280_set_interval(BME280_STANDBY_0_5_MS); }  
  else { err_code |= BME280_RET_ILLEGAL; }
	err_code |= bme280_set_mode(BME280_MODE_NORMAL);
  if(BME280_RET_OK == err_code) { m_configuration.sample_rate = sample_rate; }
	
	return err_code;

}

static ret_code_t set_transmission_rate(uint8_t transmission_rate)
{
  return ENDPOINT_NOT_IMPLEMENTED; //TODO: implement 
}

static ret_code_t set_resolution(uint8_t resolution)
{
  // BME280 has only one resolution for each sensor, return success on valid value, mark as MAX
  m_configuration.resolution = RESOLUTION_MAX;
  if(RESOLUTION_MIN       == resolution) { return ENDPOINT_SUCCESS; }
  if(RESOLUTION_MAX       == resolution) { return ENDPOINT_SUCCESS; }
  if(RESOLUTION_NO_CHANGE == resolution) { return ENDPOINT_SUCCESS; }
  return ENDPOINT_NOT_SUPPORTED; //Resolution cannot be changed, return error.
}

static ret_code_t set_scale(uint8_t scale)
{
  // BME280 has only one scale for each sensor, return success on valid value, mark as MAX
  m_configuration.scale = SCALE_MAX;
  if(SCALE_MIN       == scale) { return ENDPOINT_SUCCESS; }
  if(SCALE_MAX       == scale) { return ENDPOINT_SUCCESS; }
  if(SCALE_NO_CHANGE == scale) { return ENDPOINT_SUCCESS; }
  return ENDPOINT_NOT_SUPPORTED; //Scale cannot be changed, return error
}

static ret_code_t set_dsp_function(uint8_t dsp_function)
{
  if(DSP_LAST == dsp_function)
  {
    m_configuration.dsp_function = DSP_LAST;
    return ENDPOINT_SUCCESS; 
  }
  return ENDPOINT_NOT_IMPLEMENTED; //TODO
}

static ret_code_t set_dsp_parameter(uint8_t dsp_parameter)
{
  m_configuration.dsp_parameter = 1;
  return ENDPOINT_NOT_IMPLEMENTED; //TODO
}

static ret_code_t set_target(uint8_t target)
{
  NRF_LOG_INFO("Setting targets %d\r\n", target);
  if(TRANSMISSION_TARGET_NO_CHANGE == target) { return ENDPOINT_SUCCESS; }
  if(TRANSMISSION_TARGET_STOP == target)
  {
    //Stop application timers - TODO
  
    //NULL handlers
    p_ble_adv_handler = NULL;
    p_ble_gatt_handler = NULL;
    p_ble_mesh_handler = NULL;
    p_proprietary_handler = NULL;
    p_nfc_handler = NULL;
    p_ram_handler = NULL;
    p_flash_handler = NULL;
    m_configuration.target = target;
    return ENDPOINT_SUCCESS;
  }
  if(TRANSMISSION_TARGET_BLE_GATT & target)
  {
  p_ble_gatt_handler = get_ble_gatt_handler();
  NRF_LOG_INFO("Setting up GATT handler\r\n");
  }
  if(TRANSMISSION_TARGET_BLE_ADV & target){p_ble_adv_handler = get_ble_adv_handler();}
  if(TRANSMISSION_TARGET_BLE_MESH & target){p_ble_mesh_handler = get_ble_mesh_handler();}
  if(TRANSMISSION_TARGET_PROPRIETARY & target){ p_proprietary_handler = get_proprietary_handler(); }
  if(TRANSMISSION_TARGET_NFC & target){ p_nfc_handler = get_nfc_handler(); }
  if(TRANSMISSION_TARGET_RAM == target){ p_ram_handler = get_ram_handler(); }
  if(TRANSMISSION_TARGET_FLASH == target){ p_flash_handler = get_flash_handler(); }

  return ENDPOINT_SUCCESS;
}
/** Send transmission to all data endpoints. 
 */
static ret_code_t transmit(const ruuvi_standard_message_t message)
{
  NRF_LOG_INFO("Transmitting to all data points\r\n");  
  ret_code_t err_code = ENDPOINT_SUCCESS;
  if(p_ble_adv_handler)     { err_code |= p_ble_adv_handler(message); }
  if(p_ble_gatt_handler)    { err_code |= p_ble_gatt_handler(message); }
  if(p_proprietary_handler) { err_code |= p_proprietary_handler(message); }
  if(p_nfc_handler)         { err_code |= p_nfc_handler(message); }
  if(p_ram_handler)         { err_code |= p_ram_handler(message); }
  if(p_flash_handler)       { err_code |= p_flash_handler(message); }
  return err_code;
}

//prevent recursing into BME280 config functions with lock bit.
static bool synch_lock = false;
static ret_code_t configure_sensor(const ruuvi_standard_message_t message)
{
  synch_lock = true;
  NRF_LOG_INFO("Configuring sensor:");
  NRF_LOG_HEXDUMP_INFO((uint8_t*)&(message.destination_endpoint), sizeof(message));
  NRF_LOG_INFO("\r\n");
  bme280_set_mode(BME280_MODE_SLEEP); // Sleep sensor while configuring
                                      // TODO: Pass message to BME280 pressure, humidity handlers  

  //Return codes are truncated to 8 bits.
  ruuvi_sensor_configuration_t result = {0};
  NRF_LOG_DEBUG("Casting payload\r\n");
  ruuvi_sensor_configuration_t* payload = (void*)&(message.payload[0]);
  NRF_LOG_HEXDUMP_INFO((uint8_t*)payload, sizeof(ruuvi_sensor_configuration_t));
  NRF_LOG_DEBUG("Transmission rate\r\n");
  result.transmission_rate = set_transmission_rate(payload->transmission_rate);
  NRF_LOG_DEBUG("Resolution\r\n");  
  result.resolution = set_resolution(payload->resolution);
  NRF_LOG_DEBUG("Scale\r\n");  
  result.scale = set_scale(payload->scale);
  NRF_LOG_DEBUG("DSP\r\n");  
  result.dsp_function = set_dsp_function(payload->dsp_function);
  NRF_LOG_DEBUG("DSP_Param\r\n");  
  result.dsp_parameter = set_dsp_parameter(payload->dsp_parameter);
  NRF_LOG_DEBUG("Target %d\r\n", payload->target);  
  result.target = set_target(message.payload[6]);
  //Call sample rate as last as this may bring sensor out of sleep
  NRF_LOG_DEBUG("Sample rate\r\n");    
  result.sample_rate = set_sample_rate(payload->sample_rate);
  
  //Store endpoint request came from, even if message will not be processed due to error (TODO?)
  m_destination_endpoint = message.source_endpoint;

  ruuvi_standard_message_t reply = { .destination_endpoint = message.source_endpoint,
                                     .source_endpoint      = message.destination_endpoint,
                                     .type                 = ACKNOWLEDGEMENT,
                                     .payload              = {*((uint8_t*)&result)}}; //TODO: Check cast (value of address of configuration casted to uint8_t)
  synch_lock = false;
  NRF_LOG_INFO("Configuration done\r\n");  
  //Return error if cannot reply
  ret_code_t err_code = ENDPOINT_HANDLER_ERROR;
  message_handler p_reply_handler = get_reply_handler();
  if(p_reply_handler) 
  {  
    NRF_LOG_INFO("Sending reply from configuration\r\n");
    err_code = p_reply_handler(reply);
  }
  return err_code; //Error codes from configuration are in payload of reply
}

static ret_code_t read_sensor(const ruuvi_standard_message_t message)
{
  ret_code_t err_code = ENDPOINT_SUCCESS;
  err_code |= bme280_read_measurements(); // Read measured values from BME - TODO separate error codes for driver / endpoint.
  //Take temperature
  int32_t temperature = bme280_get_temperature();
  NRF_LOG_INFO("Sending reply from temperature: %d\r\n", (uint32_t)temperature);
  // Plaintext reply if message came from PLAINTEXT endpoint
  if(PLAINTEXT_MESSAGE == message.source_endpoint)
  {
    char ascii[8] = {0};
    sprintf(ascii, "%ld.%ld C", temperature/100, temperature%100); // TODO Check decimals on negative values
    ruuvi_standard_message_t reply = {.destination_endpoint = message.source_endpoint,
                                      .source_endpoint = TEMPERATURE,
                                      .type = ASCII,
                                      .payload = {0}};
    memcpy(&(reply.payload[0]), &(ascii[0]), sizeof(reply.payload));                                  
    NRF_LOG_INFO("Sending plain text %s\r\n", (uint32_t)reply.payload);  
    err_code |= transmit(reply);
  }
  //Else INT64 reply
  else 
  {
    int64_t rvalue = temperature;
    uint8_t* cast  = (void*)&rvalue;
    NRF_LOG_INFO("Sending raw reply\r\n");  
    ruuvi_standard_message_t reply = {.destination_endpoint = message.source_endpoint,
                                      .source_endpoint = TEMPERATURE,
                                      .type = ASCII,
                                      .payload = {cast[0]}}; //TODO: Check the casts
    err_code |= transmit(reply);
  }
  return err_code;
}

ret_code_t bme280_temperature_handler(const ruuvi_standard_message_t message)
{
  //Return if message was not meant for this endpoint.
  NRF_LOG_INFO("Message type is %d\r\n", message.type);
  if(TEMPERATURE != message.destination_endpoint){ return ENDPOINT_INVALID; }
  switch(message.type)
  {
  
    case SENSOR_CONFIGURATION:
      NRF_LOG_INFO("Configuring\r\n");
      return configure_sensor(message);
      break;
      
    case STATUS_QUERY: 
      return unknown_handler(message);
      //return query_sensor(message); TODO
      break;
      
    case DATA_QUERY:
      NRF_LOG_INFO("Querying\r\n");
      return read_sensor(message);
      break;
      
    case LOG_QUERY:
      return unknown_handler(message);
      break;
      
    case CAPABILITY_QUERY:
      return unknown_handler(message);
      break;
      
    default:
      return unknown_handler(message);
      break;
  }
  return ENDPOINT_HANDLER_ERROR; // Should not be reached
}

