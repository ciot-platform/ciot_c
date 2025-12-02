/**
 * @file ciot_uart_base.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-12-11
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "ciot_config.h"

#if CIOT_CONFIG_FEATURE_UART == 1

#include "ciot_uart.h"
#include "ciot_types.h"

static ciot_err_t ciot_uart_process_data(ciot_iface_t *iface, ciot_msg_data_t *data);
static ciot_err_t ciot_uart_get_data(ciot_iface_t *iface, ciot_msg_data_t *data);
static ciot_err_t ciot_uart_send_data(ciot_iface_t *iface, uint8_t *data, int size);
static ciot_err_t ciot_uart_read_data(ciot_iface_t *iface, uint8_t *data, int size);

ciot_err_t ciot_uart_init(ciot_uart_t self)
{
    ciot_uart_base_t *base = (ciot_uart_base_t *)self;

    base->iface.ptr = self;
    base->iface.process_data = ciot_uart_process_data;
    base->iface.get_data = ciot_uart_get_data;
    base->iface.send_data = ciot_uart_send_data;
    base->iface.read_data = ciot_uart_read_data;
    base->iface.info.type = CIOT_IFACE_TYPE_UART;
    
    return CIOT_ERR_OK;
}

ciot_err_t ciot_uart_process_req(ciot_uart_t self, ciot_uart_req_t *req)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(req);
    return CIOT_ERR_NOT_SUPPORTED;    
}

static ciot_err_t ciot_uart_process_data(ciot_iface_t *iface, ciot_msg_data_t *data)
{
    CIOT_ERR_TYPE_CHECK(data->which_type, CIOT_MSG_DATA_UART_TAG);

    ciot_uart_t self = iface->ptr;
    ciot_uart_data_t *uart = &data->uart;

    switch (uart->which_type)
    {
    case CIOT_UART_DATA_STOP_TAG:
        return ciot_uart_stop(self);
    case CIOT_UART_DATA_CONFIG_TAG:
        return ciot_uart_start(self, &uart->config);
    case CIOT_UART_DATA_REQUEST_TAG:
        return ciot_uart_process_req(self, &uart->request);
    default:
        return CIOT_ERR_INVALID_TYPE;
    }

    return CIOT_ERR_NOT_IMPLEMENTED;
}

static ciot_err_t ciot_uart_get_data(ciot_iface_t *iface, ciot_msg_data_t *data)
{
    CIOT_ERR_TYPE_CHECK(data->which_type, CIOT_MSG_DATA_GET_DATA_TAG);

    ciot_uart_base_t *self = iface->ptr;
    ciot_data_type_t data_type = data->get_data.type;
    data->which_type = CIOT_MSG_DATA_UART_TAG;

    switch (data_type)
    {
    case CIOT_DATA_TYPE_CONFIG:
        data->uart.which_type = CIOT_UART_DATA_CONFIG_TAG;
        data->uart.config = self->cfg;
        break;
    case CIOT_DATA_TYPE_STATUS:
        data->uart.which_type = CIOT_UART_DATA_STATUS_TAG;
        data->uart.status = self->status;
        break;
    default:
        return CIOT_ERR_NOT_FOUND;
    }

    return CIOT_ERR_OK;
}

static ciot_err_t ciot_uart_send_data(ciot_iface_t *iface, uint8_t *data, int size)
{
    CIOT_ERR_NULL_CHECK(iface);
    CIOT_ERR_NULL_CHECK(data);
    ciot_uart_t self = (ciot_uart_t)iface;
    return ciot_uart_send_bytes(self, data, size);
}

static ciot_err_t ciot_uart_read_data(ciot_iface_t *iface, uint8_t *data, int size)
{
    CIOT_ERR_NULL_CHECK(iface);
    CIOT_ERR_NULL_CHECK(data);
    ciot_uart_t self = (ciot_uart_t)iface;
    return ciot_uart_read_bytes(self, data, size);
}

ciot_err_t ciot_uart_get_cfg(ciot_uart_t self, ciot_uart_cfg_t *cfg)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(cfg);
    ciot_uart_base_t *base = (ciot_uart_base_t*)self;
    *cfg = base->cfg;
    return CIOT_ERR_OK;
}

ciot_err_t ciot_uart_set_gpio(ciot_uart_t self, ciot_uart_gpio_cfg_t *gpio)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(gpio);
    ciot_uart_base_t *base = (ciot_uart_base_t*)self;
    base->cfg.has_gpio = true;
    base->cfg.gpio = *gpio;
    return CIOT_ERR_OK;
}

ciot_err_t ciot_uart_get_status(ciot_uart_t self, ciot_uart_status_t *status)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(status);
    ciot_uart_base_t *base = (ciot_uart_base_t*)self;
    *status = base->status;
    return CIOT_ERR_OK;
}

#endif // CIOT_CONFIG_FEATURE_UART == 1