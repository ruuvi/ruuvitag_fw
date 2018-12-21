/**
 * Shorthands for Nordic FDS.
 *
 * Author Otso Jousimaa <otso@ojousima.net>
 * License BSD-3
 */

#ifndef FLASH_H
#define FLASH_H

ret_code_t flash_init(void);
ret_code_t flash_gc_run(void);
ret_code_t flash_record_get(const uint32_t page_id, const uint32_t record_id, const size_t data_size, void* const data);
ret_code_t flash_record_set(const uint32_t page_id, const uint32_t record_id, const size_t data_size, const void* const data);
ret_code_t flash_free_size_get(size_t* size);



#endif