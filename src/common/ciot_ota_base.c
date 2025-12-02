/**
 * @file ciot_ota_base.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-12-11
 *
 * @copyright Copyright (c) 2024
 *
 */
  
#include "ciot_config.h"

#if CIOT_CONFIG_FEATURE_OTA == 1

#include "ciot_ota.h"
#include "ciot_types.h"

static ciot_err_t ciot_ota_process_data(ciot_iface_t *iface, ciot_msg_data_t *data);
static ciot_err_t ciot_ota_get_data(ciot_iface_t *iface, ciot_msg_data_t *data);
static ciot_err_t ciot_ota_send_data(ciot_iface_t *iface, uint8_t *data, int size);
static ciot_err_t ciot_ota_process_cmd(ciot_ota_t self, ciot_ota_cmd_t cmd);

ciot_err_t ciot_ota_init(ciot_ota_t self)
{
    ciot_ota_base_t *base = (ciot_ota_base_t *)self;

    base->iface.ptr = self;
    base->iface.process_data = ciot_ota_process_data;
    base->iface.get_data = ciot_ota_get_data;
    base->iface.send_data = ciot_ota_send_data;
    base->iface.info.type = CIOT_IFACE_TYPE_OTA;
    
    return CIOT_ERR_OK;
}

ciot_err_t ciot_ota_process_req(ciot_ota_t self, ciot_ota_req_t *req)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(req);

    switch (req->which_type)
    {
        case CIOT_OTA_REQ_CMD_TAG:
            return ciot_ota_process_cmd(self, req->cmd);
        default:
            return CIOT_ERR_INVALID_TYPE;
    }
}

static ciot_err_t ciot_ota_process_data(ciot_iface_t *iface, ciot_msg_data_t *data)
{
    CIOT_ERR_TYPE_CHECK(data->which_type, CIOT_MSG_DATA_OTA_TAG);

    ciot_ota_t self = iface->ptr;
    ciot_ota_data_t *ota = &data->ota;

    switch (ota->which_type)
    {
    case CIOT_OTA_DATA_STOP_TAG:
        return ciot_ota_stop(self);
    case CIOT_OTA_DATA_CONFIG_TAG:
        return ciot_ota_start(self, &ota->config);
    case CIOT_OTA_DATA_REQUEST_TAG:
        return ciot_ota_process_req(self, &ota->request);
    default:
        return CIOT_ERR_INVALID_TYPE;
    }

    return CIOT_ERR_NOT_IMPLEMENTED;
}

static ciot_err_t ciot_ota_get_data(ciot_iface_t *iface, ciot_msg_data_t *data)
{
    CIOT_ERR_TYPE_CHECK(data->which_type, CIOT_MSG_DATA_GET_DATA_TAG);

    ciot_ota_base_t *self = iface->ptr;
    ciot_data_type_t data_type = data->get_data.type;
    data->which_type = CIOT_MSG_DATA_OTA_TAG;

    switch (data_type)
    {
    case CIOT_DATA_TYPE_CONFIG:
        data->ota.which_type = CIOT_OTA_DATA_CONFIG_TAG;
        data->ota.config = self->cfg;
        break;
    case CIOT_DATA_TYPE_STATUS:
        data->ota.which_type = CIOT_OTA_DATA_STATUS_TAG;
        data->ota.status = self->status;
        break;
    default:
        return CIOT_ERR_NOT_FOUND;
    }

    return CIOT_ERR_OK;
}

static ciot_err_t ciot_ota_send_data(ciot_iface_t *iface, uint8_t *data, int size)
{
    CIOT_ERR_NULL_CHECK(iface);
    CIOT_ERR_NULL_CHECK(data);
    return CIOT_ERR_NOT_SUPPORTED;
}

static ciot_err_t ciot_ota_process_cmd(ciot_ota_t self, ciot_ota_cmd_t cmd)
{
    CIOT_ERR_NULL_CHECK(self);
    switch (cmd)
    {
    case CIOT_OTA_CMD_ROLLBACK:
        return ciot_ota_rollback(self);
    default:
        return CIOT_ERR_INVALID_TYPE;
    }
}

ciot_err_t ciot_ota_get_cfg(ciot_ota_t self, ciot_ota_cfg_t *cfg)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(cfg);
    ciot_ota_base_t *base = (ciot_ota_base_t*)self;
    *cfg = base->cfg;
    return CIOT_ERR_OK;
}

ciot_err_t ciot_ota_get_status(ciot_ota_t self, ciot_ota_status_t *status)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(status);
    ciot_ota_base_t *base = (ciot_ota_base_t*)self;
    *status = base->status;
    return CIOT_ERR_OK;
}

#endif // CIOT_CONFIG_FEATURE_OTA == 1
