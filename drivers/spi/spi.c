/**
@addtogroup SPIWrapper SPI Wrapper for Ruuvitag
@{
@file       spi.c

Implementation of the SPI Wrapper.

Vesa Koskinen
May 11, 2016

For a detailed description see the detailed description in @ref spi.h

* @}
***************************************************************************************************/




/* INCLUDES ***************************************************************************************/
#include "spi.h"
#include "nrf_drv_spi.h"
#include "nrf_delay.h"
#include "app_util_platform.h"
#include "boards.h"

#define NRF_LOG_MODULE_NAME "SPI"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

/* CONSTANTS **************************************************************************************/
#define SPI_INSTANCE  0 /**< SPI instance index. */

/* MACROS *****************************************************************************************/

/* TYPES ******************************************************************************************/

/* PROTOTYPES *************************************************************************************/

void spi_event_handler(nrf_drv_spi_evt_t const * p_event);

/* VARIABLES **************************************************************************************/
static const nrf_drv_spi_t spi = NRF_DRV_SPI_INSTANCE(SPI_INSTANCE);  /**< SPI instance. */
volatile bool spi_xfer_done; /**< Semaphore to indicate that SPI instance completed the transfer. */
static bool initDone = false;       /**< Flag to indicate if this module is already initilized */

/* EXTERNAL FUNCTIONS *****************************************************************************/

extern void spi_init(void)
{
    /* Conigure SPI Interface */
    nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;
    spi_config.sck_pin = SPIM0_SCK_PIN;
    spi_config.miso_pin = SPIM0_MISO_PIN;
    spi_config.mosi_pin = SPIM0_MOSI_PIN;
    spi_config.frequency = NRF_DRV_SPI_FREQ_8M;

    /* Init chipselect for BME280 */
    nrf_gpio_pin_dir_set(SPIM0_SS_HUMI_PIN, NRF_GPIO_PIN_DIR_OUTPUT);
    nrf_gpio_cfg_output(SPIM0_SS_HUMI_PIN);
    nrf_gpio_pin_set(SPIM0_SS_HUMI_PIN);

    /* Init chipselect for LIS2DH12 */
    nrf_gpio_pin_dir_set(SPIM0_SS_ACC_PIN, NRF_GPIO_PIN_DIR_OUTPUT);
    nrf_gpio_cfg_output(SPIM0_SS_ACC_PIN);
    nrf_gpio_pin_set(SPIM0_SS_ACC_PIN);

    APP_ERROR_CHECK(nrf_drv_spi_init(&spi, &spi_config, spi_event_handler));

    spi_xfer_done = true;
    initDone = true;
}


extern bool spi_isInitialized(void)
{
    return initDone;
}

extern SPI_Ret spi_transfer_bme280(uint8_t* const p_toWrite, uint8_t count, uint8_t* const p_toRead)
{	

  NRF_LOG_DEBUG("Transferring to BME\r\n");

	SPI_Ret retVal = SPI_RET_OK;

	if ((NULL == p_toWrite) || (NULL == p_toRead))
	{
	    retVal = SPI_RET_ERROR;
	}

	
  /* check if an other SPI transfer is running */
  if ((true == spi_xfer_done) && (SPI_RET_OK == retVal))
	{
        spi_xfer_done = false;

        nrf_gpio_pin_clear(SPIM0_SS_HUMI_PIN);
        APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, p_toWrite, count, p_toRead, count));
        //Locks if run in interrupt context
        while (!spi_xfer_done)
        {
            //Requires initialized softdevice
            uint32_t err_code = sd_app_evt_wait();
            NRF_LOG_DEBUG("SPI status %d\r\n", err_code);
        }
        nrf_gpio_pin_set(SPIM0_SS_HUMI_PIN);
        retVal = SPI_RET_OK;
	}
	else
	{
	    retVal = SPI_RET_BUSY;
	}

  return retVal;
}

extern SPI_Ret spi_transfer_lis2dh12(uint8_t* const p_toWrite, uint8_t count, uint8_t* const p_toRead)
{
    SPI_Ret retVal = SPI_RET_OK;
    if ((NULL == p_toWrite) || (NULL == p_toRead))
    {
        retVal = SPI_RET_ERROR;
    }

    /* check if an other SPI transfer is running */
    if ((true == spi_xfer_done) && (SPI_RET_OK == retVal))
    {
        spi_xfer_done = false;

        nrf_gpio_pin_clear(SPIM0_SS_ACC_PIN);
        nrf_drv_spi_transfer(&spi, p_toWrite, count, p_toRead, count);
        while (!spi_xfer_done)
        {
            //Requires initialized softdevice - TODO
            sd_app_evt_wait();
            //__WFE(); 
        }
        nrf_gpio_pin_set(SPIM0_SS_ACC_PIN);
        retVal = SPI_RET_OK;
    }
    else
    {
        retVal = SPI_RET_BUSY;
    }

    return retVal;
}


/* INTERNAL FUNCTIONS *****************************************************************************/


/**
 * SPI user event handler
 *
 * Callback for Softdevice SPI Driver. Release blocking semaphore for SPI Transfer
 */
void spi_event_handler(nrf_drv_spi_evt_t const * p_event)
{
    spi_xfer_done = true;
    NRF_LOG_DEBUG("SPI Xfer done\r\n");
}
