/**
 * Copyright (c) 2017 - 2018, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "sdk_common.h"
#if NRF_MODULE_ENABLED(FDS)
#include "fds.h"
#include "fds_internal_defs.h"

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "fstorage.h"
#include "nrf_error.h"

#if defined(FDS_CRC_ENABLED)
    #include "crc16.h"
#endif

#include <string.h>

#define NRF_LOG_MODULE_NAME "FLASH"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

/* Flag to check fds initialization. */
static bool volatile m_fds_initialized;

/* Flag to check fds processing status. */
static bool volatile m_fds_processing;

static void fds_evt_handler(fds_evt_t const * p_evt)
{
    switch (p_evt->id)
    {
        case FDS_EVT_INIT:
            if (p_evt->result == FDS_SUCCESS)
            {
                m_fds_initialized = true;
                NRF_LOG_INFO("FDS init\r\n");
            }
            break;

        case FDS_EVT_WRITE:
        {
            if (p_evt->result == FDS_SUCCESS)
            {
                NRF_LOG_INFO("Record written\r\n");
                m_fds_processing = false;
            }
        } break;

        case FDS_EVT_UPDATE:
        {
            if (p_evt->result == FDS_SUCCESS)
            {
                NRF_LOG_INFO("Record updated\r\n");
                m_fds_processing = false;
            }
        } break;

        case FDS_EVT_DEL_RECORD:
        {
          if (p_evt->result == FDS_SUCCESS)
          {
            NRF_LOG_INFO("Record deleted\r\n");
            m_fds_processing = false;
          }
        } break;

        case FDS_EVT_DEL_FILE:
        {
          if (p_evt->result == FDS_SUCCESS)
          {
            NRF_LOG_INFO("File deleted\r\n");
            m_fds_processing = false;
          }
        } break;


        case FDS_EVT_GC:
        {
          if (p_evt->result == FDS_SUCCESS)
          {
            NRF_LOG_INFO("Garbage collected\r\n");
            m_fds_processing = false;
          }
        } break;

        default:
            break;
    }
}


/**
 *  Return largest number of bytes available for a record.
 */
ret_code_t flash_free_size_get(size_t* size)
{
  if(NULL == size) { return NRF_ERROR_NULL; }
  if(false == m_fds_initialized) { return NRF_ERROR_INVALID_STATE; }
  // Read filesystem status
  fds_stat_t stat = {0};
  ret_code_t rc = fds_stat(&stat);
  *size = stat.largest_contig * 4;
  return rc;
}

/**
 * Set data to record in page. Writes a new record if given record ID does not exist in page.
 * Updates record if it already exists.
 * Automatically runs garbage collection if record cannot fit on page. 
 *
 * parameter page_id: ID of a page. Can be random number.
 * parameter record_id: ID of a record. Can be a random number.
 * parameter data_size: size data to store
 * parameter data: pointer to data to store.
 * return: NRF__SUCCESS on success
 * return: NRF_ERROR_NULL if data is null
 * return: NRF_ERROR_INVALID_STATE if flash storage is not initialized
 * return: error code from stack on other error
 */
 ret_code_t flash_record_set(const uint32_t page_id, const uint32_t record_id, const size_t data_size, const void* const data)
 {
  if(NULL == data) { return NRF_ERROR_NULL; }
  if(false == m_fds_initialized) { return NRF_ERROR_INVALID_STATE; }
  
  ret_code_t err_code = NRF_SUCCESS;
  fds_record_desc_t desc = {0};
  fds_find_token_t  tok  = {0};
  /* A record structure. */
  fds_record_chunk_t const chunk =
  {
    .p_data = data,
    .length_words = (data_size + 3) / sizeof(uint32_t)
  };

  fds_record_t const record =
  {
    .file_id           = page_id,
    .key               = record_id,
    .data.p_chunks     = &chunk,
    .data.num_chunks   = 1
  };

  err_code |= fds_record_find(page_id, record_id, &desc, &tok);
  // If record was found
  if(FDS_SUCCESS == err_code)
  {

    /* Start write */
    m_fds_processing = true;
    
    err_code = fds_record_update(&desc, &record);
    if(FDS_SUCCESS != err_code) 
    { 
      m_fds_processing = false;
      return err_code; 
    }

    /* Wait for process to complete */
    while (m_fds_processing);

  }
  // If record was not found
  else
  {
    /* Start write */
    m_fds_processing = true;
    desc.record_id = record_id;
    err_code = fds_record_write(&desc, &record);
    if(FDS_SUCCESS != err_code) 
    { 
      m_fds_processing = false;
      return err_code; 
    }
    /* Wait for process to complete */
    while (m_fds_processing);

  }
  return err_code; 
 }

/**
 * Get data from record in page
 *
 * parameter page_id: ID of a page. Can be random number.
 * parameter record_id: ID of a record. Can be a random number.
 * parameter data_size: size data to store
 * parameter data: pointer to data to store.
 * return: NRF__SUCCESS on success
 * return: NRF_ERROR_NULL if data is null
 * return: NRF_ERROR_INVALID_STATE if flash storage is not initialized
 * return: error code from stack on other error
 */
ret_code_t flash_record_get(const uint32_t page_id, const uint32_t record_id, const size_t data_size, void* const data)
{ 
  if(NULL == data) { return NRF_ERROR_NULL; }
  if(false == m_fds_initialized) { return NRF_ERROR_INVALID_STATE; }
  
  fds_record_desc_t desc = {0};
  fds_find_token_t  tok  = {0};
  ret_code_t err_code = fds_record_find(page_id, record_id, &desc, &tok);

  // If file was found
  if(FDS_SUCCESS == err_code)
  {
    fds_flash_record_t record = {0};

    /* Open the record and read its contents. */
    err_code |= fds_record_open(&desc, &record);
    
    // Check length
    if(record.p_header->tl.length_words*4 > data_size) { return NRF_ERROR_DATA_SIZE; }

    /* Copy the data from flash into RAM. */
    memcpy(data, record.p_data, record.p_header->tl.length_words*4);

    /* Close the record when done reading. */
    err_code |= fds_record_close(&desc);
  }
  return err_code;
}

/**
 * Run garbage collection.
 *
 * return: NRF_ERROR_SUCCESS on success
 * return: NRF_ERROR_INVALID_STATE if flash is not initialized
 * return: error code from stack on other error
 */
 ret_code_t flash_gc_run(void)
 {
   if(false == m_fds_initialized) { return NRF_ERROR_INVALID_STATE; }
   return fds_gc();
 }

/**
 * Initialize flash
 *
 * return: NRF_SUCCESS on success
 * return: error code from stack on other error
 */
ret_code_t flash_init(void)
{
  ret_code_t rc = NRF_SUCCESS;
  /* Register first to receive an event when initialization is complete. */
  (void) fds_register(fds_evt_handler);
  rc |= fds_init();
  // Wait for init ok
  while(!m_fds_initialized);

  // Read filesystem status
  fds_stat_t stat = {0};
  rc |= fds_stat(&stat);
  return rc;
}
#endif