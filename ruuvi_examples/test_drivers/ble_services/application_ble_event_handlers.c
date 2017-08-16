#include "application_ble_event_handlers.h"
#include "ble_nus.h"

#define NRF_LOG_MODULE_NAME "APP_BLE_EVENT_HANDLER"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"


/**@brief Function for handling the Application's BLE Stack events.
 *
 * @details Handles all events from the BLE stack of interest to all Bluetooth Developer Studio generated Services.
 *
 * @param[in]   p_ble_evt  Event received from the BLE stack.
 */
extern ble_nus_t m_nus;
void application_on_ble_evt(ble_evt_t * p_ble_evt)
{ 
    NRF_LOG_INFO("BLE_EVENT\r\n");
    ble_nus_on_ble_evt(&m_nus, p_ble_evt);
    /*
    ble_ess_on_ble_evt(&m_ess, p_ble_evt); 
    ble_dis_on_ble_evt(&m_dis, p_ble_evt); 
    ble_bas_on_ble_evt(&m_bas, p_ble_evt); 
    ble_tps_on_ble_evt(&m_tps, p_ble_evt); 
    ble_nordic_uart_service_on_ble_evt(&m_nordic_uart_service, p_ble_evt); 
    */
}


