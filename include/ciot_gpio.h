/**
 * @file ciot_gpio.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-24
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef __CIOT_GPIO__H__
#define __CIOT_GPIO__H__

#ifdef __cplusplus
extern "C" {
#endif

#include "ciot_types.h"
#include "ciot_iface.h"

typedef struct ciot_gpio *ciot_gpio_t;

typedef int (ciot_hal_gpio_set_state_fn)(int num, int state);
typedef int (ciot_hal_gpio_get_state_fn)(int num);

typedef struct ciot_gpio_base
{
    ciot_iface_t iface;
    ciot_gpio_cfg_t cfg;
    ciot_gpio_status_t status;
    ciot_hal_gpio_set_state_fn *set_state;
    ciot_hal_gpio_get_state_fn *get_state;
    uint64_t timer;
    bool blinking;
    bool blink_signal;
} ciot_gpio_base_t;

ciot_gpio_t ciot_gpio_new(void *handle);
ciot_err_t ciot_gpio_init(ciot_gpio_t self);
ciot_err_t ciot_gpio_task(ciot_gpio_t self);
ciot_err_t ciot_gpio_start(ciot_gpio_t self, ciot_gpio_cfg_t *cfg);
ciot_err_t ciot_gpio_stop(ciot_gpio_t self);
ciot_err_t ciot_gpio_process_req(ciot_gpio_t self, ciot_gpio_req_t *req);
ciot_err_t ciot_gpio_set_cfg(ciot_gpio_t self, ciot_gpio_cfg_t *cfg);
ciot_err_t ciot_gpio_get_cfg(ciot_gpio_t self, ciot_gpio_cfg_t *cfg);
ciot_err_t ciot_gpio_get_status(ciot_gpio_t self, ciot_gpio_status_t *status);
ciot_err_t ciot_gpio_set_state(ciot_gpio_t self, uint16_t id, ciot_gpio_state_t state);
void ciot_gpio_write_pin(ciot_gpio_t self, uint16_t pin_num, int value);
int ciot_gpio_read_pin(ciot_gpio_t self, uint16_t pin_num);
ciot_gpio_state_t ciot_gpio_get_state(ciot_gpio_t self, uint16_t id);

#ifdef __cplusplus
}
#endif

#endif  //!__CIOT_GPIO__H__