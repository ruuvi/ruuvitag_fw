#include "pwm.h"
#include "app_pwm.h"

APP_PWM_INSTANCE(PWM1,3);                   // Create the instance "PWM1" using TIMER3. TODO - #define 
APP_PWM_INSTANCE(PWM2,4);                   // Create the instance "PWM2" using TIMER4.


/**
 *  Initialise PWM TODO: #define period US
 */
void pwm_init(uint16_t frequency, uint8_t pin1, uint8_t pin2, uint8_t pin3, uint8_t pin4)
{
  uint32_t period = 1000000 / frequency;
  app_pwm_config_t pwm1_cfg = APP_PWM_DEFAULT_CONFIG_2CH(period, pin1, pin2);
  app_pwm_init(&PWM1, &pwm1_cfg, NULL);
  app_pwm_config_t pwm2_cfg = APP_PWM_DEFAULT_CONFIG_2CH(period, pin3, pin4);
  app_pwm_init(&PWM2, &pwm2_cfg, NULL);  
}

/*
 *  Enable PWM (consumes power)
 */
void pwm_enable()
{
  app_pwm_enable(&PWM1);
}

/* 
 * Disable PWM
 */
void pwm_disable()
{
  app_pwm_disable(&PWM1);
}

/**
 *  Set CH1 PWM cycle
 */
void pwm_set_1(uint8_t duty)
{
  app_pwm_channel_duty_set(&PWM1, 0, duty);
}

/**
 *  Set CH2 PWM cycle
 */
void pwm_set_2(uint8_t duty)
{
  app_pwm_channel_duty_set(&PWM1, 1, duty);
}

/**
 *  Set CH3 PWM cycle
 */
void pwm_set_3(uint8_t duty)
{
  app_pwm_channel_duty_set(&PWM2, 0, duty);
}

/**
 *  Set CH4 PWM cycle
 */
void pwm_set_4(uint8_t duty)
{
  app_pwm_channel_duty_set(&PWM2, 1, duty);
}
