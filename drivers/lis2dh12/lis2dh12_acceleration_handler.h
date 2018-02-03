#ifndef LIS2DH12_ACCELERATION_HANDLER_H
#define LIS2DH12_ACCELERATION_HANDLER_H

/**
 *  WIP handler for Ruuvi configuration
 *  Is used with Ruuvi endpoints, supports streaming data to chain channels.
 *
 *  Usage: Call acceleration handler with Ruuvi Standard message (check ruuvi endpoints)
 *  to configure the sensor.
 *  
 *  When setting up pin interrupts, interrupt handler should be called when interrupt occurs.
 *  Interrupt handler will then schedule call to scheduler event handler, passing given message as context.
 *  Scheduler event handler will then read the data and pass it onwards in the application.
 *
 *  
 */

#include "ruuvi_endpoints.h"
#include "nrf_error.h"

/**
 *  Handle messages with "ACCELERATION" as destination endpoint. This should not be called directly, but rather
 *  through ruuvi_endpoints function route_message. 
 *
 *  Usage: 
 *  #include "ruuvi_endpoints.h"
 *  ruuvi_standard_message_t start;
 *  start.destination_endpoint = ACCELERATION;
 *  start.source_endpoint      = 0x60; //ANY, will be destination endpoint for data tx
 *  start.type                 = SENSOR_CONFIGURATION; 
 *  ruuvi_sensor_configuration_t config;
 *  config.sample_rate = 10; //Hz
 *  config.transmission_rate = TRANSMISSION_RATE_SAMPLERATE; //Send every sample
 *  config.resolution = 10;   // bits
 *  config.scale = SCALE_MAX; //maximum scale
 *  config.dsp_function  = DSP_LAST; //send only latest value, no DSP
 *  config.dsp_parameter = 1; //ANY
 *  config.target = TRANSMISSION_TARGET_BLE_GATT; //Send measurements with BLE GATT
 *  mempcy(&start.payload, &config, sizeof(config));
 *  route_message(start); //Starts sampling acceleration at maximum scale, 10 bit resolution, 10 Hz and sends data to BLE GATT
 *
 *  Returns error code from endpoint, i.e. ENDPOINT_SUCCESS if message was understood.
 */
ret_code_t lis2dh12_acceleration_handler(const ruuvi_standard_message_t message);

/**
 *  Initialization assings this function, user should not need to care about the implementation.
 *
 *  Schedules reading accelerometer
 */
ret_code_t lis2dh12_int1_handler(const ruuvi_standard_message_t message);

/*
 *  Reads accelerometer values and calls transmit() if transmission rate equals sample rate
 */
void lis2dh12_scheduler_event_handler(void *p_event_data, uint16_t event_size);

#endif
