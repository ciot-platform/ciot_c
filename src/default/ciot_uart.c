/**
 * @file ciot_uart.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "ciot_config.h"

#if CIOT_CONFIG_FEATURE_UART == 1

#include <stdlib.h>
#include "ciot_uart.h"
#include "ciot_err.h"

// static const char *TAG = "ciot_uart";

struct ciot_uart
{
    ciot_uart_base_t base;
};

ciot_uart_t ciot_uart_new(void *handle)
{
    ciot_uart_t self = calloc(1, sizeof(struct ciot_uart));
    ciot_uart_init(self);
    return self;
}

ciot_err_t ciot_uart_start(ciot_uart_t self, ciot_uart_cfg_t *cfg)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(cfg);
    return CIOT_ERR_NOT_IMPLEMENTED;
}

ciot_err_t ciot_uart_stop(ciot_uart_t self)
{
    CIOT_ERR_NULL_CHECK(self);
    return CIOT_ERR_NOT_IMPLEMENTED;
}

ciot_err_t ciot_uart_task(ciot_uart_t self)
{
    CIOT_ERR_NULL_CHECK(self);
    return CIOT_ERR_NOT_IMPLEMENTED;
}

ciot_err_t ciot_uart_send_bytes(ciot_uart_t self, uint8_t *bytes, int size)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(bytes);
    return CIOT_ERR_NOT_IMPLEMENTED;
}

ciot_err_t ciot_uart_read_bytes(ciot_uart_t self, uint8_t *bytes, int size)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(bytes);
    return CIOT_ERR_NOT_IMPLEMENTED;
}

#endif // CIOT_CONFIG_FEATURE_UART == 1
