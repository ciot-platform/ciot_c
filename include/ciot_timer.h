/**
 * @file ciot_timer.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-08
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef __CIOT_TIMER__H__
#define __CIOT_TIMER__H__

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include <stdbool.h>
#include "ciot_err.h"

typedef ciot_err_t (*ciot_timer_event_handler_t)(void *args);

typedef struct ciot_timer_cfg
{
    ciot_timer_event_handler_t event_handler;
    void *event_args;
} ciot_timer_cfg_t;

ciot_err_t ciot_timer_start(ciot_timer_cfg_t *cfg);
uint64_t ciot_timer_now(void);
uint64_t ciot_timer_millis(void);
ciot_err_t ciot_timer_init(uint64_t *timer, uint16_t interval);
bool ciot_timer_compare(uint64_t *timer, uint16_t interval);

#ifdef __cplusplus
}
#endif

#endif  //!__CIOT_TIMER__H__
