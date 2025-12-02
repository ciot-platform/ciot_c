/**
 * @file ciot_http_client_base.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-12-11
 *
 * @copyright Copyright (c) 2024
 *
 */
 
#include "ciot_config.h"

#if CIOT_CONFIG_FEATURE_HTTP_CLIENT == 1
 
#include "ciot_http_client.h"
#include "ciot_types.h"

static ciot_err_t ciot_http_client_process_data(ciot_iface_t *iface, ciot_msg_data_t *data);
static ciot_err_t ciot_http_client_get_data(ciot_iface_t *iface, ciot_msg_data_t *data);
static ciot_err_t ciot_http_client_send_data(ciot_iface_t *iface, uint8_t *data, int size);

ciot_err_t ciot_http_client_init(ciot_http_client_t self)
{
    ciot_http_client_base_t *base = (ciot_http_client_base_t *)self;

    base->iface.ptr = self;
    base->iface.process_data = ciot_http_client_process_data;
    base->iface.get_data = ciot_http_client_get_data;
    base->iface.send_data = ciot_http_client_send_data;
    base->iface.info.type = CIOT_IFACE_TYPE_HTTP_CLIENT;
    return CIOT_ERR_OK;
}

ciot_err_t ciot_http_client_process_req(ciot_http_client_t self, ciot_http_client_req_t *req)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(req);
    return CIOT_ERR_NOT_SUPPORTED;    
}

static ciot_err_t ciot_http_client_process_data(ciot_iface_t *iface, ciot_msg_data_t *data)
{
    CIOT_ERR_TYPE_CHECK(data->which_type, CIOT_MSG_DATA_HTTP_CLIENT_TAG);

    ciot_http_client_t self = iface->ptr;
    ciot_http_client_data_t *http_client = &data->http_client;

    switch (http_client->which_type)
    {
    case CIOT_HTTP_CLIENT_DATA_STOP_TAG:
        return ciot_http_client_stop(self);
    case CIOT_HTTP_CLIENT_DATA_CONFIG_TAG:
        return ciot_http_client_start(self, &http_client->config);
    case CIOT_HTTP_CLIENT_DATA_REQUEST_TAG:
        return ciot_http_client_process_req(self, &http_client->request);
    default:
        return CIOT_ERR_INVALID_TYPE;
    }

    return CIOT_ERR_NOT_IMPLEMENTED;
}

static ciot_err_t ciot_http_client_get_data(ciot_iface_t *iface, ciot_msg_data_t *data)
{
    CIOT_ERR_TYPE_CHECK(data->which_type, CIOT_MSG_DATA_GET_DATA_TAG);

    ciot_http_client_base_t *self = iface->ptr;
    ciot_data_type_t data_type = data->get_data.type;
    data->which_type = CIOT_MSG_DATA_HTTP_CLIENT_TAG;

    switch (data_type)
    {
    case CIOT_DATA_TYPE_CONFIG:
        data->http_client.which_type = CIOT_HTTP_CLIENT_DATA_CONFIG_TAG;
        data->http_client.config = self->cfg;
        break;
    case CIOT_DATA_TYPE_STATUS:
        data->http_client.which_type = CIOT_HTTP_CLIENT_DATA_STATUS_TAG;
        data->http_client.status = self->status;
        break;
    default:
        return CIOT_ERR_NOT_FOUND;
    }

    return CIOT_ERR_OK;
}

static ciot_err_t ciot_http_client_send_data(ciot_iface_t *iface, uint8_t *data, int size)
{
    CIOT_ERR_NULL_CHECK(iface);
    CIOT_ERR_NULL_CHECK(data);
    ciot_http_client_t self = (ciot_http_client_t)iface;
    return ciot_http_client_send_bytes(self, data, size);
}

#endif // CIOT_CONFIG_FEATURE_http_client == 1
