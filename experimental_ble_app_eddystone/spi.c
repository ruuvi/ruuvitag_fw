/******************************************************************************
spi.c
spi wrapper for Ruuvitag
Vesa Koskinen
May 11, 2016


******************************************************************************/
#include "nrf_drv_spi.h"
#include "app_error.h"
#include "app_util_platform.h"
#include "boards.h"
#include "SEGGER_RTT.h"


#define SPI_INSTANCE  0 /**< SPI instance index. */

static const nrf_drv_spi_t spi = NRF_DRV_SPI_INSTANCE(SPI_INSTANCE);  /**< SPI instance. */
static volatile bool spi_xfer_done;  /**< Flag used to indicate that SPI instance completed the transfer. */

static uint8_t       m_rx_buf[20];    /**< RX buffer. */

/**
 * @brief SPI user event handler.
 */
void spi_event_handler(nrf_drv_spi_evt_t const * p_event)
{
    spi_xfer_done = true;

    if (m_rx_buf[0] != 0)
    {
		//SEGGER_RTT_printf(0, " Received: %d\r\n",m_rx_buf[0]);
    }
}

/**
 * @brief Send and receive bytes for bme280
 * Todo: make callback for each device using spi
 *
 */
void spi_transfer_bme280(uint8_t *to_write, uint8_t count, uint8_t *to_read)
{	
	spi_xfer_done = false;
	
	nrf_gpio_pin_clear(SPIM0_SS_HUMI_PIN);
	APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, to_write, count, to_read, count));
	while (!spi_xfer_done)
	{
		__WFE();
	} 
	nrf_gpio_pin_set(SPIM0_SS_HUMI_PIN); 
}

/**
 * @brief Initialize spi driver
 */
void spi_initialize(void)
{
	nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG(SPI_INSTANCE);
	spi_config.sck_pin = SPIM0_SCK_PIN;
	spi_config.miso_pin = SPIM0_MISO_PIN;
	spi_config.mosi_pin = SPIM0_MOSI_PIN;
	spi_config.frequency = NRF_DRV_SPI_FREQ_4M;
	
	nrf_gpio_pin_dir_set(SPIM0_SS_HUMI_PIN, NRF_GPIO_PIN_DIR_OUTPUT);
	nrf_gpio_cfg_output(SPIM0_SS_HUMI_PIN); 
	nrf_gpio_pin_set(SPIM0_SS_HUMI_PIN); 
	
	nrf_gpio_pin_dir_set(SPIM0_SS_ACC_PIN, NRF_GPIO_PIN_DIR_OUTPUT);
	nrf_gpio_cfg_output(SPIM0_SS_ACC_PIN); 
	nrf_gpio_pin_set(SPIM0_SS_ACC_PIN); 
	
	APP_ERROR_CHECK(nrf_drv_spi_init(&spi, &spi_config, spi_event_handler));
}
