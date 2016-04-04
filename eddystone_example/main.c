#include <stdbool.h>
#include <stdint.h>
#include <ble_advdata.h>
#include "ble_radio_notification.h"
#include "softdevice_handler.h"
#include "bsp.h"
#include "app_timer.h"
#include "eddystone.h"

#define IS_SRVC_CHANGED_CHARACT_PRESENT 0                                 /**< Include or not the service_changed characteristic. if not enabled, the server's database cannot be changed for the lifetime of the device*/

#define APP_CFG_NON_CONN_ADV_TIMEOUT    0                                 /**< Time for which the device must be advertising in non-connectable mode (in seconds). 0 disables timeout. */
#define NON_CONNECTABLE_ADV_INTERVAL    MSEC_TO_UNITS(100, UNIT_0_625_MS) /**< The advertising interval for non-connectable advertisement (100 ms). This value can vary between 100ms to 10.24s). */

#define DEAD_BEEF                       0xDEADBEEF                        /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

#define APP_TIMER_PRESCALER             0                                 /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_MAX_TIMERS            (2+BSP_APP_TIMERS_NUMBER)         /**< Maximum number of simultaneously created timers. */
#define APP_TIMER_OP_QUEUE_SIZE         4                                 /**< Size of timer operation queues. */


#define APP_MEASURED_RSSI               0xC3                              /**< The Beacon's measured RSSI at 1 meter distance in dBm. */

#define VBAT_MAX_IN_MV                  3300

#define EDDYSTONE_UID                   0
#define EDDYSTONE_URL                   1
#define EDDYSTONE_TLM                   2



#if defined(USE_UICR_FOR_MAJ_MIN_VALUES)
#define MAJ_VAL_OFFSET_IN_BEACON_INFO   18                                /**< Position of the MSB of the Major Value in m_beacon_info array. */
#define UICR_ADDRESS                    0x10001080                        /**< Address of the UICR register used by this example. The major and minor versions to be encoded into the advertising data will be picked up from this location. */
#endif


static edstn_frame_t edstn_frames[3];

static uint32_t pdu_count = 0;

static ble_gap_adv_params_t m_adv_params;                                 /**< Parameters to be passed to the stack when starting advertising. */


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
void assert_nrf_callback(uint16_t line_num, const uint8_t *p_file_name) {
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}


void init_url_frame_buffer() {
    uint8_t *encoded_advdata = edstn_frames[EDDYSTONE_URL].adv_frame;
    uint8_t *len_advdata = &edstn_frames[EDDYSTONE_URL].adv_len;

    eddystone_head_encode(encoded_advdata, 0x10, len_advdata);

    encoded_advdata[(*len_advdata)++] = APP_MEASURED_RSSI;
    encoded_advdata[(*len_advdata)++] = 0x02;
    encoded_advdata[(*len_advdata)++] = 'r';
    encoded_advdata[(*len_advdata)++] = 'u';
    encoded_advdata[(*len_advdata)++] = 'u';
    encoded_advdata[(*len_advdata)++] = 'v';
    encoded_advdata[(*len_advdata)++] = 'i';
    encoded_advdata[(*len_advdata)++] = '.';
    encoded_advdata[(*len_advdata)++] = 'c';
    encoded_advdata[(*len_advdata)++] = 'o';
    encoded_advdata[(*len_advdata)++] = 'm';

    encoded_advdata[0x07] = (*len_advdata) - 8; // Length	Service Data. Ibid. § 1.11
}

void init_uid_frame_buffer() {
    uint8_t *encoded_advdata = edstn_frames[EDDYSTONE_UID].adv_frame;
    uint8_t *len_advdata = &edstn_frames[EDDYSTONE_UID].adv_len;

    eddystone_head_encode(encoded_advdata, 0x00, len_advdata);

    encoded_advdata[(*len_advdata)++] = APP_MEASURED_RSSI;
    encoded_advdata[(*len_advdata)++] = 0x00;
    encoded_advdata[(*len_advdata)++] = 0x01;
    encoded_advdata[(*len_advdata)++] = 0x02;
    encoded_advdata[(*len_advdata)++] = 0x03;
    encoded_advdata[(*len_advdata)++] = 0x04;
    encoded_advdata[(*len_advdata)++] = 0x05;
    encoded_advdata[(*len_advdata)++] = 0x06;
    encoded_advdata[(*len_advdata)++] = 0x07;
    encoded_advdata[(*len_advdata)++] = 0x08;
    encoded_advdata[(*len_advdata)++] = 0x09;

    encoded_advdata[(*len_advdata)++] = 0x00;
    encoded_advdata[(*len_advdata)++] = 0x01;
    encoded_advdata[(*len_advdata)++] = 0x02;
    encoded_advdata[(*len_advdata)++] = 0x03;
    encoded_advdata[(*len_advdata)++] = 0x04;
    encoded_advdata[(*len_advdata)++] = 0x05;
    encoded_advdata[(*len_advdata)++] = 0x06;

    encoded_advdata[0x07] = (*len_advdata) - 8; // Length	Service Data. Ibid. § 1.11
}


uint32_t eddystone_set_adv_data(uint32_t frame_index) {
    uint8_t *p_encoded_advdata = edstn_frames[frame_index].adv_frame;
    return sd_ble_gap_adv_data_set(p_encoded_advdata, edstn_frames[frame_index].adv_len, NULL, 0);
}

uint32_t big32cpy(uint8_t *dest, uint32_t val) {
    dest[3] = (uint8_t) (val >> 0u);
    dest[2] = (uint8_t) (val >> 8u);
    dest[1] = (uint8_t) (val >> 16u);
    dest[0] = (uint8_t) (val >> 24u);
    return 4;
}

uint32_t big16cpy(uint8_t *dest, uint16_t val) {
    dest[1] = (uint8_t) (val >> 0u);
    dest[0] = (uint8_t) (val >> 8u);
    return 4;
}

uint16_t temperature_data_get(void) {
    int32_t temp;
    uint32_t err_code;

    err_code = sd_temp_get(&temp);
    APP_ERROR_CHECK(err_code);

    int8_t hi = (temp / 4);
    int8_t lo = (temp * 25) % 100;

    return hi << 8u | lo;
}

uint8_t battery_level_get(void) {
    #if 0 // Fix this to support nRF52
    // Configure ADC
    NRF_ADC->CONFIG = (ADC_CONFIG_RES_8bit << ADC_CONFIG_RES_Pos) |
                      (ADC_CONFIG_INPSEL_SupplyOneThirdPrescaling << ADC_CONFIG_INPSEL_Pos) |
                      (ADC_CONFIG_REFSEL_VBG << ADC_CONFIG_REFSEL_Pos) |
                      (ADC_CONFIG_PSEL_Disabled << ADC_CONFIG_PSEL_Pos) |
                      (ADC_CONFIG_EXTREFSEL_None << ADC_CONFIG_EXTREFSEL_Pos);
    NRF_ADC->EVENTS_END = 0;
    NRF_ADC->ENABLE = ADC_ENABLE_ENABLE_Enabled;

    NRF_ADC->EVENTS_END = 0;    // Stop any running conversions.
    NRF_ADC->TASKS_START = 1;

    while (!NRF_ADC->EVENTS_END) {
    }

    uint16_t vbg_in_mv = 1200;
    uint8_t adc_max = 255;
    uint16_t vbat_current_in_mv = (NRF_ADC->RESULT * 3 * vbg_in_mv) / adc_max;

    NRF_ADC->EVENTS_END = 0;
    NRF_ADC->TASKS_STOP = 1;

    return (uint8_t) ((vbat_current_in_mv * 100) / VBAT_MAX_IN_MV);
    #endif
    return (uint8_t) 0; // Return 0 instead of the batt value
}

void init_tlm_frame_buffer() {
    uint8_t *encoded_advdata = edstn_frames[EDDYSTONE_TLM].adv_frame;
    uint8_t *len_advdata = &edstn_frames[EDDYSTONE_TLM].adv_len;

    eddystone_head_encode(encoded_advdata, 0x20, len_advdata);
    encoded_advdata[(*len_advdata)++] = 0x00; // Version

//    uint8_t battery_data = battery_level_get();
    encoded_advdata[(*len_advdata)++] = 0x00; // Battery voltage, 1 mV/bit
    encoded_advdata[(*len_advdata)++] = 0x00;

    // Beacon temperature
//    eddystone_uint16(encoded_advdata, len_advdata, temperature_data_get());
    encoded_advdata[(*len_advdata)++] = 0x00;
    encoded_advdata[(*len_advdata)++] = 0x00;

    // Advertising PDU count
//    eddystone_uint32(encoded_advdata, len_advdata, pdu_count);
    encoded_advdata[(*len_advdata)++] = 0x00;
    encoded_advdata[(*len_advdata)++] = 0x00;
    encoded_advdata[(*len_advdata)++] = 0x00;
    encoded_advdata[(*len_advdata)++] = 0x00;


    // Time since power-on or reboot
//    *len_advdata += big32cpy(encoded_advdata + *len_advdata, pdu_count);
    encoded_advdata[(*len_advdata)++] = 0x00;
    encoded_advdata[(*len_advdata)++] = 0x00;
    encoded_advdata[(*len_advdata)++] = 0x00;
    encoded_advdata[(*len_advdata)++] = 0x00;

    encoded_advdata[0x07] = (*len_advdata) - 8; // Length	Service Data. Ibid. § 1.11
}


/**@brief Function for initializing the Advertising functionality.
 *
 * @details Encodes the required advertising data and passes it to the stack.
 *          Also builds a structure to be passed to the stack when starting advertising.
 */
static void advertising_init(void) {
    init_uid_frame_buffer();
    init_url_frame_buffer();
    init_tlm_frame_buffer();
    eddystone_set_adv_data(EDDYSTONE_UID);

    // Initialize advertising parameters (used when starting advertising).
    memset(&m_adv_params, 0, sizeof(m_adv_params));

    m_adv_params.type = BLE_GAP_ADV_TYPE_ADV_NONCONN_IND;
    m_adv_params.p_peer_addr = NULL;                             // Undirected advertisement.
    m_adv_params.fp = BLE_GAP_ADV_FP_ANY;
    m_adv_params.interval = NON_CONNECTABLE_ADV_INTERVAL;
    m_adv_params.timeout = APP_CFG_NON_CONN_ADV_TIMEOUT;
}


/**@brief Function for starting advertising.
 */
static void advertising_start(void) {
    uint32_t err_code;

    err_code = sd_ble_gap_adv_start(&m_adv_params);
    APP_ERROR_CHECK(err_code);

    err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 */
static void ble_stack_init(void) {
    // Initialize the SoftDevice handler module.
    SOFTDEVICE_HANDLER_INIT(NRF_CLOCK_LFCLKSRC_XTAL_20_PPM, NULL);

    // Enable BLE stack 
    uint32_t err_code;
    ble_enable_params_t ble_enable_params;
    memset(&ble_enable_params, 0, sizeof(ble_enable_params));
#ifdef S130
    ble_enable_params.gatts_enable_params.attr_tab_size   = BLE_GATTS_ATTR_TAB_SIZE_DEFAULT;
#endif
    ble_enable_params.gatts_enable_params.service_changed = IS_SRVC_CHANGED_CHARACT_PRESENT;
    err_code = sd_ble_enable(&ble_enable_params);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for doing power management.
 */
static void power_manage(void) {
    uint32_t err_code = sd_app_evt_wait();
    APP_ERROR_CHECK(err_code);
}

// uid uid uri  uid uid uri
void eddystone_interleave(bool radio_active) {
    if (radio_active) {
        if ((pdu_count + 1) % 6 == 0) {
            init_tlm_frame_buffer();
            eddystone_set_adv_data(EDDYSTONE_TLM);
        }
        else if (pdu_count % 3 == 0) {
            eddystone_set_adv_data(EDDYSTONE_URL);
        }
        else {
            eddystone_set_adv_data(EDDYSTONE_UID);
        }
        pdu_count++;
    }
}


/**
 * @brief Function for application main entry.
 */
int main(void) {
    uint32_t err_code;
    // Initialize.
    APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_MAX_TIMERS, APP_TIMER_OP_QUEUE_SIZE, false);
    err_code = bsp_init(BSP_INIT_LED, APP_TIMER_TICKS(100, APP_TIMER_PRESCALER), NULL);
    APP_ERROR_CHECK(err_code);
    ble_stack_init();

    err_code = ble_radio_notification_init(NRF_APP_PRIORITY_LOW,
                                           NRF_RADIO_NOTIFICATION_DISTANCE_5500US, eddystone_interleave);

    advertising_init();

    APP_ERROR_CHECK(err_code);

    // Start execution.
    advertising_start();

    // Enter main loop.
    for (; ;) {
        power_manage();
    }
}


/**
 * @}
 */
