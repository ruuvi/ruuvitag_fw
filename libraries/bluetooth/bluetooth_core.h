/**
 * Interface for using Nordic SDKs Bluetooth core functions
 * such as init, advertise etc.
 */

#include <stdbool.h>
#include <stdint.h>
#include "ble_advdata.h"
#include "bluetooth_config.h"
#include "nordic_common.h"
#include "softdevice_handler.h"
#include "app_timer.h"
#include "bsp.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"

void ble_stack_init(void);
