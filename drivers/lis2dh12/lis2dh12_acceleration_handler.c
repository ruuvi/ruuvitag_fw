#include "lis2dh12_acceleration_handler.h"
#include "ruuvi_endpoints.h"
#include "nrf_error.h"
#include "lis2dh12.h"
#include "math.h"

#define NRF_LOG_MODULE_NAME "LIS2DH12_HANDLER"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"


static message_handler_state_t m_state = {0};

static ret_code_t set_sample_rate(uint8_t sample_rate)
{
  ret_code_t err_code = LIS2DH12_RET_OK;
  NRF_LOG_INFO("Setting sample_rate %d\r\n", sample_rate);
  if(SAMPLE_RATE_NO_CHANGE == sample_rate) { return ENDPOINT_SUCCESS; }
  else if(SAMPLE_RATE_STOP == sample_rate)
  { 
    err_code |= lis2dh12_set_sample_rate(LIS2DH12_RATE_0);
    lis2dh12_set_fifo_mode(LIS2DH12_MODE_BYPASS);
    if(LIS2DH12_RET_OK == err_code) { m_state.configuration.sample_rate = sample_rate; }
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
    m_state.configuration.sample_rate = sample_rate; 
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
        err_code |= lis2dh12_set_interrupts(LIS2DH12_NO_INTERRUPTS, 1);        
        break;
    case TRANSMISSION_RATE_SAMPLERATE:
        NRF_LOG_DEBUG("Enabling LIS interrupts\r\n");
        err_code |= lis2dh12_set_fifo_watermark(30);
        err_code |= lis2dh12_set_interrupts(LIS2DH12_I1_WTM, 1);
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
  if(LIS2DH12_RET_OK == err_code) { m_state.configuration.transmission_rate = transmission_rate; }  
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
    if(ENDPOINT_SUCCESS == err_code) { m_state.configuration.resolution = resolution; }
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
        
        default:
            err_code |= ENDPOINT_NOT_SUPPORTED;
            break;
     }     
     if(ENDPOINT_SUCCESS == err_code) { m_state.configuration.scale = scale; }

     return err_code;
}

static ret_code_t set_dsp_function(uint8_t dsp_function)
{
  if(DSP_LAST == dsp_function)
  {
    m_state.configuration.dsp_function = DSP_LAST;
    return ENDPOINT_SUCCESS; 
  }
  return ENDPOINT_NOT_IMPLEMENTED; //TODO
}

static ret_code_t set_dsp_parameter(uint8_t dsp_parameter)
{
  m_state.configuration.dsp_parameter = 1;
  return ENDPOINT_NOT_IMPLEMENTED; //TODO
}

/** 
 *  Setup targets to which data will be sent. 
 *  Note: Chaining is done separately.
 *  TODO: Can a function pointer / other code deduplication be used?
 */
static ret_code_t set_target(uint8_t target)
{
  NRF_LOG_INFO("Setting targets %d\r\n", target);
  if(TRANSMISSION_TARGET_NO_CHANGE == target) { return ENDPOINT_SUCCESS; }

  //NULL handlers
  m_state.p_ble_adv_handler = NULL;
  m_state.p_ble_gatt_handler = NULL;
  m_state.p_ble_mesh_handler = NULL;
  m_state.p_proprietary_handler = NULL;
  m_state.p_nfc_handler = NULL;
  m_state.p_ram_handler = NULL;
  m_state.p_flash_handler = NULL;
  m_state.configuration.target = target;  
  
  if(TRANSMISSION_TARGET_STOP == target)
  {
    //Stop application timers - TODO
    return ENDPOINT_SUCCESS;
  }
  
  //Resetup handlers
  if(TRANSMISSION_TARGET_BLE_GATT & target)
  {
  m_state.p_ble_gatt_handler = get_ble_gatt_handler();
  NRF_LOG_DEBUG("Setting up GATT handler\r\n");
  }
  if(TRANSMISSION_TARGET_BLE_ADV & target){m_state.p_ble_adv_handler = get_ble_adv_handler();}
  if(TRANSMISSION_TARGET_BLE_MESH & target){m_state.p_ble_mesh_handler = get_ble_mesh_handler();}
  if(TRANSMISSION_TARGET_PROPRIETARY & target){ m_state.p_proprietary_handler = get_proprietary_handler(); }
  if(TRANSMISSION_TARGET_NFC & target){ m_state.p_nfc_handler = get_nfc_handler(); }
  if(TRANSMISSION_TARGET_RAM == target){ m_state.p_ram_handler = get_ram_handler(); }
  if(TRANSMISSION_TARGET_FLASH == target){ m_state.p_flash_handler = get_flash_handler(); }

  return ENDPOINT_SUCCESS;
}
/** 
 *  Send transmission to all data endpoints.
 *  TODO: Can a function pointer / other code deduplication be used?
 */
static ret_code_t transmit(const ruuvi_standard_message_t message)
{
  ret_code_t err_code = ENDPOINT_SUCCESS;
  NRF_LOG_DEBUG("Transmitting to all data points\r\n");  
  if(m_state.p_ble_adv_handler)     { err_code |= m_state.p_ble_adv_handler(message); }
  if(m_state.p_ble_gatt_handler)    { err_code |= m_state.p_ble_gatt_handler(message); }
  if(m_state.p_proprietary_handler) { err_code |= m_state.p_proprietary_handler(message); }
  if(m_state.p_nfc_handler)         { err_code |= m_state.p_nfc_handler(message); }
  if(m_state.p_ram_handler)         { err_code |= m_state.p_ram_handler(message); }
  if(m_state.p_flash_handler)       { err_code |= m_state.p_flash_handler(message); }
  if(m_state.p_chain_handler)
  { 
    ruuvi_standard_message_t chainmsg;
    memcpy(&chainmsg, &message, sizeof(ruuvi_standard_message_t));
    //Send message upstream to chain
    chainmsg.destination_endpoint = m_state.downstream_endpoint;
    NRF_LOG_DEBUG("Chaining to %d\r\n", chainmsg.destination_endpoint);
    err_code |= m_state.p_chain_handler(chainmsg);
  }
  
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
  m_state.destination_endpoint = message.source_endpoint;

  ruuvi_standard_message_t reply = { .destination_endpoint = message.source_endpoint,
                                     .source_endpoint      = message.destination_endpoint,
                                     .type                 = ACKNOWLEDGEMENT,
                                     .payload              = {*((uint8_t*)&result)}};
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

/**
 *  Read function is also responsble for passing the raw data to chained listener.
 *  Transmit function is responsible for determining if data should be sent.
 */
static ret_code_t read_sensor(const ruuvi_standard_message_t message)
{
    //TODO: Return DSP state
    ret_code_t err_code = ENDPOINT_SUCCESS;
  
    lis2dh12_sensor_buffer_t buffer; 
    err_code |= lis2dh12_read_samples(&buffer, 1);
    uint16_t rvalue[4];
    rvalue[0] = buffer.sensor.x;
    rvalue[1] = buffer.sensor.y;
    rvalue[2] = buffer.sensor.z;
    rvalue[3] = sqrt(rvalue[0]*rvalue[0] + rvalue[1]*rvalue[1] + rvalue[2]*rvalue[2]);
    NRF_LOG_DEBUG("Sending raw reply\r\n");  
    ruuvi_standard_message_t reply = {.destination_endpoint = message.source_endpoint,
                                    .source_endpoint = ACCELERATION,
                                    .type = UINT16,
                                    .payload = { 0 }};
    memcpy(reply.payload, rvalue, sizeof(reply.payload));
    err_code |= transmit(reply);
    return err_code;
}

/**
 *  Copy function pointer address to which to send the data to be chained
 *  TODO: Check how this could be deduplicated
 */
static ret_code_t configure_chain_downstream(const ruuvi_standard_message_t message)
{
  ret_code_t err_code = ENDPOINT_SUCCESS;
  // Return on invalid message type
  if(CHAIN_DOWNSTREAM_CONFIGURATION != message.type) { return ENDPOINT_HANDLER_ERROR; }
  ruuvi_chain_configuration_t* config = (void*)&message.payload;
  // Stop transmitting if transmission rate is 0
  if(TRANSMISSION_RATE_STOP == config->transmission_rate) 
  {
    m_state.downstream_endpoint = 0;
    m_state.p_chain_handler = NULL;
  }

  //Else configure data chain
  else
  {  
    //Get chain handler
    m_state.p_chain_handler      = get_chain_handler();

    //Get target endpoint
    m_state.downstream_endpoint  = message.source_endpoint;
  }
  
  //Reply via reply handler if applicable
  message_handler p_reply_handler = get_reply_handler();
  if(p_reply_handler) 
  {  
    ruuvi_standard_message_t reply = { .destination_endpoint = message.source_endpoint,
                                       .source_endpoint      = message.destination_endpoint,
                                       .type                 = ACKNOWLEDGEMENT,
                                       .payload              = { 0 }};  
    NRF_LOG_DEBUG("Sending reply after configuring Downstream Endpoint %d\r\n", m_state.downstream_endpoint);
    err_code = p_reply_handler(reply);
  }
  return err_code;
}

/**
 *  Handles incoming messages.
 */
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
      
    case CHAIN_DOWNSTREAM_CONFIGURATION:
      NRF_LOG_INFO("Setting up message chain\r\n");
      return configure_chain_downstream(message);
      break;
      
    case CHAIN_UPSTREAM_CONFIGURATION:
      NRF_LOG_ERROR("Sensor cannot be upstream target\r\n");
      return ENDPOINT_INVALID;
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

/** Process sensor data, I.E. transmit data onwards **/
static void process(const ruuvi_standard_message_t message)
{
  if(TRANSMISSION_RATE_SAMPLERATE == m_state.configuration.transmission_rate)
  {
    transmit(message);
  }
}

/** Scheduler handler to read accelerometer buffer **/
void lis2dh12_scheduler_event_handler(void *p_event_data, uint16_t event_size)
{
    NRF_LOG_DEBUG("Accelerometer scheduled function\r\n");
    size_t count = 0;
    lis2dh12_get_fifo_sample_number(&count);
    lis2dh12_sensor_buffer_t buffer[32];
    memset(buffer, 0, sizeof(buffer));
    lis2dh12_read_samples(buffer, count);
    NRF_LOG_DEBUG("Sending raw UINT16 reply\r\n");
    for(int ii = 0; ii < count; ii++)
    {
        int16_t rvalue[4];
        rvalue[0] = buffer[ii].sensor.x;
        rvalue[1] = buffer[ii].sensor.y;
        rvalue[2] = buffer[ii].sensor.z;
        rvalue[3] = sqrt(rvalue[0]*rvalue[0] + rvalue[1]*rvalue[1] + rvalue[2]*rvalue[2]);
        ruuvi_standard_message_t reply = {.destination_endpoint = m_state.destination_endpoint,
                                        .source_endpoint = ACCELERATION,
                                        .type = INT16,
                                        .payload = {0}};
        memcpy(reply.payload, rvalue, sizeof(reply.payload));

        // All samples are sent to processing.
        process(reply);

        NRF_LOG_DEBUG("%d %d %d %d\r\n", rvalue[0], rvalue[1], rvalue[2], rvalue[3]);
    }
}

/** 
 *  Handle interrupt from lis2dh12, schedule sensor read & transmit
 *  Never do long actions, such as sensor reads in interrupt context.
 *  Using peripherals in interrupt is also risky, as peripherals might require interrupts for their function.
 **/
ret_code_t lis2dh12_int1_handler(const ruuvi_standard_message_t message)
{
    NRF_LOG_DEBUG("Accelerometer interrupt\r\n");

    app_sched_event_put ((void*)(&message),
                         sizeof(message),
                         lis2dh12_scheduler_event_handler);
    return NRF_SUCCESS;
}
