/**
@addtogroup LIS2DH12Driver LIS2DH12 Acceleration Sensor Driver
@{
@file       LIS2DH12.c

Implementation of the LIS2DH12 driver.

For a detailed description see the detailed description in @ref LIS2DH12.h

* @}
***************************************************************************************************/

/* INCLUDES ***************************************************************************************/
#include "lis2dh12.h"
#include "lis2dh12_registers.h"

#include <string.h>
#include <stdlib.h>

#include "spi.h"
#include "nrf_drv_gpiote.h"
#include "nrf_delay.h"
#include "nrf.h"
#include "app_timer.h"
#include "bsp.h"
#include "boards.h"
#include "init.h" //Timer ticks - todo: refactor

#define NRF_LOG_MODULE_NAME "LIS2DH12"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"


/* CONSTANTS **************************************************************************************/
/** Maximum Size of SPI Addresses */
#define ADR_MAX 0x3FU

/** Size of raw sensor data for all 3 axis */
#define SENSOR_DATA_SIZE 6U

/** Bit Mask to set read bit for SPI Transfer */
#define SPI_READ 0x80U

/** Bit Mask to enable auto address incrementation for multi read */
#define SPI_ADR_INC 0x40U

/* MACROS *****************************************************************************************/
//TODO: Refactor to some place else
APP_TIMER_DEF(lis2dh12_timer_id);                           /** Creates timer id for our program **/



/* PROTOTYPES *************************************************************************************/
static lis2dh12_ret_t selftest(void);
void timer_lis2dh12_event_handler(void* p_context);

/* VARIABLES **************************************************************************************/
static lis2dh12_scale_t      state_scale = LIS2DH12_SCALE16G;
static lis2dh12_resolution_t state_resolution = LIS2DH12_RES10BIT;
static const uint8_t lis2dh12_mgpb_map[] = {1,2,4,12};

/**
 *  Initializes LIS2DH12, and puts it in sleep mode.
 *  
 */
lis2dh12_ret_t lis2dh12_init(void)
{
    lis2dh12_ret_t err_code = LIS2DH12_RET_OK;

    /* Initialize SPI */
    if (false == spi_isInitialized()){ spi_init(); }
    
    // Initialize the lis2dh12 timer module.
    // Requires the low-frequency clock initialized
    // Create timer TODO refactor to some place else.
    /*err_code |= app_timer_create(&lis2dh12_timer_id,
                                  APP_TIMER_MODE_REPEATED,
                                  NULL);
    APP_ERROR_CHECK(err_code);*/
    
    /** Reboot memory  - causes FIFO to fail - maybe delay is needed  before enabling axes? **/
    //err_code |= lis2dh12_reset();
    
    /* Start Selftest */
    err_code |= selftest();

    return err_code;
}

/** Reboots memory to default settings **/
lis2dh12_ret_t lis2dh12_reset(void)
{
    lis2dh12_ret_t err_code = LIS2DH12_RET_OK;
    uint8_t ctrl[1] = {0};    
    ctrl[0] = LIS2DH12_BOOT_MASK;
    err_code |= lis2dh12_write_register(LIS2DH12_CTRL_REG5, ctrl, 1);
    return err_code;
}

/**
 *  Enables X-Y-Z axes
 */
lis2dh12_ret_t lis2dh12_enable(void)
{
    lis2dh12_ret_t err_code = LIS2DH12_RET_OK;
    /* Enable XYZ axes */
    uint8_t ctrl[1] = {0};
    ctrl[0] = LIS2DH12_XYZ_EN_MASK;
    err_code |= lis2dh12_write_register(LIS2DH12_CTRL_REG1, ctrl, 1);
    return err_code;
}

/**
 *  
 */
lis2dh12_ret_t lis2dh12_set_scale(lis2dh12_scale_t scale)
{
    lis2dh12_ret_t err_code = LIS2DH12_RET_OK;
    //Read current value of CTRL4 Register
    uint8_t ctrl4[1] = {0};
    err_code |= lis2dh12_read_register(LIS2DH12_CTRL_REG4, ctrl4, 1);
    //Reset scale bits
    ctrl4[0] &= ~LIS2DH12_FS_MASK;
    ctrl4[0] |= scale;
    //Write register value back to lis2dh12
    err_code |= lis2dh12_write_register(LIS2DH12_CTRL_REG4, ctrl4, 1);
    if(LIS2DH12_RET_OK == err_code){ state_scale = scale; }
    return err_code;
}

/**
 *  Sets resolution to lis2dh12in bits
 *  valid values are in enum lis2dh12_resolution_t
 *  Invalid resolution will put device to normal power (10 bit) mode.
 *
 *  returns error code, 0 on success.
 */
lis2dh12_ret_t lis2dh12_set_resolution(lis2dh12_resolution_t resolution)
{
    lis2dh12_ret_t err_code = LIS2DH12_RET_OK;
    uint8_t ctrl1[1] = {0};
    uint8_t ctrl4[1] = {0};
    //Read registers 3 & 4
    err_code |= lis2dh12_read_register(LIS2DH12_CTRL_REG1, ctrl1, 1);
    err_code |= lis2dh12_read_register(LIS2DH12_CTRL_REG4, ctrl4, 1);
    //Reset Low-power, high-resolution masks
    ctrl1[0] &= ~LIS2DH12_LPEN_MASK;
    ctrl4[0] &= ~LIS2DH12_HR_MASK;
    switch(resolution)
    {
        case LIS2DH12_RES12BIT:
             ctrl4[0] |= LIS2DH12_HR_MASK;
             break;

        //No action needed
        case LIS2DH12_RES10BIT:
             break;

        case LIS2DH12_RES8BIT:
             ctrl1[0] |= LIS2DH12_LPEN_MASK;
             break;
        //Writing normal power to lis2dh12 is safe
        default:
             err_code |= LIS2DH12_RET_INVALID;
             break;
    }
    err_code |= lis2dh12_write_register(LIS2DH12_CTRL_REG1, ctrl1, 1);
    err_code |= lis2dh12_write_register(LIS2DH12_CTRL_REG4, ctrl4, 1);
    if(LIS2DH12_RET_OK == err_code){ state_resolution = resolution; }    
    return err_code;
}

/**
 *  
 * Note: By design, when the device from high-resolution configuration (HR) is set to power-down 
 * mode (PD), it is recommended to read register REFERENCE (26h)
 * for a complete reset of the filtering block before switching to normal/high-performance mode again for proper 
 * device functionality.
 */
lis2dh12_ret_t lis2dh12_set_sample_rate(lis2dh12_sample_rate_t sample_rate)
{
    lis2dh12_ret_t err_code = LIS2DH12_RET_OK;
    uint8_t ctrl[1] = {0};
    err_code |= lis2dh12_read_register(LIS2DH12_CTRL_REG1, ctrl, 1);
    NRF_LOG_DEBUG("Read samplerate %x, status %d\r\n", ctrl[0], err_code);
    // Clear sample rate bits
    ctrl[0] &= ~LIS2DH12_ODR_MASK;
    // Setup sample rate
    ctrl[0] |= sample_rate;
    err_code |= lis2dh12_write_register(LIS2DH12_CTRL_REG1, ctrl, 1);
    NRF_LOG_DEBUG("Wrote samplerate %x, status %d\r\n", ctrl[0], err_code);

    //Always read REFERENCE register when powering down to reset filter.
    if(LIS2DH12_RATE_0 == sample_rate)
    {
        err_code |= lis2dh12_read_register(LIS2DH12_CTRL_REG6, ctrl, 1);
    }
    return err_code;
}

/**
 *  Select FIFO mode
 *
 *  Bypass:         FiFo not in use. Setting Bypass resets FiFo
 *  FIFO:           FiFo is in use. Accumulates data until full or reset
 *  Stream:         FiFo is in use. Accumulates data, discarding oldest sample on overflow
 *  Stream-to-FIFO: FiFo is in use. Starts in stream, switches to FIFO on interrupt. Remember to configure the interrupt source.
 *
 */
lis2dh12_ret_t lis2dh12_set_fifo_mode(lis2dh12_fifo_mode_t mode)
{
    lis2dh12_ret_t err_code = LIS2DH12_RET_OK;
    uint8_t ctrl_fifo[1] = {0};
    uint8_t ctrl5[1] = {0};

    err_code |= lis2dh12_read_register(LIS2DH12_CTRL_REG5, ctrl5, 1);
    err_code |= lis2dh12_read_register(LIS2DH12_FIFO_CTRL_REG, ctrl_fifo, 1);

    // Clear FiFo bits
    ctrl_fifo[0] &= ~LIS2DH12_FM_MASK;
    //Clear enable bit
    ctrl5[0] &= ~LIS2DH12_FIFO_EN_MASK;
    // Setup FiFo rate
    ctrl_fifo[0] |= mode;
    //Enable FiFo if appropriate
    if(LIS2DH12_MODE_BYPASS != mode){ ctrl5[0] |= LIS2DH12_FIFO_EN_MASK; }
    //FIFO must be enabled before setting mode
    err_code |= lis2dh12_write_register(LIS2DH12_CTRL_REG5, ctrl5, 1);
    err_code |= lis2dh12_write_register(LIS2DH12_FIFO_CTRL_REG, ctrl_fifo, 1);
    return err_code;
}

/** Return factor for current state **/
uint8_t get_mgpb()
{
    switch(state_scale)
    {
        case LIS2DH12_SCALE2G:  return lis2dh12_mgpb_map[0];
        case LIS2DH12_SCALE4G:  return lis2dh12_mgpb_map[1];
        case LIS2DH12_SCALE8G:  return lis2dh12_mgpb_map[2];
        case LIS2DH12_SCALE16G: return lis2dh12_mgpb_map[3];
        default:                return 0;
    }
}

lis2dh12_ret_t lis2dh12_read_samples(lis2dh12_sensor_buffer_t* buffer, size_t count)
{
     lis2dh12_ret_t err_code = LIS2DH12_RET_OK;
     size_t bytes_to_read = count*sizeof(lis2dh12_sensor_buffer_t);
     NRF_LOG_DEBUG("Reading %d bytes \r\n", bytes_to_read);
     err_code |= lis2dh12_read_register(LIS2DH12_OUT_X_L, (uint8_t*)buffer, count*sizeof(lis2dh12_sensor_buffer_t));
     uint8_t mgpb = get_mgpb();
     // Use constant bitshift, so we don't have to adjust mgpb with resolution
     uint8_t justify = 4; 
     for(int ii = 0; ii < count; ii++)
     {
        NRF_LOG_DEBUG("Before justification %d \r\n", buffer[ii].sensor.z);
        buffer[ii].sensor.x >>= justify;
        buffer[ii].sensor.y >>= justify;
        buffer[ii].sensor.z >>= justify;
        NRF_LOG_DEBUG("Before scaling %d \r\n", buffer[ii].sensor.z);
        buffer[ii].sensor.x *= mgpb;
        buffer[ii].sensor.y *= mgpb;
        buffer[ii].sensor.z *= mgpb;
     }
     return err_code;
}

// put number of samples in HW FIFO to count
lis2dh12_ret_t lis2dh12_get_fifo_sample_number(size_t* count)
{
    lis2dh12_ret_t err_code = LIS2DH12_RET_OK;    
    uint8_t ctrl[1] = {0};
    err_code |= lis2dh12_read_register(LIS2DH12_FIFO_SRC_REG, ctrl, 1);
    *count = ctrl[0] & LIS2DH12_FSS_MASK;
    return err_code;
}

// Generate watermark interrupt when FIFO reaches certain level
lis2dh12_ret_t lis2dh12_set_fifo_watermark(size_t count)
{
    if(count > 32) return LIS2DH12_RET_INVALID;
    lis2dh12_ret_t err_code = LIS2DH12_RET_OK;
    uint8_t ctrl[1] = {0};
    err_code |= lis2dh12_read_register(LIS2DH12_FIFO_CTRL_REG, ctrl, 1);
    ctrl[0] &= ~LIS2DH12_FTH_MASK;
    ctrl[0] += count;
    err_code |= lis2dh12_write_register(LIS2DH12_FIFO_CTRL_REG, ctrl, 1);
    return err_code;
}

/**
 *  Set interrupt on pin. Write "0" To disable interrupt on pin. 
 *
 *  @param interrupts interrupts, see registers.h
 *  @param pin 1 or, others are invalid
 */
lis2dh12_ret_t lis2dh12_set_interrupts(uint8_t interrupts, uint8_t pin)
{
  if(1 != pin && 2 != pin){ return LIS2DH12_RET_INVALID; }
  uint8_t ctrl[1]; 
  ctrl[0] = interrupts;
  uint8_t target_reg = LIS2DH12_CTRL_REG3;
  if( 2 == pin ) { target_reg = LIS2DH12_CTRL_REG6; }
  return lis2dh12_write_register(target_reg, ctrl, 1);
}

/* INTERNAL FUNCTIONS *****************************************************************************/

/**
 * Execute LIS2DH12 Selftest
 * TODO: Run the self-test internal to device
 *
 * @return LIS2DH12_RET_OK Selftest passed
 * @return LIS2DH12_RET_ERROR_SELFTEST Selftest failed
 */
static lis2dh12_ret_t selftest(void)
{
    uint8_t value[1] = {0};
    lis2dh12_read_register(LIS2DH12_WHO_AM_I, value, 1);
    if(LIS2DH12_I_AM_MASK != value[0]) { NRF_LOG_ERROR("WHO_AM_I: %x\r\n", value[0])}
    return (LIS2DH12_I_AM_MASK == value[0]) ? LIS2DH12_RET_OK : LIS2DH12_RET_ERROR;
}

/**
 * Read registers
 *
 * Read one or more registers from the sensor
 *
 * @param[in] address Start address to read from
 * @param[out] p_toRead Pointer to result buffer
 * @param[in] cound Number of bytes to read
 *
 * @return LIS2DH12_RET_OK No Error
 * @return LIS2DH12_RET_NULL Result buffer is NULL Pointer
 * @return LIS2DH12_RET_ERROR Read attempt was not successful
 */
lis2dh12_ret_t lis2dh12_read_register(const uint8_t address, uint8_t* const p_toRead, const size_t count)
{
    NRF_LOG_DEBUG("LIS2DH12 Register read started'\r\n");
    lis2dh12_ret_t err_code = LIS2DH12_RET_OK;
    uint8_t* write_buffer   = calloc(count+1, sizeof(uint8_t));
    //Separate buffer to read data, includes room for response to address byte. 
    //This is pretty ineficient, TODO
    uint8_t* read_buffer    = calloc(count+1, sizeof(uint8_t));

    if (NULL == p_toRead)
    {
        err_code |= LIS2DH12_RET_NULL;
    }
    else
    {
        write_buffer[0] = address | SPI_READ | SPI_ADR_INC;
        err_code |= spi_transfer_lis2dh12(write_buffer, (count + 1U), read_buffer);

        if (SPI_RET_OK == (SPI_Ret)err_code)
        {
            /* Transfer was ok, copy result */
            memcpy(p_toRead, &(read_buffer[1]), count);
        }
    }
    NRF_LOG_DEBUG("LIS2DH12 Register read complete'\r\n");
    free(read_buffer);
    free(write_buffer);
    return err_code;
}

/**
 * Write a register
 *
 * @param[in] address Register address to write, address is 5bit, so max value is 0x1F
 * @param[in] dataToWrite Data to write to register
 *
 * @return LIS2DH12_RET_OK No Error
 * @return LIS2DH12_RET_ERROR Address is lager than allowed
 */
lis2dh12_ret_t lis2dh12_write_register(uint8_t address, uint8_t* const dataToWrite, size_t count)
{
    lis2dh12_ret_t err_code = LIS2DH12_RET_OK;
    uint8_t* to_read  = calloc(count+1, sizeof(uint8_t)); /* dummy, not used for writing */
    uint8_t* to_write = calloc(count+1, sizeof(uint8_t));

    /* SPI Addresses are 5bit only */
    if (address <= ADR_MAX)
    {
        to_write[0] = address;
        memcpy(&(to_write[1]), dataToWrite, count);

        err_code |= spi_transfer_lis2dh12(to_write, (count+1), to_read);
    }
    free(to_read);
    free(to_write);

    return err_code;
}

