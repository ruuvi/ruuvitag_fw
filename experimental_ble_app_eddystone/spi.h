/******************************************************************************
spi.h
spi wrapper for Ruuvitag
Vesa Koskinen
May 11, 2016


******************************************************************************/

#include <stdint.h>

void 		spi_initialize(void);
void    	spi_transfer_bme280(uint8_t *to_write, uint8_t count, uint8_t *to_read);

