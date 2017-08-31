#include "lis2dh12_acceleration_handler.h"
#include "ruuvi_endpoints.h"
#include "nrf_error.h"
#include "lis2dh12.h"
#include "math.h"

#define NRF_LOG_MODULE_NAME "LIS2DH12_HANDLER"
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
static ruuvi_endpoint_t m_destination_endpoint = MAM;

static ret_code_t set_sample_rate(uint8_t sample_rate)
{
  ret_code_t err_code = LIS2DH12_RET_OK;
  NRF_LOG_INFO("Setting sample_rate %d\r\n", sample_rate);
  if(SAMPLE_RATE_NO_CHANGE == sample_rate) { return ENDPOINT_SUCCESS; }
  else if(SAMPLE_RATE_STOP == sample_rate)
  { 
    err_code |= lis2dh12_set_sample_rate(LIS2DH12_RATE_0);
    lis2dh12_set_fifo_mode(LIS2DH12_MODE_BYPASS);
    if(LIS2DH12_RET_OK == err_code) { m_configuration.sample_rate = sample_rate; }
    return err_code;
  }

  // TODO
  else if(SAMPLE_RATE_SINGLE == sample_rate){ return ENDPOINT_NOT_IMPLEMENTED; }
  
  //Round sample rate down, enable FIFO
  err_code |= lis2dh12_set_fifo_mode(LIS2DH12_MODE_STREAM);
  if(sample_rate == 1)       { err_code |= lis2dh12_set_sample_rate(LIS2DH12_RATE_1);  }
  else if(sample_rate <= 10) { err_code |= lis2dh12_set_sample_rate(LIS2DH12_RATE_10); }
  else if(sample_rate <= 25) { err_code |= lis2dh12_set_sample_rate(LIS2DH12_RATE_25); }
  else if(sample_rate <= 50) { err_code |= lis2dh12_set_sample_rate(LIS2DH12_RATE_50); }  
  else if(sample_rate <= 100){ err_code |= lis2dh12_set_sample_rate(LIS2DH12_RATE_100); }  
  else if(sample_rate <= 200){ err_code |= lis2dh12_set_sample_rate(LIS2DH12_RATE_200); }    
  else if(sample_rate <= 400){ err_code |= lis2dh12_set_sample_rate(LIS2DH12_RATE_400); }    
  else { err_code |= LIS2DH12_RET_INVALID; }
  
    if(LIS2DH12_RET_OK == err_code) 
    { 
        m_configuration.sample_rate = sample_rate; 
        
    }
  
    return err_code;
}

static ret_code_t set_transmission_rate(uint8_t transmission_rate)
{
  ret_code_t err_code = ENDPOINT_SUCCESS;
  NRF_LOG_INFO("Setting transmission_rate %d\r\n", transmission_rate);
  switch(transmission_rate)
  {
    case TRANSMISSION_RATE_STOP:
        err_code |= lis2dh12_set_interrupts(LIS2DH12_NO_INTERRUPTS);        
        break;
    case TRANSMISSION_RATE_SAMPLERATE:
        NRF_LOG_INFO("Enabling LIS interrupts\r\n");
        err_code |= lis2dh12_set_fifo_watermark(30);
        err_code |= lis2dh12_set_interrupts(LIS2DH12_I1_WTM);
        break;

    case TRANSMISSION_RATE_DSPRATE:
        err_code |= ENDPOINT_NOT_IMPLEMENTED;
        break;

    case TRANSMISSION_RATE_NO_CHANGE:
        break;
    default :
        err_code |= ENDPOINT_NOT_IMPLEMENTED;
        break;
  }
  return err_code;
}

static ret_code_t set_resolution(uint8_t resolution)
{
    ret_code_t err_code = ENDPOINT_SUCCESS;
    NRF_LOG_DEBUG("Setting resolution %d\r\n", resolution);
    switch (resolution)
    {
        case RESOLUTION_MIN:
            err_code |= lis2dh12_set_resolution(LIS2DH12_RES8BIT);
            break;

        case RESOLUTION_MAX:
            err_code |= lis2dh12_set_resolution(LIS2DH12_RES12BIT);
            break;

        case RESOLUTION_NO_CHANGE:
            break;

        case 8:
            err_code |= lis2dh12_set_resolution(LIS2DH12_RES8BIT);
            break;
        
        case 10:
            err_code |= lis2dh12_set_resolution(LIS2DH12_RES10BIT);
            break;
        
        case 12:
            err_code |= lis2dh12_set_resolution(LIS2DH12_RES12BIT);
            break;                        
  
        default:
            err_code |= ENDPOINT_NOT_SUPPORTED;
    }
    if(ENDPOINT_SUCCESS == err_code) { m_configuration.resolution = resolution; }
    return err_code;
}

static ret_code_t set_scale(uint8_t scale)
{
    ret_code_t err_code = ENDPOINT_SUCCESS;
    NRF_LOG_DEBUG("Setting scale %d\r\n", scale);
    switch(scale)
    {
        case SCALE_MIN:
            err_code |= lis2dh12_set_scale(LIS2DH12_SCALE2G);
            break;

        case SCALE_MAX:
            err_code |= lis2dh12_set_scale(LIS2DH12_SCALE16G);
            break;

        case SCALE_NO_CHANGE:
            break;
    
        case 2:
            err_code |= lis2dh12_set_scale(LIS2DH12_SCALE2G);
            break; 

        case 4:
            err_code |= lis2dh12_set_scale(LIS2DH12_SCALE4G);
            break;

        case 8:
            err_code |= lis2dh12_set_scale(LIS2DH12_SCALE8G);
            break;

        case 16:
            err_code |= lis2dh12_set_scale(LIS2DH12_SCALE16G);
            break; 
     }

     return err_code;
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
  NRF_LOG_DEBUG("Setting targets %d\r\n", target);
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
  NRF_LOG_DEBUG("Setting up GATT handler\r\n");
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
  NRF_LOG_DEBUG("Transmitting to all data points\r\n");  
  ret_code_t err_code = ENDPOINT_SUCCESS;
  if(p_ble_adv_handler)     { err_code |= p_ble_adv_handler(message); }
  if(p_ble_gatt_handler)    { err_code |= p_ble_gatt_handler(message); }
  if(p_proprietary_handler) { err_code |= p_proprietary_handler(message); }
  if(p_nfc_handler)         { err_code |= p_nfc_handler(message); }
  if(p_ram_handler)         { err_code |= p_ram_handler(message); }
  if(p_flash_handler)       { err_code |= p_flash_handler(message); }
  return err_code;
}

static ret_code_t configure_sensor(const ruuvi_standard_message_t message)
{
  NRF_LOG_DEBUG("Configuring sensor:");
  NRF_LOG_HEXDUMP_DEBUG((uint8_t*)&(message.destination_endpoint), sizeof(message));
  NRF_LOG_DEBUG("\r\n");

  //Return codes are truncated to 8 bits.
  ruuvi_sensor_configuration_t result = {0};
  NRF_LOG_DEBUG("Casting payload\r\n");
  ruuvi_sensor_configuration_t* payload = (void*)&(message.payload[0]);
  NRF_LOG_HEXDUMP_DEBUG((uint8_t*)payload, sizeof(ruuvi_sensor_configuration_t));
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

  NRF_LOG_DEBUG("Configuration result:");
  NRF_LOG_HEXDUMP_DEBUG((uint8_t*)&(result.sample_rate), sizeof(result));
  NRF_LOG_DEBUG("\r\n");
  
  //Store endpoint request came from, even if message will not be processed due to error (TODO?)
  m_destination_endpoint = message.source_endpoint;

  ruuvi_standard_message_t reply = { .destination_endpoint = message.source_endpoint,
                                     .source_endpoint      = message.destination_endpoint,
                                     .type                 = ACKNOWLEDGEMENT,
                                     .payload              = {*((uint8_t*)&result)}}; //TODO: Check cast (value of address of configuration casted to uint8_t)
  //Return error if cannot reply
  ret_code_t err_code = ENDPOINT_HANDLER_ERROR;
  message_handler p_reply_handler = get_reply_handler();
  if(p_reply_handler) 
  {  
    NRF_LOG_DEBUG("Sending reply from configuration\r\n");
    err_code = p_reply_handler(reply);
  }
  return err_code; //Error codes from configuration are in payload of reply
}

static ret_code_t read_sensor(const ruuvi_standard_message_t message)
{
    ret_code_t err_code = ENDPOINT_SUCCESS;
  
    lis2dh12_sensor_buffer_t buffer; 
    err_code |= lis2dh12_read_samples(&buffer, 1);
    uint16_t rvalue[4];
    rvalue[0] = buffer.sensor.x;
    rvalue[1] = buffer.sensor.y;
    rvalue[2] = buffer.sensor.z;
    rvalue[3] = sqrt(rvalue[0]*rvalue[0] + rvalue[1]*rvalue[1] + rvalue[2]*rvalue[2]);
    uint8_t* cast  = (void*)&rvalue;
    NRF_LOG_DEBUG("Sending raw reply\r\n");  
    ruuvi_standard_message_t reply = {.destination_endpoint = message.source_endpoint,
                                    .source_endpoint = TEMPERATURE,
                                    .type = ASCII,
                                    .payload = {cast[0]}}; //TODO: Check the casts
    err_code |= transmit(reply);
    return err_code;
}

ret_code_t lis2dh12_acceleration_handler(const ruuvi_standard_message_t message)
{
  //Return if message was not meant for this endpoint.
  NRF_LOG_DEBUG("Message type is %d\r\n", message.type);
  if(ACCELERATION != message.destination_endpoint){ return ENDPOINT_INVALID; }
  switch(message.type)
  {
  
    case SENSOR_CONFIGURATION:
      NRF_LOG_DEBUG("Configuring\r\n");
      return configure_sensor(message);
      break;
      
    case STATUS_QUERY: 
      return unknown_handler(message);
      //return query_sensor(message); TODO
      break;
      
    case DATA_QUERY:
      NRF_LOG_DEBUG("Querying\r\n");
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

/** Scheduler handler to read accelerometer buffer **/
static uint32_t exercise_value = 0;
void lis2dh12_scheduler_event_handler(void *p_event_data, uint16_t event_size)
{
    NRF_LOG_DEBUG("Accelerometer scheduled function\r\n");
    size_t count = 0;
    lis2dh12_get_fifo_sample_number(&count);
    lis2dh12_sensor_buffer_t buffer[32];
    memset(buffer, 0, sizeof(buffer));
    lis2dh12_read_samples(buffer, count);
    for(int ii = 0; ii < count; ii++)
    {
        int16_t rvalue[4];
        rvalue[0] = buffer[ii].sensor.x;
        rvalue[1] = buffer[ii].sensor.y;
        rvalue[2] = buffer[ii].sensor.z;
        rvalue[3] = sqrt(rvalue[0]*rvalue[0] + rvalue[1]*rvalue[1] + rvalue[2]*rvalue[2]);
        /*
        uint8_t* cast  = (void*)&rvalue;
        ruuvi_standard_message_t message;
        message.source_endpoint = ACCELERATION;
        message.destination_endpoint = m_destination_endpoint;
        message.type = UINT16;
        
        NRF_LOG_INFO("Sending raw reply\r\n");  
        ruuvi_standard_message_t reply = {.destination_endpoint = message.source_endpoint,
                                        .source_endpoint = TEMPERATURE,
                                        .type = ASCII,
                                        .payload = {cast[0]}}; //TODO: Check the casts
        transmit(reply);
        */
        //XXX Hack for the hackathon
        int16_t accumulation = rvalue[3] - 1200;
        if(accumulation > 0) { exercise_value += accumulation; }
        //NRF_LOG_INFO("%d %d %d %d\r\n", rvalue[0], rvalue[1], rvalue[2], rvalue[3])
    }

    NRF_LOG_INFO("Accumulated exercise %d\r\n", exercise_value);
}

//XXX Hackathon
uint32_t get_exercise(void)
{
    return exercise_value;
}

/** Handle interrupt from lis2dh12, schedule sensor read & transmit **/
ret_code_t lis2dh12_int1_handler(const ruuvi_standard_message_t message)
{
    NRF_LOG_DEBUG("Accelerometer interrupt\r\n");
   
    app_sched_event_put ((void*)(&message),
                         sizeof(message),
                         lis2dh12_scheduler_event_handler);
    return NRF_SUCCESS;
}