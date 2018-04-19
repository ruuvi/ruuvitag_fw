#include "application_ble_event_handlers.h"
#include "ble.h"
#include "application_service_if.h"
#include "nrf_ble_es.h"

#define NRF_LOG_MODULE_NAME "APP_BLE_EVENT_HANDLER"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"


/**@brief Function for handling the Application's BLE Stack events.
 *
 * @details Handles all events from the BLE stack of interest to all Bluetooth Developer Studio generated Services.
 *
 * @param[in]   p_ble_evt  Event received from the BLE stack.
 */

void application_on_ble_evt(ble_evt_t * p_ble_evt)
{ 
  /** Return pointer to BLE dfu service **/
  #if BLE_DFU_ENABLED
  ble_dfu_t* p_dfu = get_dfu();
  ble_dfu_on_ble_evt(p_dfu, p_ble_evt);
  #endif 
  
  nrf_ble_es_on_ble_evt(p_ble_evt);
}


