/* Copyright (c) 2015 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

/** @file
 *
 * @defgroup experimental_ble_sdk_app_eddystone_main main.c
 * @{
 * @ingroup experimental_ble_sdk_app_eddystone
 * @brief Eddystone Beacon UID Transmitter sample application main file.
 *
 * This file contains the source code for an Eddystone beacon transmitter sample application.
 */

#include <stdbool.h>
#include <stdint.h>
#include "ble_advdata.h"
#include "nordic_common.h"
#include "softdevice_handler.h"
#include "bsp.h"
#include "app_timer.h"
#include "nrf_temp.h"
#include "bme280.h"
#include "nrf_delay.h"
#include "spi.h"
#include "base91.h"
#include "nrf_log.h"

#define swap_u32(num) ((num>>24)&0xff) | ((num<<8)&0xff0000) | ((num>>8)&0xff00) | ((num<<24)&0xff000000);
#define float2fix(a) ((int)((a)*256.0))         						  //Convert float to fix. a is a float

#define IS_SRVC_CHANGED_CHARACT_PRESENT 0                                 /**< Include the service changed characteristic. If not enabled, the server's database cannot be changed for the lifetime of the device. */

#define CENTRAL_LINK_COUNT              0                                 /**< Number of central links used by the application. When changing this number remember to adjust the RAM settings*/
#define PERIPHERAL_LINK_COUNT           0                                 /**< Number of peripheral links used by the application. When changing this number remember to adjust the RAM settings*/

#define APP_CFG_NON_CONN_ADV_TIMEOUT    0                                 /**< Time for which the device must be advertising in non-connectable mode (in seconds). 0 disables the time-out. */
#define NON_CONNECTABLE_ADV_INTERVAL    MSEC_TO_UNITS(100, UNIT_0_625_MS) /**< The advertising interval for non-connectable advertisement (100 ms). This value can vary between 100 ms and 10.24 s). */

// Eddystone common data
#define APP_EDDYSTONE_UUID              0xFEAA                            /**< UUID for Eddystone beacons according to specification. */
#define APP_EDDYSTONE_RSSI              240                              /**< 0xEE = -18 dB is the approximate signal strength at 0 m. */

// Eddystone UID data
#define APP_EDDYSTONE_UID_FRAME_TYPE    0x00                              /**< UID frame type is fixed at 0x00. */
#define APP_EDDYSTONE_UID_RFU           0x00, 0x00                        /**< Reserved for future use according to specification. */
#define APP_EDDYSTONE_UID_ID            0x01, 0x02, 0x03, 0x04, \
                                        0x05, 0x06                        /**< Mock values for 6-byte Eddystone UID ID instance.  */
#define APP_EDDYSTONE_UID_NAMESPACE     0xAA, 0xAA, 0xBB, 0xBB, \
                                        0xCC, 0xCC, 0xDD, 0xDD, \
                                        0xEE, 0xEE                        /**< Mock values for 10-byte Eddystone UID ID namespace. */

// Eddystone URL data
#define APP_EDDYSTONE_URL_FRAME_TYPE    0x10                              /**< URL Frame type is fixed at 0x10. */
#define APP_EDDYSTONE_URL_SCHEME        0x03                              /**< 0x03 = "https://" URL prefix scheme according to specification. */

// Eddystone TLM data
#define APP_EDDYSTONE_TLM_FRAME_TYPE    0x20                              /**< TLM frame type is fixed at 0x20. */
#define APP_EDDYSTONE_TLM_VERSION       0x00                              /**< TLM version might change in the future to accommodate other data according to specification. */
#define APP_EDDYSTONE_TLM_BATTERY       0x00, 0xF0                        /**< Mock value. Battery voltage in 1 mV per bit. */
#define APP_EDDYSTONE_TLM_TEMPERATURE   0x0F, 0x00                        /**< Mock value. Temperature [C]. Signed 8.8 fixed-point notation. */
#define APP_EDDYSTONE_TLM_ADV_COUNT     0x00, 0x00, 0x00, 0x00            /**< Running count of advertisements of all types since power-up or reboot. */
#define APP_EDDYSTONE_TLM_SEC_COUNT     0x00, 0x00, 0x00, 0x00            /**< Running count in 0.1 s resolution since power-up or reboot. */

#define DEAD_BEEF                       0xDEADBEEF                        /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

#define APP_TIMER_PRESCALER             0                                 /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_OP_QUEUE_SIZE         4                                 /**< Size of timer operation queues. */

volatile uint32_t time = 0;

static ble_gap_adv_params_t m_adv_params;                                 /**< Parameters to be passed to the stack when starting advertising. */


// BASE91
static struct basE91 b91;
char url_buffer[19] = {'r', 'u', 'u', '.', 'v', 'i', '#'};
size_t enc_data_len = 0;



/**@brief Callback function for asserts in the SoftDevice.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in]   line_num   Line number of the failing ASSERT call.
 * @param[in]   file_name  File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}


// Configure Eddystone-URL frame
static void advertise_url_init(void)
{
    uint32_t      err_code;
    ble_advdata_t advdata;
    uint8_t       flags = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    ble_uuid_t    adv_uuids[] = {{APP_EDDYSTONE_UUID, BLE_UUID_TYPE_BLE}};

    uint8_array_t eddystone_data_array;                             // Array for Service Data structure.
/** @snippet [Eddystone data array] */
    eddystone_data_array.p_data = (uint8_t *) url_buffer;   // Pointer to the data to advertise.
    // eddystone_data_array.size = sizeof(url_buffer);         // Size of the data to advertise.
    eddystone_data_array.size = 10 + enc_data_len;         // Size of the data to advertise.

/** @snippet [Eddystone data array] */

    ble_advdata_service_data_t service_data;                        // Structure to hold Service Data.
    service_data.service_uuid = APP_EDDYSTONE_UUID;                 // Eddystone UUID to allow discoverability on iOS devices.
    service_data.data = eddystone_data_array;                       // Array for service advertisement data.

    // Build and set advertising data.
    memset(&advdata, 0, sizeof(advdata));

    advdata.name_type               = BLE_ADVDATA_NO_NAME;
    advdata.flags                   = flags;
    advdata.uuids_complete.uuid_cnt = sizeof(adv_uuids) / sizeof(adv_uuids[0]);
    advdata.uuids_complete.p_uuids  = adv_uuids;
    advdata.p_service_data_array    = &service_data;                // Pointer to Service Data structure.
    advdata.service_data_count      = 1;

    err_code = ble_advdata_set(&advdata, NULL);
    APP_ERROR_CHECK(err_code);

    // Initialize advertising parameters (used when starting advertising).
    memset(&m_adv_params, 0, sizeof(m_adv_params));

    m_adv_params.type        = BLE_GAP_ADV_TYPE_ADV_NONCONN_IND;
    m_adv_params.p_peer_addr = NULL;                                // Undirected advertisement.
    m_adv_params.fp          = BLE_GAP_ADV_FP_ANY;
    m_adv_params.interval    = NON_CONNECTABLE_ADV_INTERVAL;
    m_adv_params.timeout     = APP_CFG_NON_CONN_ADV_TIMEOUT;
}




/**@brief Function for starting advertising **/
static void advertising_start(void)
{
    uint32_t err_code;
    // Initialize advertising parameters (used when starting advertising).
    memset(&m_adv_params, 0, sizeof(m_adv_params));

    m_adv_params.type        = BLE_GAP_ADV_TYPE_ADV_NONCONN_IND;
    m_adv_params.p_peer_addr = NULL;                                // Undirected advertisement.
    m_adv_params.fp          = BLE_GAP_ADV_FP_ANY;
    m_adv_params.interval    = NON_CONNECTABLE_ADV_INTERVAL;
    m_adv_params.timeout     = APP_CFG_NON_CONN_ADV_TIMEOUT;

    err_code = sd_ble_gap_adv_start(&m_adv_params);
    APP_ERROR_CHECK(err_code);

    err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing the BLE stack.
 * @details Initializes the SoftDevice and the BLE event interrupt.
 */
static void ble_stack_init(void)
{
    uint32_t err_code;
    
    nrf_clock_lf_cfg_t clock_lf_cfg = NRF_CLOCK_LFCLKSRC;
    
    // Initialize the SoftDevice handler module.
    SOFTDEVICE_HANDLER_INIT(&clock_lf_cfg, NULL);
    
    ble_enable_params_t ble_enable_params;
    err_code = softdevice_enable_get_default_config(CENTRAL_LINK_COUNT,
                                                    PERIPHERAL_LINK_COUNT,
                                                    &ble_enable_params);
    APP_ERROR_CHECK(err_code);
    
    //Check the ram settings against the used number of links
    CHECK_RAM_START_ADDR(CENTRAL_LINK_COUNT,PERIPHERAL_LINK_COUNT);
    
    // Enable BLE stack.
    err_code = softdevice_enable(&ble_enable_params);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for doing power management.
 */
static void power_manage(void)
{
    uint32_t err_code = sd_app_evt_wait();
    APP_ERROR_CHECK(err_code);
}

// Sensor values
struct ruuvi_sensor_t
{
uint8_t     format;         // Includes time format
uint8_t     humidity;      	// one lsb is 0.5%
uint16_t	temperature;    // Signed 8.8 fixed-point notation.
uint16_t    pressure;       // Todo
uint16_t    time;           // Seconds, minutes or hours from beginning
};

// Last sent values
struct temp_ruuvi_sensor_t
{
uint8_t     format; 
uint8_t     humidity;  
uint16_t	temperature; 
uint16_t    pressure;
uint16_t    time;  
};


/**
 * @brief Function for application main entry.
 */
int main(void)
{    
    uint32_t err_code = NRF_LOG_INIT(); //XXX UNUSED
    if(!err_code)
    {
        NRF_LOG("Initializing RuuviTag b2...\n");
    }

    float temperature = 0;
    float pressure = 0;
    float humidity = 0;
    float temperature_tot = 0;
    float pressure_tot = 0;
    float humidity_tot = 0;   
    //float temperature_temp = 0;
    //float pressure_temp = 0;
    //float humidity_temp = 0;
    uint8_t mainloop_count = 0;
    char log_buffer[100] = {0};
    
	struct ruuvi_sensor_t sensordata;
    struct temp_ruuvi_sensor_t sensortemp;
	
	spi_initialize();
	
	nrf_gpio_pin_dir_set(LED_2, NRF_GPIO_PIN_DIR_OUTPUT);
	nrf_gpio_cfg_output(LED_2); 
	nrf_gpio_pin_clear(LED_2); 
    
	nrf_delay_ms(100);
	
	BME280Settings settings;
	settings.humidOverSample = 1;
	settings.pressOverSample = 1;
	settings.tempOverSample = 1;
	settings.runMode = 3;
	settings.filter = 0;
	settings.tStandby = 0;
	
	bme280_initialize(settings);
    //NRF_LOG_PRINTF(0, "BME280 init completed.\n");
	//NRF_LOG_PRINTF(0, "Temperature %f!\n", readTemperature());
	
    APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, false);
    err_code = bsp_init(BSP_INIT_LED, APP_TIMER_TICKS(100, APP_TIMER_PRESCALER), NULL);
    APP_ERROR_CHECK(err_code);
    ble_stack_init();

    LEDS_ON(LEDS_MASK);

    // BASE91 INIT
    basE91_init(&b91);
	
    
	char buffer_base91_out [50] = {0};
    
    // Fill the URL buffer (No sensor readings yet) 
    url_buffer[0] = APP_EDDYSTONE_URL_FRAME_TYPE; // Eddystone URL frame type
    url_buffer[1] = APP_EDDYSTONE_RSSI; // RSSI value at 0 m
    url_buffer[2] = APP_EDDYSTONE_URL_SCHEME; // Scheme or prefix for URL ("http", "http://www", etc.)
    url_buffer[3] = 0x72; // r
    url_buffer[4] = 0x75; // u
    url_buffer[5] = 0x75; // u
    url_buffer[6] = 0x2e; // .
    url_buffer[7] = 0x76; // v
    url_buffer[8] = 0x69; // i
    url_buffer[9] = 0x23; // #        // URL with a maximum length of 17 bytes. Last byte is suffix (".com", ".org", etc.)
    url_buffer[10] = 0x2e; // .
    url_buffer[11] = 0x2e; // .
    url_buffer[12] = 0x2e; // .
    url_buffer[13] = 0x2e; // .
    url_buffer[14] = 0x2e; // .
    url_buffer[15] = 0x2e; // .
    url_buffer[16] = 0x2e; // .
    url_buffer[17] = 0x2e; // .
    url_buffer[18] = 0x2e; // .
    
    
    advertise_url_init(); // Initialize Eddystone-URL
    advertising_start(); // Start execution
    
    time = 0; // Reset time counter
    
    // Enter main loop.
    for (;;)
    {	        
        power_manage();
        
        nrf_delay_ms(1000);    
        
        /** Measure n times and calculate average **/
        temperature_tot = 0;
        pressure_tot = 0;
        humidity_tot = 0;
        for (int i = 0; i<10; i++) {
            temperature_tot += readTemperature();
            pressure_tot += readPressure();
            humidity_tot += readHumidity();
            nrf_delay_ms(100);
        }
        temperature = temperature_tot / 10;
        pressure = pressure_tot / 10;
        humidity = humidity_tot / 10;
 
        // Fill the data structure here
        sensordata.format 		= 1;
        sensordata.humidity 	= (uint8_t)(humidity*2);
        sensordata.temperature 	= float2fix((double)temperature);
        sensordata.pressure		= (uint16_t)((double)pressure-50000.0);
        sensordata.time			= time;
        
        
        
        // Check if need to encode only if the same URL is sent already for 5 mainloop cycles
        if ((mainloop_count >= 5))
        {
            // Encode only if the values has changed. Otherwise advertise the old value
            if ((sensordata.temperature != sensortemp.temperature)
                || (sensordata.pressure != sensortemp.pressure) 
                || (sensordata.humidity != sensortemp.humidity))
            {
                sprintf(log_buffer, "Humidity: %f, Temperature: %f, Pressure: %f\n", humidity, temperature, pressure);
                NRF_LOG(log_buffer);
	            NRF_LOG("\n\r");
                
                sprintf (log_buffer, "Format: %d, Humi: %d, Temp: %d, Press: %d, Time: %d\n", sensordata.format, sensordata.humidity, sensordata.temperature, sensordata.pressure, sensordata.time);
                NRF_LOG(log_buffer);
	            NRF_LOG("\n\r");

                
                // Encode using Base91 library
                memset(&buffer_base91_out, 0, sizeof(buffer_base91_out)); 
                enc_data_len =	basE91_encode(&b91, &sensordata, sizeof(sensordata), buffer_base91_out);
                enc_data_len += basE91_encode_end(&b91, buffer_base91_out + enc_data_len);
                memset(&b91, 0, sizeof(b91));

                // Fill the URL buffer
                url_buffer[0] = APP_EDDYSTONE_URL_FRAME_TYPE;
                url_buffer[1] = APP_EDDYSTONE_RSSI;
                url_buffer[2] = APP_EDDYSTONE_URL_SCHEME;
                url_buffer[3] = 0x72; // r
                url_buffer[4] = 0x75; // u
                url_buffer[5] = 0x75; // u
                url_buffer[6] = 0x2e; // .
                url_buffer[7] = 0x76; // v
                url_buffer[8] = 0x69; // i
                url_buffer[9] = 0x23; // #        

                /** We've got 18-7=11 characters available.
                    Encoding 64 bits using Base91 produces max 9 value. All good. **/
                memcpy(&url_buffer[10], &buffer_base91_out, enc_data_len);
                
                NRF_LOG("Encoded: ");
	            NRF_LOG(buffer_base91_out);
	            NRF_LOG("\n\r");
                
                if (enc_data_len != 9) advertise_url_init(); // Initialize again
                
                // Remember the sent values so that we can compare later do we want to encode again or not
                sensortemp.humidity = sensordata.humidity;
                sensortemp.temperature = sensordata.temperature;
                sensortemp.pressure = sensordata.pressure;
                mainloop_count = 1;
            }
        } 
        else
        {
            mainloop_count++;
        }
        time++;
    }
}


/**
 * @}
 */
