/*
 *
 *
 *
 */

#include <stdint.h>
#include <stdbool.h>

#include "app_util_platform.h"
#include "nrf_gpio.h"
#include "nrf_drv_spi.h"

#include "custom_board.h"

#ifndef SPIM0_SS_HUMI_PIN
  #error missing define SPIM0_SS_HUMI_PIN
#endif


static volatile bool spi_xfer_done;
static const nrf_drv_spi_t spi = NRF_DRV_SPI_INSTANCE(0);

static void spi_event_handler(nrf_drv_spi_evt_t const * p_event)
{
	spi_xfer_done = true;

}

void bme280_platform_init()
{
	nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG(0);

	spi_config.ss_pin = SPIM0_SS_HUMI_PIN;
        APP_ERROR_CHECK(nrf_drv_spi_init(&spi, &spi_config, spi_event_handler));

}


uint8_t bme280_read_reg(uint8_t reg)
{
	uint8_t buf[2];
	uint8_t rx[2];
	spi_xfer_done = false;

	buf[0] = reg | 0x80;
	buf[1] = 0x00;
	nrf_drv_spi_transfer(&spi, buf, 2, rx, 2);

	while (!spi_xfer_done) {
		__WFE();
	}

	return rx[1];
}


void bme280_write_reg(uint8_t reg, uint8_t value)
{
	uint8_t buf[2];
	spi_xfer_done = false;

	buf[0] = reg & 0x7F;
	buf[1] = value;
	nrf_drv_spi_transfer(&spi, buf, 2, NULL, 0);

	while (!spi_xfer_done) {
		__WFE();
	}
}
