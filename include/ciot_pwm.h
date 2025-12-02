/**
 * @file ciot_pwm.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-04-20
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef __CIOT_PWM__H__
#define __CIOT_PWM__H__

#include "ciot_types.h"
#include "ciot_iface.h"
#include "ciot_err.h"

ciot_err_t ciot_pwm_init(uint8_t pin);
ciot_err_t ciot_pwm_set_duty(uint8_t pin, uint16_t duty);
ciot_err_t ciot_pwm_set_freq(uint32_t freq);
ciot_err_t ciot_pwm_set_resolution(uint8_t resolution);

#endif  //!__CIOT_PWM__H__