/**
 * @file ciot_ntp_base.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-12-11
 *
 * @copyright Copyright (c) 2024
 *
 */
  
#include "ciot_config.h"

#if CIOT_CONFIG_FEATURE_NTP == 1

#include "ciot_ntp.h"
#include "ciot_types.h"

static ciot_err_t ciot_ntp_process_data(ciot_iface_t *iface, ciot_msg_data_t *data);
static ciot_err_t ciot_ntp_get_data(ciot_iface_t *iface, ciot_msg_data_t *data);
static ciot_err_t ciot_ntp_send_data(ciot_iface_t *iface, uint8_t *data, int size);

ciot_err_t ciot_ntp_init(ciot_ntp_t self)
{
    ciot_ntp_base_t *base = (ciot_ntp_base_t *)self;

    base->iface.ptr = self;
    base->iface.process_data = ciot_ntp_process_data;
    base->iface.get_data = ciot_ntp_get_data;
    base->iface.send_data = ciot_ntp_send_data;
    base->iface.info.type = CIOT_IFACE_TYPE_NTP;
    
    return CIOT_ERR_OK;
}

ciot_err_t ciot_ntp_process_req(ciot_ntp_t self, ciot_ntp_req_t *req)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(req);
    return CIOT_ERR_NOT_SUPPORTED;    
}

static ciot_err_t ciot_ntp_process_data(ciot_iface_t *iface, ciot_msg_data_t *data)
{
    CIOT_ERR_TYPE_CHECK(data->which_type, CIOT_MSG_DATA_NTP_TAG);

    ciot_ntp_t self = iface->ptr;
    ciot_ntp_data_t *ntp = &data->ntp;

    switch (ntp->which_type)
    {
    case CIOT_NTP_DATA_STOP_TAG:
        return ciot_ntp_stop(self);
    case CIOT_NTP_DATA_CONFIG_TAG:
        return ciot_ntp_start(self, &ntp->config);
    case CIOT_NTP_DATA_REQUEST_TAG:
        return ciot_ntp_process_req(self, &ntp->request);
    default:
        return CIOT_ERR_INVALID_TYPE;
    }

    return CIOT_ERR_NOT_IMPLEMENTED;
}

static ciot_err_t ciot_ntp_get_data(ciot_iface_t *iface, ciot_msg_data_t *data)
{
    CIOT_ERR_TYPE_CHECK(data->which_type, CIOT_MSG_DATA_GET_DATA_TAG);

    ciot_ntp_base_t *self = iface->ptr;
    ciot_data_type_t data_type = data->get_data.type;
    data->which_type = CIOT_MSG_DATA_NTP_TAG;

    switch (data_type)
    {
    case CIOT_DATA_TYPE_CONFIG:
        data->ntp.which_type = CIOT_NTP_DATA_CONFIG_TAG;
        data->ntp.config = self->cfg;
        break;
    case CIOT_DATA_TYPE_STATUS:
        data->ntp.which_type = CIOT_NTP_DATA_STATUS_TAG;
        data->ntp.status = self->status;
        break;
    default:
        return CIOT_ERR_NOT_FOUND;
    }

    return CIOT_ERR_OK;
}

static ciot_err_t ciot_ntp_send_data(ciot_iface_t *iface, uint8_t *data, int size)
{
    CIOT_ERR_NULL_CHECK(iface);
    CIOT_ERR_NULL_CHECK(data);
    return CIOT_ERR_NOT_SUPPORTED;
}

ciot_err_t ciot_ntp_get_cfg(ciot_ntp_t self, ciot_ntp_cfg_t *cfg)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(cfg);
    ciot_ntp_base_t *base = (ciot_ntp_base_t*)self;
    *cfg = base->cfg;
    return CIOT_ERR_OK;
}
ciot_err_t ciot_ntp_get_status(ciot_ntp_t self, ciot_ntp_status_t *status)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(status);
    ciot_ntp_base_t *base = (ciot_ntp_base_t*)self;
    *status = base->status;
    return CIOT_ERR_OK;
}

#endif // CIOT_CONFIG_FEATURE_NTP == 1
