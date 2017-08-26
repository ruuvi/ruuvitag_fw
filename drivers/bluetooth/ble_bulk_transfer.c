#include "ble_bulk_transfer.h"

#include "ble_nus.h"
#include "nrf_queue.h"
#include "nrf_error.h"

#include "ruuvi_endpoints.h"

#define NRF_LOG_MODULE_NAME "BLE_BULK_TX"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

NRF_QUEUE_DEF(ble_bulk_tx_t, m_ble_tx_queue, BLE_BULK_QUEUE_SIZE, NRF_QUEUE_MODE_OVERFLOW);
NRF_QUEUE_DEF(ruuvi_standard_message_t, m_std_tx_queue, BLE_STD_QUEUE_SIZE, NRF_QUEUE_MODE_OVERFLOW);

/** Pointer to NUS **/
ble_nus_t* p_nus;

/** Asynchronous transfer.
 *  This is entry point for bulk transfer library, i.e. data and length can be any values
 *  Driver handles splitting data to chunks
 *
 *  @param endpoint destination endpoint of data transfer. Plese refer to Ruuvi interface specification (TODO), typically 0xE0 - 0xFF
 *  @param data byte array to be transferred. Must be dynamically allocated, will be freed once tx is complete on non-acknowledged tx, after acknowledge on acknowledged packets
 *  @param length number of of bytes to be transferred. Maximum 255*18 = 4590 bytes.
 *
 *  Returns TRANSFER_SUCCESS if message was placed to transfer queue, error code if queuing failed.
 **/
bulk_transfer_ret_t ble_bulk_transfer_asynchronous(const ruuvi_endpoint_t endpoint, uint8_t* data, const size_t length)
{
  if(!nrf_queue_available_get(&m_ble_tx_queue)) { return NRF_ERROR_NO_MEM; }
  if(BLE_BULK_TX_MAX_SIZE < length) { return TX_ERROR_MAX_SIZE_EXCEEDED; }
  
  uint8_t num_chunks = (length/BLE_CHUNK_SIZE);
  if(length%BLE_CHUNK_SIZE) {num_chunks++;}
  ble_bulk_tx_header_t header = {.endpoint = endpoint,
                                 .index = 255,
                                 .chunks = num_chunks,
                                 .CRC8 = 0};  //XXX
  uint8_t* index = calloc (1, sizeof(uint8_t));
  *index=255; //Header has index of 255, start TX with header
  ble_bulk_tx_t tx = {.data     = data,
                      .endpoint = endpoint,
                      .length   = length,
                      .chunk_index = index,
                      .header = header
                     };
  NRF_LOG_INFO("Preparing to send %d bytes in %d chunks\r\n", length, num_chunks);
  return nrf_queue_push(&m_ble_tx_queue, &tx);
}

ret_code_t ble_std_transfer_asynchronous(const ruuvi_standard_message_t message)
{
  NRF_LOG_INFO("STD message added to queue\r\n");
  return nrf_queue_push(&m_std_tx_queue, &message);
}

/** Process BLE message queue. This function should be scheduled in main loop and BLE TX READY event.**/
// TODO: Split to several functions
ret_code_t ble_message_queue_process(void)
{
  ret_code_t err_code = NRF_SUCCESS;
  //Send messages from std queue first
  while(!nrf_queue_is_empty(&m_std_tx_queue) &&
        NRF_SUCCESS == err_code)
  {
    ruuvi_standard_message_t txs[1];
    ruuvi_standard_message_t* tx =&(txs[0]);
    err_code = nrf_queue_peek (&m_std_tx_queue,
                               tx);
    err_code |= ble_transfer_raw((void*) tx, sizeof(ruuvi_standard_message_t));
    //Pop tx if transmission was placed in SD queue
    if(NRF_SUCCESS == err_code) {nrf_queue_pop (&m_std_tx_queue, tx); }
    NRF_LOG_INFO("Sent STD message\r\n");
  }

  //return success if bulk queue is empty and there was no error in std queue
  if(nrf_queue_is_empty(&m_ble_tx_queue) &&
     NRF_SUCCESS == err_code)
  {
    NRF_LOG_DEBUG("Queue is empty\r\n");
    return NRF_SUCCESS;
  }
  //Allocate memory for the first element of the queue
  ble_bulk_tx_t txs[1];
  ble_bulk_tx_t* tx =  &(txs[0]);

  //Process while no errors occur
  while(NRF_SUCCESS == err_code)
  {
    //Read first element
    err_code = nrf_queue_peek (&m_ble_tx_queue,
                               tx);

    //No more elements could be read -> queue is processed -> return success
    if(NRF_SUCCESS != err_code) { return NRF_SUCCESS; }
    uint8_t* chunk_index = tx->chunk_index;
    NRF_LOG_DEBUG("Processing tx, next chunk is %d\r\n", *chunk_index);
    //Send header
    if(255==*chunk_index)
    {
      uint8_t data[BLE_BULK_HEADER_SIZE] = {0};
      data[0] = tx->header.endpoint;
      data[1] = tx->header.index;
      data[2] = tx->header.chunks;
      data[3] = tx->header.CRC8;            
      err_code = ble_transfer_raw(data, BLE_BULK_HEADER_SIZE);
      if(NRF_SUCCESS == err_code) {(*chunk_index)++;} //Roll around to element 0
    }
  
    //While this element has unsent data and data was queued successfully
    while((((*chunk_index)+1) * BLE_CHUNK_SIZE) <= tx->length && 
          NRF_SUCCESS == err_code)
    {
      //Create TX package
      uint8_t data[BLE_RAW_SIZE] = {0};
      data[0] = tx->endpoint;
      data[1] = *chunk_index;
      memcpy(&(data[2]), &(tx->data[*chunk_index * BLE_CHUNK_SIZE]), BLE_CHUNK_SIZE);
    
      //Send TX package
      err_code = ble_transfer_raw(data, BLE_RAW_SIZE);
      // Move to next chunk on succeess
      if(NRF_SUCCESS == err_code) {(*chunk_index)++;}
    }
    //Send last chunk if data was not divisible by CHUNK_SIZE
    uint8_t remainder_size = tx->length % BLE_CHUNK_SIZE;
    if(((*chunk_index) * BLE_CHUNK_SIZE) <= tx->length && 
       NRF_SUCCESS == err_code && 
       (remainder_size))
    {
      //Create TX package, header has 2 bytes
      uint8_t data[2 + remainder_size];
      data[0] = tx->endpoint;
      data[1] = *chunk_index;
      NRF_LOG_DEBUG("Last chunk has %d payload bytes\r\n", remainder_size);
      memcpy(&(data[2]), &(tx->data[*chunk_index * BLE_CHUNK_SIZE]), remainder_size);
    
      //Send TX package
      err_code = ble_transfer_raw(data, 2+remainder_size);
      if(NRF_SUCCESS == err_code) {(*chunk_index)++;}
    }
    
    //This element has been processed, pop tx from queue
    if(NRF_SUCCESS == err_code &&
       *chunk_index == tx->header.chunks) 
    {
      //Clear TX out of memory and queue
      nrf_queue_pop (&m_ble_tx_queue, tx);
      ble_bulk_message_clean(tx);      
      NRF_LOG_INFO("Processed tx from queue.\r\n");
      break;
    }
  }
  if(NRF_SUCCESS != err_code){ NRF_LOG_INFO("BLE transfer status: %d\r\n", err_code); }
  return err_code;
}

/**
 *  Asynchronous transfer of raw binary data, max 20 bytes per chunk.
 *  This function is meant for driver's own use only. 
 */
ret_code_t ble_transfer_raw(uint8_t* data, size_t length)
{
  NRF_LOG_INFO("Transferring %d bytes\r\n", length);
  uint8_t data_array[BLE_RAW_SIZE] = {0};
  uint32_t       err_code;
  memcpy(&data_array, data, length);  
  err_code = ble_nus_string_send(p_nus, data_array, length);
  return err_code;
}

/** Set pointer to NUS service **/
void ble_bulk_set_nus(ble_nus_t* nus)
{
 p_nus = nus;
}

/** Free whole message queue **/
ret_code_t ble_bulk_message_queue_purge()
{
  ble_bulk_tx_t txs[1] = {};
  ble_bulk_tx_t* tx =  &(txs[0]);
  while(!nrf_queue_is_empty(&m_ble_tx_queue))
  {
      nrf_queue_pop (&m_ble_tx_queue, tx);
      ble_bulk_message_clean(tx);
  }
  return NRF_SUCCESS;
}

/** Free single tx element **/
ret_code_t ble_bulk_message_clean(ble_bulk_tx_t* element)
{
  //Free TX data
  free(element->data);
  free(element->chunk_index);
  return NRF_SUCCESS;
}

