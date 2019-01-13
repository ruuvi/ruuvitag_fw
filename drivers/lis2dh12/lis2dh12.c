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


/* PROTOTYPES *************************************************************************************/
static lis2dh12_ret_t selftest(void);
void timer_lis2dh12_event_handler(void* p_context);
static int16_t rawToMg(int16_t raw_acceleration);
static uint8_t scale_interrupt_threshold(int16_t threshold_mg);

/* VARIABLES **************************************************************************************/
static lis2dh12_scale_t      state_scale = LIS2DH12_SCALE16G;
static lis2dh12_resolution_t state_resolution = LIS2DH12_RES10BIT;



/**
 *  Initializes LIS2DH12, and puts it in sleep mode.
 *  
 */
lis2dh12_ret_t lis2dh12_init(void)
{
    lis2dh12_ret_t err_code = LIS2DH12_RET_OK;

    /* Initialize SPI */
    if (false == spi_isInitialized()){ spi_init(); }
    
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
  uint8_t ctrl[3] = {0x10, 0x00, 0x07}; //Default values, only first and third are non-zero
  err_code |= lis2dh12_write_register(0x1E, &ctrl[0], 1);
  err_code |= lis2dh12_write_register(0x1F, &ctrl[1], 1);
  err_code |= lis2dh12_write_register(0x20, &ctrl[2], 1);
  err_code |= lis2dh12_write_register(0x21, &ctrl[1], 1);
  err_code |= lis2dh12_write_register(0x22, &ctrl[1], 1);
  err_code |= lis2dh12_write_register(0x23, &ctrl[1], 1);
  err_code |= lis2dh12_write_register(0x24, &ctrl[1], 1);
  err_code |= lis2dh12_write_register(0x25, &ctrl[1], 1);
  err_code |= lis2dh12_write_register(0x26, &ctrl[1], 1);
  err_code |= lis2dh12_write_register(0x2E, &ctrl[1], 1);
  err_code |= lis2dh12_write_register(0x30, &ctrl[1], 1);
  err_code |= lis2dh12_write_register(0x32, &ctrl[1], 1);
  err_code |= lis2dh12_write_register(0x33, &ctrl[1], 1);
  err_code |= lis2dh12_write_register(0x34, &ctrl[1], 1);
  err_code |= lis2dh12_write_register(0x36, &ctrl[1], 1);
  err_code |= lis2dh12_write_register(0x37, &ctrl[1], 1);
  err_code |= lis2dh12_write_register(0x38, &ctrl[1], 1);
  err_code |= lis2dh12_write_register(0x3A, &ctrl[1], 1);
  err_code |= lis2dh12_write_register(0x3B, &ctrl[1], 1);
  err_code |= lis2dh12_write_register(0x3C, &ctrl[1], 1);
  err_code |= lis2dh12_write_register(0x3D, &ctrl[1], 1);
  err_code |= lis2dh12_write_register(0x3E, &ctrl[1], 1);
  err_code |= lis2dh12_write_register(0x3F, &ctrl[1], 1);
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

lis2dh12_ret_t lis2dh12_read_samples(lis2dh12_sensor_buffer_t* buffer, size_t count)
{
     lis2dh12_ret_t err_code = LIS2DH12_RET_OK;
     size_t bytes_to_read = count*sizeof(lis2dh12_sensor_buffer_t);
     NRF_LOG_DEBUG("Reading %d bytes \r\n", bytes_to_read);
     err_code |= lis2dh12_read_register(LIS2DH12_OUT_X_L, (uint8_t*)buffer, count*sizeof(lis2dh12_sensor_buffer_t));
     // Use constant bitshift, so we don't have to adjust mgpb with resolution
     for(int ii = 0; ii < count; ii++)
     {
        buffer[ii].sensor.x = rawToMg(buffer[ii].sensor.x);
        buffer[ii].sensor.y = rawToMg(buffer[ii].sensor.y);
        buffer[ii].sensor.z = rawToMg(buffer[ii].sensor.z);
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
 * Enable activity detection interrupt on pin 2. Interrupt is high for samples where high-passed acceleration exceeds mg
 */
lis2dh12_ret_t lis2dh12_set_activity_interrupt_pin_2(uint16_t mg)
{
    uint8_t cfg = 0;

    // // Configure activity interrupt - TODO: Implement in driver, add tests.
    // uint8_t ctrl[1];
    // // Enable high-pass for Interrupt function 2.
    // //CTRLREG2 = 0x02
    // ctrl[0] = LIS2DH12_HPIS2_MASK;
    // lis2dh12_write_register(LIS2DH12_CTRL_REG2, ctrl, 1);
    lis2dh12_read_register(LIS2DH12_CTRL_REG2, &cfg, 1);
    cfg |= LIS2DH12_HPIS2_MASK;
    lis2dh12_write_register(LIS2DH12_CTRL_REG2, &cfg, 1);

    // Enable interrupt 2 on X-Y-Z HI/LO.
    // INT2_CFG = 0x7F
    // ctrl[0] = 0x7F;
    // lis2dh12_write_register(LIS2DH12_INT2_CFG, ctrl, 1);
    cfg = 0;
    cfg |= LIS2DH12_6D_MASK | LIS2DH12_ZHIE_MASK | LIS2DH12_ZLIE_MASK;
    cfg |= LIS2DH12_YHIE_MASK | LIS2DH12_YLIE_MASK;
    cfg |= LIS2DH12_XHIE_MASK | LIS2DH12_XLIE_MASK;
    lis2dh12_set_interrupt_configuration(cfg, 2);
    // Interrupt on 64 mg+ (highpassed, +/-).
    //INT2_THS= 0x04 // 4 LSB = 64 mg @2G scale
    // ctrl[0] = LIS2DH12_ACTIVITY_THRESHOLD;
    // lis2dh12_write_register(LIS2DH12_INT2_THS, ctrl, 1);

    uint8_t threshold = scale_interrupt_threshold(mg);
    if(0 == threshold)
    {
      threshold = 1;
    }

    lis2dh12_set_threshold(threshold, 2);

    // Enable Interrupt function 2 on LIS interrupt pin 2 (stays high for 1/ODR).
    lis2dh12_set_interrupts(LIS2DH12_I2C_INT2_MASK, 2);

    return LIS2DH12_RET_OK;
}

/**
 *  Set interrupt on pin. Write "0" To disable interrupt on pin. 
 *  NOTE: pin 1 and pin 2 DO NOT support identical configurations.
 *
 *  @param interrupts interrupts, see registers.h
 *  @param function 1 or 2, others are invalid
 */
lis2dh12_ret_t lis2dh12_set_interrupts(uint8_t interrupts, uint8_t function)
{
  if(1 != function && 2 != function){ return LIS2DH12_RET_INVALID; }
  uint8_t ctrl[1]; 
  ctrl[0] = interrupts;
  uint8_t target_reg = LIS2DH12_CTRL_REG3;
  if( 2 == function ) { target_reg = LIS2DH12_CTRL_REG6; }
  return lis2dh12_write_register(target_reg, ctrl, 1);
}

/**
 * Setup interrupt configuration: AND/OR of events, X-Y-Z Hi/Lo, 6-direction detection
 *
 * @param cfg, configuration. See registers.h for description
 * @param function number of interrupt, 1 or 2. Others are invalid
 *
 * @return error code from SPI write or LIS2DH12_RET_INVALID if pin was invalid. 0 on success.
 */
lis2dh12_ret_t lis2dh12_set_interrupt_configuration(uint8_t cfg, uint8_t function)
{
  if(1 != function && 2 != function){ return LIS2DH12_RET_INVALID; }
  uint8_t ctrl[1]; 
  ctrl[0] = cfg;
  uint8_t target_reg = LIS2DH12_INT1_CFG;
  if( 2 == function ) { target_reg = LIS2DH12_INT2_CFG; }
  return lis2dh12_write_register(target_reg, ctrl, 1);
}

/**
 *  Setup number of LSBs needed to trigger activity interrupt. 
 *
 *  @param bits number of LSBs required to trigger the interrupt
 *  @param pin 1 or 2, others are invalid
 *
 *  @return error code from stack
 */
lis2dh12_ret_t lis2dh12_set_threshold(uint8_t bits, uint8_t pin)
{
  if((1 != pin && 2 != pin) || bits > 0x7F){ return LIS2DH12_RET_INVALID; }
  uint8_t ctrl[1];
  ctrl[0] = bits;
  uint8_t target_reg = LIS2DH12_INT1_THS;
  if(2 == pin) { target_reg = LIS2DH12_INT2_THS;} 
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
 * Conversion functions from 
 * https://github.com/STMicroelectronics/STMems_Standard_C_drivers/blob/3e3b7528dfacb223aea250daf4e512e335f17509/lis2dh12_STdC/driver/lis2dh12_reg.c#L104
 */
static inline int16_t lis2dh12_from_fs2_hr_to_mg(int16_t lsb)
{
  return (lsb / 16 ) * 1;
}

static inline int16_t lis2dh12_from_fs4_hr_to_mg(int16_t lsb)
{
  return (lsb / 16 ) * 2;
}

static inline int16_t lis2dh12_from_fs8_hr_to_mg(int16_t lsb)
{
  return (lsb / 16 ) * 4;
}

static inline int16_t lis2dh12_from_fs16_hr_to_mg(int16_t lsb)
{
  return (lsb / 16) * 12;
}

static inline int16_t lis2dh12_from_fs2_nm_to_mg(int16_t lsb)
{
  return (lsb / 64) * 4;
}

static inline int16_t lis2dh12_from_fs4_nm_to_mg(int16_t lsb)
{
  return (lsb / 64) * 8;
}

static inline int16_t lis2dh12_from_fs8_nm_to_mg(int16_t lsb)
{
  return (lsb / 64) * 16;
}

static inline int16_t lis2dh12_from_fs16_nm_to_mg(int16_t lsb)
{
  return (lsb / 64) * 48;
}

static inline int16_t lis2dh12_from_fs2_lp_to_mg(int16_t lsb)
{
  return (lsb / 256) * 16;
}

static inline int16_t lis2dh12_from_fs4_lp_to_mg(int16_t lsb)
{
  return (lsb / 256) * 32;
}

static inline int16_t lis2dh12_from_fs8_lp_to_mg(int16_t lsb)
{
  return (lsb / 256) * 64;
}

static inline int16_t lis2dh12_from_fs16_lp_to_mg(int16_t lsb)
{
  return (lsb / 256) * 192;
}


/**
 * Convert raw value to acceleration in mg. Reads scale and resolution from state variables.
 *
 * @param raw_acceleration raw ADC value from LIS2DH12
 * @return int16_t representing acceleration in milli-G. 
 */
static int16_t rawToMg(int16_t raw_acceleration)
{
  switch(state_scale)
  {
    case LIS2DH12_SCALE2G:
      switch(state_resolution)
      {
        case LIS2DH12_RES8BIT:
          return lis2dh12_from_fs2_lp_to_mg(raw_acceleration);

        case LIS2DH12_RES10BIT:
          return  lis2dh12_from_fs2_nm_to_mg(raw_acceleration);

        case LIS2DH12_RES12BIT:
          return  lis2dh12_from_fs2_hr_to_mg(raw_acceleration);

        default:
          break;
      }
      break;

    case LIS2DH12_SCALE4G:
      switch(state_resolution)
      {
        case LIS2DH12_RES8BIT:
          return  lis2dh12_from_fs4_lp_to_mg(raw_acceleration);

        case LIS2DH12_RES10BIT:
          return  lis2dh12_from_fs4_nm_to_mg(raw_acceleration);

        case LIS2DH12_RES12BIT:
          return  lis2dh12_from_fs4_hr_to_mg(raw_acceleration);
      }
      break;

    case LIS2DH12_SCALE8G:
      switch(state_resolution)
      {
        case LIS2DH12_RES8BIT:
          return  lis2dh12_from_fs8_lp_to_mg(raw_acceleration);

        case LIS2DH12_RES10BIT:
          return  lis2dh12_from_fs8_nm_to_mg(raw_acceleration);

        case LIS2DH12_RES12BIT:
          return  lis2dh12_from_fs8_hr_to_mg(raw_acceleration);

        default:
          break;
      }
    break;

    case LIS2DH12_SCALE16G:
      switch(state_resolution)
      {
        case LIS2DH12_RES8BIT:
          return  lis2dh12_from_fs16_lp_to_mg(raw_acceleration);

        case LIS2DH12_RES10BIT:
          return  lis2dh12_from_fs16_nm_to_mg(raw_acceleration);

        case LIS2DH12_RES12BIT:
          return  lis2dh12_from_fs16_hr_to_mg(raw_acceleration);

        default:
          break;
      }
    break;

    default:
      break;
  }
  // reached only in case of an error, return "smallest representable value"
  return 0x8000;
}

/**
 * Return correct threshold setting for activity interrupt at
 * given threshold. Scales threshold upwards to next value.
 *
 * @param threshold_mg desired threshold. Will be converted to positive value if negative value is given.
 * @return threshold to set or 0x7F (max) if given threshold is not possible.
 */
static uint8_t scale_interrupt_threshold(int16_t threshold_mg)
{
  // Adjust for scale
  // 1 LSb = 16 mg @ FS = 2 g
  // 1 LSb = 32 mg @ FS = 4 g
  // 1 LSb = 62 mg @ FS = 8 g
  // 1 LSb = 186 mg @ FS = 16 g
  uint8_t divisor;
  switch(state_scale)
  {
    case LIS2DH12_FS_2G:
      divisor = 16;
      break;

    case LIS2DH12_FS_4G:
      divisor = 32;
      break;

    case LIS2DH12_FS_8G:
      divisor = 62;
      break;

    case LIS2DH12_FS_16G:
      divisor = 186;
      break;

    default:
      divisor = 16;
      break;
  }

  if(threshold_mg < 0) { threshold_mg = 0 - threshold_mg; }
  uint8_t threshold = (threshold_mg/divisor) + 1;
  if(threshold > 0x7F) { threshold = 0x7F; }
  return threshold;
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

