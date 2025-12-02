/**
 * @file ciot_event.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-27
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <stdbool.h>

#include "ciot_event.h"

bool ciot_event_is_ack(ciot_event_type_t event_type)
{
    return (event_type == CIOT_EVENT_TYPE_STARTED ||
        event_type == CIOT_EVENT_TYPE_STOPPED ||
        event_type == CIOT_EVENT_TYPE_DONE ||
        event_type == CIOT_EVENT_TYPE_ERROR);
}