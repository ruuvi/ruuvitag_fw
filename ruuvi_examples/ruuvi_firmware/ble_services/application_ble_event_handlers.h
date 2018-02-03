#ifndef APPLICATION_BLE_EVENT_HANDLERS_H
#define APPLICATION_BLE_EVENT_HANDLERS_H

#include "ble.h"
#include "ble_hci.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"

void application_on_ble_evt(ble_evt_t * p_ble_evt);

#endif
