/**
 * @file ciot_base.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-12-11
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "ciot.h"
#include "ciot_types.h"

static ciot_err_t ciot_process_data(ciot_iface_t *iface, ciot_msg_data_t *data);
static ciot_err_t ciot_get_data(ciot_iface_t *iface, ciot_msg_data_t *data);
static ciot_err_t ciot_send_data(ciot_iface_t *iface, uint8_t *data, int size);

ciot_err_t ciot_init(ciot_t self)
{
    self->iface.ptr = self;
    self->iface.process_data = ciot_process_data;
    self->iface.get_data = ciot_get_data;
    self->iface.send_data = ciot_send_data;
    self->iface.info.type = CIOT_IFACE_TYPE_CIOT;
    return CIOT_ERR_OK;
}

ciot_err_t ciot_process_req(ciot_t self, ciot_req_t *req)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(req);

    switch (req->which_type)
    {
    case CIOT_REQ_SAVE_CFG_TAG:
        self->iface.req_status.state = CIOT_IFACE_REQ_STATE_IDLE;
        return ciot_save_cfg(self, &req->save_cfg);
    case CIOT_REQ_DELETE_CFG_TAG:
        self->iface.req_status.state = CIOT_IFACE_REQ_STATE_IDLE;
        return ciot_delete_cfg(self, &req->delete_cfg);
    }

    return CIOT_ERR_INVALID_TYPE;    
}

static ciot_err_t ciot_process_data(ciot_iface_t *iface, ciot_msg_data_t *data)
{
    CIOT_ERR_TYPE_CHECK(data->which_type, CIOT_MSG_DATA_CIOT_TAG);

    ciot_t self = iface->ptr;
    ciot_data_t *ciot = &data->ciot;

    switch (ciot->which_type)
    {
    case CIOT_DATA_REQUEST_TAG:
        return ciot_process_req(self, &ciot->request);
    default:
        return CIOT_ERR_INVALID_TYPE;
    }

    return CIOT_ERR_INVALID_TYPE;
}

static ciot_err_t ciot_get_data(ciot_iface_t *iface, ciot_msg_data_t *data)
{
    CIOT_ERR_TYPE_CHECK(data->which_type, CIOT_MSG_DATA_GET_DATA_TAG);

    ciot_t self = iface->ptr;
    ciot_data_type_t data_type = data->get_data.type;
    data->which_type = CIOT_MSG_DATA_CIOT_TAG;

    switch (data_type)
    {
    case CIOT_DATA_TYPE_CONFIG:
        data->which_type = 0;
        return CIOT_ERR_NOT_FOUND;
    case CIOT_DATA_TYPE_STATUS:
        data->ciot.which_type = CIOT_DATA_STATUS_TAG;
        data->ciot.status = self->status;
        break;
    case CIOT_DATA_TYPE_INFO:
        data->ciot.which_type = CIOT_DATA_INFO_TAG;
        ciot_get_ifaces_info(self, &self->info);
        data->ciot.info = self->info;
        break;
    default:
        break;
    }

    return CIOT_ERR_OK;
}

static ciot_err_t ciot_send_data(ciot_iface_t *iface, uint8_t *data, int size)
{
    CIOT_ERR_NULL_CHECK(iface);
    CIOT_ERR_NULL_CHECK(data);
    return CIOT_ERR_NOT_SUPPORTED;
}
