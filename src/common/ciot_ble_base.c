/**
 * @file ciot_ble_base.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */

 
#include "ciot_config.h"

#if CIOT_CONFIG_FEATURE_BLE == 1

#include <stdlib.h>
#include <string.h>
#include "ciot_ble.h"
#include "ciot_config.h"

// static const char *TAG = "ciot_ble";

static ciot_err_t ciot_ble_process_data(ciot_iface_t *iface, ciot_msg_data_t *data);
static ciot_err_t ciot_ble_get_data(ciot_iface_t *iface, ciot_msg_data_t *msg);
static ciot_err_t ciot_ble_send_data(ciot_iface_t *iface, uint8_t *data, int size);

ciot_err_t ciot_ble_init(ciot_ble_t self)
{
    ciot_ble_base_t *base = (ciot_ble_base_t*)self;

    base->iface.ptr = self;
    base->iface.process_data = ciot_ble_process_data;
    base->iface.get_data = ciot_ble_get_data;
    base->iface.send_data = ciot_ble_send_data;
    base->iface.info.type = CIOT_IFACE_TYPE_BLE;

    return CIOT_ERR_OK;
}

static ciot_err_t ciot_ble_process_data(ciot_iface_t *iface, ciot_msg_data_t *data)
{
    CIOT_ERR_TYPE_CHECK(data->which_type, CIOT_MSG_DATA_BLE_TAG);

    ciot_ble_t self = iface->ptr;
    ciot_ble_data_t *ble = &data->ble;

    switch (ble->which_type)
    {
    case CIOT_BLE_DATA_STOP_TAG:
        return ciot_ble_stop(self);
    case CIOT_BLE_DATA_CONFIG_TAG:
        return ciot_ble_start(self, &ble->config);
    case CIOT_BLE_DATA_REQUEST_TAG:
        return ciot_ble_process_req(self, &ble->request);
    default:
        return CIOT_ERR_INVALID_TYPE;
    }

    return CIOT_ERR_NOT_IMPLEMENTED;
}

static ciot_err_t ciot_ble_get_data(ciot_iface_t *iface, ciot_msg_data_t *data)
{
    CIOT_ERR_TYPE_CHECK(data->which_type, CIOT_MSG_DATA_GET_DATA_TAG);

    ciot_ble_base_t *self = iface->ptr;
    ciot_data_type_t data_type = data->get_data.type;
    data->which_type = CIOT_MSG_DATA_BLE_TAG;

    switch (data_type)
    {
    case CIOT_DATA_TYPE_CONFIG:
        data->ble.which_type = CIOT_BLE_DATA_CONFIG_TAG;
        data->ble.config = self->cfg;
        break;
    case CIOT_DATA_TYPE_STATUS:
        data->ble.which_type = CIOT_BLE_DATA_STATUS_TAG;
        data->ble.status = self->status;
        break;
    default:
        return CIOT_ERR_NOT_FOUND;
    }

    return CIOT_ERR_OK;
}

static ciot_err_t ciot_ble_send_data(ciot_iface_t *iface, uint8_t *data, int size)
{
    return CIOT_ERR_NOT_SUPPORTED;
}

ciot_err_t ciot_ble_process_req(ciot_ble_t self, ciot_ble_req_t *req)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(req);
    ciot_ble_base_t *base = (ciot_ble_base_t*)self;
    switch (req->which_type)
    {
    case CIOT_BLE_REQ_SET_MAC_TAG:
        CIOT_ERR_RETURN(ciot_ble_set_mac(self, req->set_mac));
        if(memcmp(base->cfg.mac, req->set_mac, 6) == 0)
        {
            base->iface.req_status.state = CIOT_IFACE_REQ_STATE_IDLE;
        }
        return CIOT_ERR_OK;
    default:
        break;
    }
    return CIOT_ERR_INVALID_TYPE;
}

ciot_err_t ciot_ble_get_cfg(ciot_ble_t self, ciot_ble_cfg_t *cfg)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(cfg);
    ciot_ble_base_t *base = (ciot_ble_base_t*)self;
    *cfg = base->cfg;
    return CIOT_ERR_OK;
}

ciot_err_t ciot_ble_get_status(ciot_ble_t self, ciot_ble_status_t *status)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(status);
    ciot_ble_base_t *base = (ciot_ble_base_t*)self;
    *status = base->status;
    return CIOT_ERR_OK;
}

ciot_err_t ciot_ble_get_info(ciot_ble_t self, ciot_ble_info_t *info)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(info);
    ciot_ble_base_t *base = (ciot_ble_base_t*)self;
    *info = base->info;
    return CIOT_ERR_OK;
}

bool ciot_ble_mac_is_valid(uint8_t mac[6])
{
    for (size_t i = 0; i < 6; i++)
    {
        if (mac[i] != 0)
        {
            return true;
        }
    }
    return false;
}

ciot_err_t ciot_ble_set_ifaces(ciot_ble_t self, ciot_ble_ifaces_t *ifaces)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(ifaces);
    ciot_ble_base_t *base = (ciot_ble_base_t*)self;
    base->ifaces = *ifaces;
    return CIOT_ERR_OK;
}

#endif
