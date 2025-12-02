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

#if CIOT_CONFIG_FEATURE_TIMER == 1 && defined(CIOT_PLATFORM_ESP8266)

#include <time.h>

#include "esp_timer.h"
#include "ciot_timer.h"

uint64_t ciot_timer_now(void)
{
    return time(NULL);
}

uint64_t ciot_timer_millis(void)
{
    return (esp_timer_get_time() / 1000);
}

#endif  //!CIOT_CONFIG_FEATURE_TIMER == 1
