/**
 * @file ciot_pwm.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "ciot_config.h"

#if CIOT_CONFIG_FEATURE_PWM == 1 && defined(CIOT_PLATFORM_ESP8266)

#include <stdlib.h>
#include "ciot_pwm.h"

// static const char *TAG = "ciot_pwm";

ciot_err_t ciot_pwm_init(uint8_t pin)
{
    return CIOT_ERR_NOT_SUPPORTED;
}

ciot_err_t ciot_pwm_set_duty(uint8_t pin, uint16_t duty)
{
    return CIOT_ERR_NOT_SUPPORTED;
}

ciot_err_t ciot_pwm_set_freq(uint32_t freq)
{
    return CIOT_ERR_NOT_SUPPORTED;
}

ciot_err_t ciot_pwm_set_resolution(uint8_t resolution)
{
    return CIOT_ERR_NOT_SUPPORTED;
}

#endif // CIOT_CONFIG_FEATURE_PWM == 1
