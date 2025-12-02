/**
 * @file ciot_mbus_client_base.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-12-11
 *
 * @copyright Copyright (c) 2024
 *
 */

 
#include "ciot_config.h"

#if CIOT_CONFIG_FEATURE_MBUS_CLIENT == 1

#include "ciot_mbus_client.h"
#include "ciot_types.h"

static ciot_err_t ciot_mbus_client_process_data(ciot_iface_t *iface, ciot_msg_data_t *data);
static ciot_err_t ciot_mbus_client_get_data(ciot_iface_t *iface, ciot_msg_data_t *data);

ciot_err_t ciot_mbus_client_init(ciot_mbus_client_t self)
{
    ciot_mbus_client_base_t *base = (ciot_mbus_client_base_t *)self;

    base->iface.ptr = self;
    base->iface.process_data = ciot_mbus_client_process_data;
    base->iface.get_data = ciot_mbus_client_get_data;
    base->iface.info.type = CIOT_IFACE_TYPE_MBUS_CLIENT;
    return CIOT_ERR_OK;
}

ciot_err_t ciot_mbus_client_process_req(ciot_mbus_client_t self, ciot_mbus_client_req_t *req)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(req);
    return CIOT_ERR_NOT_SUPPORTED;    
}

static ciot_err_t ciot_mbus_client_process_data(ciot_iface_t *iface, ciot_msg_data_t *data)
{
    CIOT_ERR_TYPE_CHECK(data->which_type, CIOT_MSG_DATA_MBUS_CLIENT_TAG);

    ciot_mbus_client_t self = iface->ptr;
    ciot_mbus_client_data_t *mbus_client = &data->mbus_client;

    switch (mbus_client->which_type)
    {
    case CIOT_MBUS_CLIENT_DATA_STOP_TAG:
        return ciot_mbus_client_stop(self);
    case CIOT_MBUS_CLIENT_DATA_CONFIG_TAG:
        return ciot_mbus_client_start(self, &mbus_client->config);
    case CIOT_MBUS_CLIENT_DATA_REQUEST_TAG:
        return ciot_mbus_client_process_req(self, &mbus_client->request);
    default:
        return CIOT_ERR_INVALID_TYPE;
    }

    return CIOT_ERR_NOT_IMPLEMENTED;
}

static ciot_err_t ciot_mbus_client_get_data(ciot_iface_t *iface, ciot_msg_data_t *data)
{
    CIOT_ERR_TYPE_CHECK(data->which_type, CIOT_MSG_DATA_GET_DATA_TAG);

    ciot_mbus_client_base_t *self = iface->ptr;
    ciot_data_type_t data_type = data->get_data.type;
    data->which_type = CIOT_MSG_DATA_MBUS_CLIENT_TAG;

    switch (data_type)
    {
    case CIOT_DATA_TYPE_CONFIG:
        data->mbus_client.which_type = CIOT_MBUS_CLIENT_DATA_CONFIG_TAG;
        data->mbus_client.config = self->cfg;
        break;
    case CIOT_DATA_TYPE_STATUS:
        data->mbus_client.which_type = CIOT_MBUS_CLIENT_DATA_STATUS_TAG;
        data->mbus_client.status = self->status;
        break;
    default:
        return CIOT_ERR_NOT_FOUND;
    }

    return CIOT_ERR_OK;
}

#endif // CIOT_CONFIG_FEATURE_MBUS_CLIENT == 1