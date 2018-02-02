#ifndef BLE_BULK_TRANSFER_H
#define BLE_BULK_TRANSFER_H
#include <stdint.h>
#include <stdlib.h>

#include "nrf_error.h"
#include "ble_nus.h"

#include "ruuvi_endpoints.h"

// TODO: Move to a separate config file?
#define BLE_BULK_QUEUE_SIZE 10
#define BLE_CHUNK_SIZE 18
#define BLE_BULK_TX_MAX_SIZE (255*BLE_CHUNK_SIZE)
#define BLE_RAW_SIZE BLE_NUS_MAX_DATA_LEN
#define BLE_BULK_HEADER_SIZE 4

//Large enough queue for 32 FiFo samples by default
#ifndef BLE_STD_QUEUE_SIZE
   #define BLE_STD_QUEUE_SIZE 40
#endif

typedef struct{
  const ruuvi_endpoint_t endpoint;
  const uint8_t index;
  const uint8_t chunks;
  const uint8_t CRC8; 
}ble_bulk_tx_header_t;

typedef struct{
  ble_bulk_tx_header_t header;
  uint8_t* data;
  const ruuvi_endpoint_t endpoint;
  const size_t length;
  uint8_t* chunk_index;
}ble_bulk_tx_t;

typedef enum{
  TX_SUCCESS = 0,
  TX_ERROR_MAX_SIZE_EXCEEDED = 1
}bulk_transfer_ret_t;

bulk_transfer_ret_t ble_bulk_transfer_asynchronous(const ruuvi_endpoint_t endpoint, uint8_t* data, const size_t length);

ret_code_t ble_std_transfer_asynchronous(const ruuvi_standard_message_t message);

ret_code_t ble_message_queue_process(void);

ret_code_t ble_transfer_raw(uint8_t* data, size_t length);

ret_code_t ble_bulk_message_queue_purge(void);

ret_code_t ble_bulk_message_clean(ble_bulk_tx_t* element);

void ble_bulk_set_nus(ble_nus_t* nus);

#endif
