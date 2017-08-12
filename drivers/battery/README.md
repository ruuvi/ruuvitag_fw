# Battery voltage driver
Returns battery voltage when `uint16_t getBattery(void);` is called.

Note: The battery voltage reading is asynchronous, i.e. a command to 
sample is sent to ADC. Function returns _latest available result_
which might be one sample earlier. 
