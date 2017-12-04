#ifndef PWM_H
#define PWM_H

#include <stdint.h>

/**
 *  Initialise PWM 
 */
void pwm_init(uint16_t frequency, uint8_t pin1, uint8_t pin2, uint8_t pin3, uint8_t pin4);

/*
 *  Enable PWM (consumes power)
 */
void pwm_enable();

/* 
 * Disable PWM
 */
void pwm_disable();

/**
 *  Set CH1 PWM cycle
 */
void pwm_set_1(uint8_t duty);

/**
 *  Set CH2 PWM cycle
 */
void pwm_set_2(uint8_t duty);


/**
 *  Set CH3 PWM cycle
 */
void pwm_set_3(uint8_t duty);


/**
 *  Set CH4 PWM cycle
 */
void pwm_set_4(uint8_t duty);

#endif
