# Lis2DH12 driver

# Initialization
 * Call `lis2dh12_init();` to make sure SPI is initialized and communicatates properly with LIS2DH12.
 * Call `lis2dh12_reset()` to reset the memory contents
 * wait for 10 ms, i.e. `nrf_delay_ms(10);`
 * Enable X-Y-Z axes by `lis2dh12_enable();`
 * Initialize pin interrupts if your program is interrupt-driven, see nrf_nordic_pininterrupt for details.
 * Do not read sensor in interrupt context, you should rather schedule sensor read and let app process the request

# Configuration
 * Set sample rate, resolution and scale as required by application.
 * If you're using FIFO, remember to set watermark level and enable related interrupt. Note: FIFO has depth of 10 bits

# Using
 * Call `lis2dh12_get_fifo_sample_number(size_t* count);` to determine how many samples should be read from FIFO
 * Call `lis2dh12_read_samples(lis2dh12_sensor_buffer_t* buffer, size_t count)` to read _count_ samples into _buffer_.
 * Access samples by buffer[index].x etc. Samples are int16, in mg.
 
