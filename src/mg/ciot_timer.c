/**
 * @file ciot_timer.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-08
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "ciot_config.h"

#if CIOT_CONFIG_FEATURE_TIMER == 1 && defined(CIOT_PLATFORM_MONGOOSE)

#include <time.h>
#include "mongoose.h"
#include "ciot_timer.h"

uint64_t ciot_timer_now(void)
{
    return time(NULL);
}

uint64_t ciot_timer_millis(void)
{
    return mg_millis();
}

#endif // CIOT_CONFIG_FEATURE_TIMER == 1
