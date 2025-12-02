/**
 * @file ciot_uart.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-12-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef __CIOT_UART__H__
#define __CIOT_UART__H__

#ifdef __cplusplus
extern "C" {
#endif

#include "ciot_types.h"
#include "ciot_iface.h"

typedef struct ciot_uart *ciot_uart_t;

typedef struct ciot_uart_base
{
    ciot_iface_t iface;
    ciot_uart_cfg_t cfg;
    ciot_uart_status_t status;
    // ciot_uart_info_t info;
} ciot_uart_base_t;

ciot_uart_t ciot_uart_new(void *handle);
ciot_err_t ciot_uart_init(ciot_uart_t self);
ciot_err_t ciot_uart_start(ciot_uart_t self, ciot_uart_cfg_t *cfg);
ciot_err_t ciot_uart_stop(ciot_uart_t self);
ciot_err_t ciot_uart_task(ciot_uart_t self);
ciot_err_t ciot_uart_process_req(ciot_uart_t self, ciot_uart_req_t *req);
ciot_err_t ciot_uart_set_gpio(ciot_uart_t self, ciot_uart_gpio_cfg_t *gpio);
ciot_err_t ciot_uart_get_cfg(ciot_uart_t self, ciot_uart_cfg_t *cfg);
ciot_err_t ciot_uart_get_status(ciot_uart_t self, ciot_uart_status_t *status);
ciot_err_t ciot_uart_send_bytes(ciot_uart_t self, uint8_t *data, int size);
ciot_err_t ciot_uart_read_bytes(ciot_uart_t self, uint8_t *data, int size);

#ifdef __cplusplus
}
#endif

#endif  //!__CIOT_UART__H__
