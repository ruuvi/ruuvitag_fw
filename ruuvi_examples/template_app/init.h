/**
 *  Initialization functions for RuuviTag template application project.
 */

/**
 * Initialize logging
 *
 * This function initializes logging peripherals, and must be called before using NRF_LOG().
 * Exact functionality depends on defines at sdk_config.h.
 * 
 * 
 * 
 * @return 0          Operation successful
 * @retval 1          Something went wrong
 *
 */
uint8_t init_log(void);

