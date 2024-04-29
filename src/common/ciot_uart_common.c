/**
 * @file ciot_uart.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-11-14
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "ciot_uart.h"

#if (CIOT_CONFIG_FEATURE_UART)

#include <string.h>

#if CIOT_CONFIG_FEATURE_TIMER
static const char *TAG = "ciot_uart_common";
#endif

ciot_err_t ciot_uart_process_req(ciot_uart_t self, ciot_uart_req_t *req)
{
    CIOT_NULL_CHECK(self);
    CIOT_NULL_CHECK(req);
    switch (req->type)
    {
    case CIOT_UART_REQ_UNKNOWN:
        return CIOT_ERR_INVALID_TYPE;
    case CIOT_UART_REQ_SEND_DATA:
        return ciot_uart_send_data(self, req->data.send_data.data, req->data.send_data.size);
    case CIOT_UART_REQ_SEND_BYTES:
        return ciot_uart_send_bytes((ciot_iface_t *)self, req->data.send_data.data, req->data.send_data.size);
    case CIOT_UART_REQ_ENABLE_BRIDGE_MODE:
        return ciot_uart_set_bridge_mode(self, true);
    }
    return CIOT_ERR_INVALID_TYPE;
}

ciot_err_t ciot_uart_on_message(ciot_iface_t *iface, uint8_t *data, int size)
{
    CIOT_NULL_CHECK(iface);
    CIOT_NULL_CHECK(data);
    CIOT_NULL_CHECK(iface->event_handler);
    ciot_uart_base_t *self = (ciot_uart_base_t *)iface;
    ciot_iface_event_t ciot_evt = {0};

    ciot_evt.type = self->cfg.bridge_mode ? CIOT_IFACE_EVENT_DATA : CIOT_IFACE_EVENT_REQUEST;
    ciot_evt.data = (ciot_iface_event_data_u *)data;
    ciot_evt.size = size;
    return iface->event_handler(iface, &ciot_evt, iface->event_args);
}

ciot_err_t ciot_uart_task_internal(ciot_iface_t *iface, ciot_s_t ciot_s)
{
    ciot_err_t err = CIOT_OK;
#if CIOT_CONFIG_FEATURE_TIMER
    err = ciot_s_check_timeout(ciot_s);
    if (err != CIOT_OK)
    {
        CIOT_LOGE(TAG, "[%d]:%s %s[%d]", iface->info.id, ciot_iface_to_str(iface), ciot_err_to_message(err), err);
    }
#endif
    return err;
}

ciot_err_t ciot_uart_event(ciot_uart_base_t *base, ciot_iface_event_type_t event)
{
    if(base->iface.event_handler != NULL)
    {
        ciot_iface_event_t iface_event = { 0 };
        ciot_uart_status_msg_t status_msg = { 0 };

        status_msg.header.iface = base->iface.info;
        status_msg.header.type = CIOT_MSG_TYPE_EVENT;
        status_msg.status = base->status;

        iface_event.data = (ciot_iface_event_data_u*)&status_msg;
        iface_event.size = sizeof(status_msg);
        iface_event.type = event;

        return base->iface.event_handler(&base->iface, &iface_event, base->iface.event_args);
    }
    return CIOT_OK;
}

#endif
