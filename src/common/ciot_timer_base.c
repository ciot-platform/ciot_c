/**
 * @file ciot_timer_base.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-12-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */
 
#include "ciot_config.h"

#if CIOT_CONFIG_FEATURE_TIMER == 1

#include "ciot_timer.h"

bool ciot_timer_compare(uint64_t *timer, uint16_t interval)
{
    if (ciot_timer_now() >= *timer)
    {
        *timer = ciot_timer_now() + interval;
        return true;
    }
    return false;
}

ciot_err_t ciot_timer_init(uint64_t *timer, uint16_t interval)
{
    CIOT_ERR_NULL_CHECK(timer);
    *timer = ciot_timer_now() + interval;
    return CIOT_ERR_OK;
}

#endif // CIOT_CONFIG_FEATURE_TIMER == 1