#include "chain_channels.h"
#include "ruuvi_endpoints.h"
#include "dsp.h"


//TODO: Refactor had dependency to nRF52 scheduler out of library
#include "app_scheduler.h"
#include "app_timer_appsh.h"
#include "init.h" // timer prescaler
APP_TIMER_DEF(ch0_timer);
APP_TIMER_DEF(ch1_timer);
APP_TIMER_DEF(ch2_timer);
APP_TIMER_DEF(ch3_timer);
APP_TIMER_DEF(ch4_timer);
APP_TIMER_DEF(ch5_timer);
APP_TIMER_DEF(ch6_timer);
APP_TIMER_DEF(ch7_timer);
APP_TIMER_DEF(ch8_timer);
APP_TIMER_DEF(ch9_timer);
APP_TIMER_DEF(chA_timer);
APP_TIMER_DEF(chB_timer);
APP_TIMER_DEF(chC_timer);
APP_TIMER_DEF(chD_timer);
APP_TIMER_DEF(chE_timer);
APP_TIMER_DEF(chF_timer);
static const app_timer_id_t* p_timers[] = {&ch0_timer, &ch1_timer, &ch2_timer, &ch3_timer,
                                           &ch4_timer, &ch5_timer, &ch6_timer, &ch7_timer,
                                           &ch8_timer, &ch9_timer, &chA_timer, &chB_timer,
                                           &chC_timer, &chD_timer, &chE_timer, &chF_timer};


#define NRF_LOG_MODULE_NAME "CHAIN"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

static message_handler_state_t m_states[NUM_CHAIN_CHANNELS];
static message_handler_state_t* p_state = NULL;
static uint8_t m_chain_index = 0;

//TODO: Deduplicate
static ret_code_t set_dsp(uint8_t dsp_function, uint8_t dsp_parameter)
{
  ret_code_t status = ENDPOINT_NOT_IMPLEMENTED;
  switch(dsp_function)
  {
    case DSP_LAST:
      p_state->configuration.dsp_function  = DSP_LAST;
      p_state->configuration.dsp_parameter = 1; //TODO: Store n last samples?
      status = ENDPOINT_SUCCESS; 
      break;
    case DSP_STDEV:
      NRF_LOG_INFO("Setting up STDEV filtering for chain %d, parameter %d\r\n", m_chain_index, dsp_parameter);
      p_state->configuration.dsp_function = DSP_STDEV;
      p_state->configuration.dsp_parameter = dsp_parameter;
      for(size_t ii = 0; ii < MAX_DSP_STATES; ii++)
      {
        if(dsp_is_init(&(p_state->dsp[ii])))
        {
          dsp_uninit(&(p_state->dsp[ii]));
        }
        p_state->dsp[ii] = dsp_init(dsp_function, dsp_parameter);
      }
      break;

    default: 
      break;
  }
  return status;
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
  p_state->p_ble_adv_handler = NULL;
  p_state->p_ble_gatt_handler = NULL;
  p_state->p_ble_mesh_handler = NULL;
  p_state->p_proprietary_handler = NULL;
  p_state->p_nfc_handler = NULL;
  p_state->p_ram_handler = NULL;
  p_state->p_flash_handler = NULL;
  p_state->configuration.target = target;  
  
  if(TRANSMISSION_TARGET_STOP == target)
  {
    //Stop application timers - TODO
    return ENDPOINT_SUCCESS;
  }
  
  //Resetup handlers
  if(TRANSMISSION_TARGET_BLE_GATT & target)
  {
  p_state->p_ble_gatt_handler = get_ble_gatt_handler();
  NRF_LOG_DEBUG("Setting up GATT handler\r\n");
  }
  if(TRANSMISSION_TARGET_BLE_ADV & target){p_state->p_ble_adv_handler = get_ble_adv_handler();}
  if(TRANSMISSION_TARGET_BLE_MESH & target){p_state->p_ble_mesh_handler = get_ble_mesh_handler();}
  if(TRANSMISSION_TARGET_PROPRIETARY & target){ p_state->p_proprietary_handler = get_proprietary_handler(); }
  if(TRANSMISSION_TARGET_NFC & target){ p_state->p_nfc_handler = get_nfc_handler(); }
  if(TRANSMISSION_TARGET_RAM == target){ p_state->p_ram_handler = get_ram_handler(); }
  if(TRANSMISSION_TARGET_FLASH == target){ p_state->p_flash_handler = get_flash_handler(); }

  return ENDPOINT_SUCCESS;
}

/**
 * 
 */
static ret_code_t set_transmission_rate(const uint8_t rate)
{
  ret_code_t err_code = ENDPOINT_SUCCESS;
  if(TRANSMISSION_RATE_STOP == rate)
  {
    p_state->p_chain_handler = NULL;
    err_code |= app_timer_stop(*(p_timers[m_chain_index]));
  }
  //Else configure data chain
  else
  {
    //Get chain handler
    p_state->p_chain_handler = get_chain_handler();
    //seconds, TODO: define constants
    if(rate < 60) { err_code |=  app_timer_start(*(p_timers[m_chain_index]), APP_TIMER_TICKS(1000 * (rate), APP_TIMER_PRESCALER), p_state); }
    //Minutes
    else if(rate < 120) { err_code |=  app_timer_start(*(p_timers[m_chain_index]), APP_TIMER_TICKS(60000 * (rate - 59), APP_TIMER_PRESCALER), p_state); }
    //Hours - todo: Check that values are possible with given prescaler
    else if(rate < 250) { err_code |=  app_timer_start(*(p_timers[m_chain_index]), APP_TIMER_TICKS(3600000 * (rate - 119), APP_TIMER_PRESCALER), p_state); }
    NRF_LOG_INFO("Setting up transmission rate %d, status %d\r\n", rate, err_code);
  }
  return err_code;
}

/** 
 *  Send transmission to all data endpoints.
 *  TODO: Can a function pointer / other code deduplication be used?
 */
static ret_code_t transmit(const ruuvi_standard_message_t message)
{
  ret_code_t err_code = ENDPOINT_SUCCESS;
  NRF_LOG_DEBUG("Transmitting to all data points\r\n");  
  if(p_state->p_ble_adv_handler)     { err_code |= p_state->p_ble_adv_handler(message); }
  if(p_state->p_ble_gatt_handler)    { err_code |= p_state->p_ble_gatt_handler(message); }
  if(p_state->p_proprietary_handler) { err_code |= p_state->p_proprietary_handler(message); }
  if(p_state->p_nfc_handler)         { err_code |= p_state->p_nfc_handler(message); }
  if(p_state->p_ram_handler)         { err_code |= p_state->p_ram_handler(message); }
  if(p_state->p_flash_handler)       { err_code |= p_state->p_flash_handler(message); }
  if(p_state->p_chain_handler)       { err_code |= p_state->p_chain_handler(message); }
  return err_code;
}

/**
 *  Configure upstream channel.
 */
static ret_code_t configure_upstream_endpoint(const ruuvi_standard_message_t message)
{
  ruuvi_standard_message_t configuration;
  configuration.source_endpoint = m_chain_index + ENDPOINT_CHAIN_OFFSET;
  ruuvi_chain_configuration_t* p_config = (void*)&message.payload;
  configuration.destination_endpoint = p_config->upstream_endpoint;
  configuration.type = CHAIN_DOWNSTREAM_CONFIGURATION;
  memcpy(&configuration.payload, &message.payload, sizeof(message.payload));
  route_message(configuration);
  //Sensor will acknowledge to reply_handler
  return ENDPOINT_SUCCESS;
}

static ret_code_t configure_chain_upstream(const ruuvi_standard_message_t message)
{
  NRF_LOG_DEBUG("Configuring upstream endpoint: \r\n");
  NRF_LOG_HEXDUMP_DEBUG((uint8_t*)&(message.destination_endpoint), sizeof(message));
  NRF_LOG_DEBUG("\r\n");

  //Return codes are truncated to 8 bits.
  ruuvi_chain_configuration_t result = {0};
  NRF_LOG_DEBUG("Casting payload\r\n");
  ruuvi_chain_configuration_t* payload = (void*)message.payload;
  NRF_LOG_HEXDUMP_DEBUG((uint8_t*)payload, sizeof(ruuvi_sensor_configuration_t));
  NRF_LOG_DEBUG("Transmission rate\r\n");
  result.transmission_rate = set_transmission_rate(payload->transmission_rate);
  NRF_LOG_DEBUG("DSP\r\n");
  result.dsp_function = set_dsp(payload->dsp_function, payload->dsp_parameter);
  NRF_LOG_DEBUG("Target %d\r\n", payload->target);
  result.target = set_target(payload->target);
  NRF_LOG_DEBUG("Data source\r\n");
  result.upstream_endpoint = configure_upstream_endpoint(message);

  NRF_LOG_DEBUG("Configuration result:");
  NRF_LOG_HEXDUMP_DEBUG((uint8_t*)&result, sizeof(result));
  NRF_LOG_DEBUG("\r\n");
  
  //Store endpoint request came from, even if message will not be processed due to error (TODO?)
  p_state->destination_endpoint = message.source_endpoint;

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
 *  Read current DSP value and transmit it onwards.
 */
static ret_code_t read_value_i16(const ruuvi_standard_message_t message)
{
  ret_code_t err_code = ENDPOINT_SUCCESS;
  int16_t values[4];
  for(size_t ii = 0; ii < 4; ii++)
  {
    NRF_LOG_DEBUG("Processing DSP CH %d\r\n", ii);
    dsp_filter_t* p_filter = &(p_state->dsp[ii]);
    float next = p_filter->read(&(p_filter->z), p_filter->dsp_parameter);
    //TODO: Check under/overflows
    values[ii] = (int16_t)next; 
  }

  ruuvi_standard_message_t reply = {.destination_endpoint = message.destination_endpoint,
                                    .source_endpoint = (m_chain_index + ENDPOINT_CHAIN_OFFSET),
                                    .type = message.type,
                                    .payload = { 0 }};
  memcpy(reply.payload, values, sizeof(reply.payload));
  NRF_LOG_DEBUG("DSP: X: %d Y: %d Z: %d SUM %d\r\n", values[0], values[1], values[2], values[3]);
  err_code |= transmit(reply);
  return err_code;
}



/**
 *  Configure this endpoint as a downstream endpoint, i.e. receiver of data.
 *  Upstream must be configured separately to send data to this endpoint.
 *  TODO: Check how this could be deduplicated
 */
static ret_code_t configure_chain_downstream(const ruuvi_standard_message_t message)
{
  // Return on invalid message type
  if(CHAIN_DOWNSTREAM_CONFIGURATION != message.type) { return ENDPOINT_HANDLER_ERROR; }
  ruuvi_chain_configuration_t* config = (void*)&message.payload;
  // Stop transmitting if transmission rate is 0
  if(TRANSMISSION_RATE_STOP == config->transmission_rate)
  {
    p_state->p_chain_handler = NULL;
    app_timer_stop(*(p_timers[m_chain_index]));
  }
  //Else configure data chain
  else
  {
    //Get chain handler
    p_state->p_chain_handler = get_chain_handler();
    //seconds, TODO: define constants
    if(config->transmission_rate < 60) { app_timer_start(*(p_timers[m_chain_index]), APP_TIMER_TICKS(1000 * (config->transmission_rate), APP_TIMER_PRESCALER), p_state); }
    //Minutes
    else if(config->transmission_rate < 120) { app_timer_start(*(p_timers[m_chain_index]), APP_TIMER_TICKS(60000 * (config->transmission_rate - 59), APP_TIMER_PRESCALER), p_state); }
    //Hours - todo: Check that values are possible with given prescaler
    else if(config->transmission_rate < 250) { app_timer_start(*(p_timers[m_chain_index]), APP_TIMER_TICKS(3600000 * (config->transmission_rate - 119), APP_TIMER_PRESCALER), p_state); }
    NRF_LOG_INFO("Setting up transmission rate %d \r\n", config->transmission_rate);
  }
  return ENDPOINT_SUCCESS;
}


/**
 * Call DSP function for each of message values.
 */
static ret_code_t process_i16(const ruuvi_standard_message_t message)
{
  int16_t values[4];
  memcpy(values, message.payload, sizeof(message.payload));
  for(size_t ii = 0; ii < 4; ii++)
  {
    NRF_LOG_DEBUG("Processing DSP CH %d\r\n", ii);
    float next = (float) values[ii];
    dsp_filter_t* p_filter = &(p_state->dsp[ii]);
    NRF_LOG_DEBUG("Filter is init: %d, parameter is %d, next value is %d \r\n", dsp_is_init(p_filter), p_filter->dsp_parameter, values[ii]);
    p_filter->process(&(p_filter->z), p_filter->dsp_parameter, next);
  }
  //If we were configured to transmit each sample, trigger transmission now
  if(TRANSMISSION_RATE_SAMPLERATE == p_state->configuration.transmission_rate)
  {
    read_value_i16(message);
  }
  NRF_LOG_DEBUG("I16 Done\r\n");
  return NRF_SUCCESS;
}

/**
 *  Handles incoming messages.
 */
ret_code_t chain_handler(const ruuvi_standard_message_t message)
{
  //Return if the message was not targeted to chain channel, i.e. data query replies
  if (message.destination_endpoint <  ENDPOINT_CHAIN_OFFSET ||
      message.destination_endpoint >= ENDPOINT_CHAIN_OFFSET + NUM_CHAIN_CHANNELS)
  {
    return ENDPOINT_INVALID;
  }
  // Get index of target chain
  m_chain_index = message.destination_endpoint - ENDPOINT_CHAIN_OFFSET;
  NRF_LOG_DEBUG("Received Chain message to chain %d\r\n", m_chain_index);
  //Store pointer to state of selected chain channel
  p_state = &(m_states[m_chain_index]);
  switch(message.type)
  {
    case SENSOR_CONFIGURATION:
      //Chains are configured with chain configuration messages
      return unknown_handler(message);

    case STATUS_QUERY: 
      return unknown_handler(message);
      //return query_sensor(message); TODO

    case DATA_QUERY:
      NRF_LOG_DEBUG("Querying\r\n");
      return unknown_handler(message);//TODO: Determine data type in chain channel
      //return read_value_i16(message);

    case CHAIN_DOWNSTREAM_CONFIGURATION:
      NRF_LOG_INFO("Configuring downstream to receive data from chain\r\n");
      return configure_chain_downstream(message);
      
    case CHAIN_UPSTREAM_CONFIGURATION:
      NRF_LOG_INFO("Configuring upstream to send data to chain\r\n");
      return configure_chain_upstream(message);

    case LOG_QUERY:
      return unknown_handler(message);

    case CAPABILITY_QUERY:
      return unknown_handler(message);

    //TODO: Separate function for handling data types?
    case INT16:
      NRF_LOG_DEBUG("Processing I16\r\n");
      return process_i16(message);

    default:
      return unknown_handler(message);
  }
  return ENDPOINT_HANDLER_ERROR; // Should not be reached
}

/**
 * Handler to call when transmission data is sent.
 * Send state as a context.
 */
static void chain_transmission_handler(void *p_context)
{
  NRF_LOG_DEBUG("Transmission called\r\n");
  p_state = p_context;
  //Find endpoint index by looking up index of state pointer
  size_t ii = 0;
  for(; ii < NUM_CHAIN_CHANNELS; ii++){
    if(&(m_states[ii]) == p_state) break;
  }
  m_chain_index = ii;
  ruuvi_standard_message_t message = {.destination_endpoint = p_state->destination_endpoint,
                                      .source_endpoint = (m_chain_index + ENDPOINT_CHAIN_OFFSET),
                                      .type = INT16,
                                      .payload = { 0 }};    
  //XXX generalise                                    
  read_value_i16(message);
}

/**
 *  Initializes application timers
 */
ret_code_t chain_handler_init(void)
{
  for(int ii = 0; ii < NUM_CHAIN_CHANNELS; ii++)
  {
    app_timer_create(p_timers[ii], APP_TIMER_MODE_REPEATED, chain_transmission_handler);
  }
  return ENDPOINT_SUCCESS;
}
