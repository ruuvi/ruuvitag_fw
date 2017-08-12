# Battery voltage driver
Returns battery voltage when `uint16_t getBattery(void);` is called.
Synchronous function, i.e. starts sample, waits until sample is ready and returns value
