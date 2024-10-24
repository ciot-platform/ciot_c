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

#include <stdlib.h>
#include <string.h>
#include "ciot_ble.h"
#include "ciot_config.h"

// static const char *TAG = "ciot_ble";

static ciot_err_t ciot_iface_process_req(ciot_iface_t *iface, ciot_msg_t *req);
static ciot_err_t ciot_iface_get_data(ciot_iface_t *iface, ciot_msg_t *msg);
static ciot_err_t ciot_iface_send_data(ciot_iface_t *iface, uint8_t *data, int size);

ciot_err_t ciot_ble_init(ciot_ble_t self)
{
    ciot_ble_base_t *base = (ciot_ble_base_t*)self;

    ciot_iface_init(&base->iface);
    ciot__ble_data__init(&base->data);
    ciot__ble_cfg__init(&base->cfg);
    ciot__ble_status__init(&base->status);
    ciot__ble_info__init(&base->info);

    base->iface.ptr = self;
    base->iface.process_req = ciot_iface_process_req;
    base->iface.get_data = ciot_iface_get_data;
    base->iface.send_data = ciot_iface_send_data;
    base->iface.info.type = CIOT__IFACE_TYPE__IFACE_TYPE_BLE;

    base->info.sw_mac.data = base->macs.sw;
    base->info.sw_mac.len = sizeof(base->macs.sw);
    base->info.hw_mac.data = base->macs.hw;
    base->info.hw_mac.len = sizeof(base->macs.hw);

    return CIOT__ERR__OK;
}

static ciot_err_t ciot_iface_process_req(ciot_iface_t *iface, ciot_msg_t *req)
{
    ciot_ble_t self = iface->ptr;

    switch (req->type)
    {
        case CIOT__MSG_TYPE__MSG_TYPE_START:
            return ciot_ble_start(self, req->data->ble->config);
        case CIOT__MSG_TYPE__MSG_TYPE_STOP:
            return ciot_ble_stop(self);
        case CIOT__MSG_TYPE__MSG_TYPE_REQUEST:
            return ciot_ble_process_req(self, req->data->ble->request);
    default:
        break;
    }

    return CIOT__ERR__OK;
}

static ciot_err_t ciot_iface_get_data(ciot_iface_t *iface, ciot_msg_t *msg)
{
    ciot_ble_base_t *self = iface->ptr;

    self->data.config = NULL;
    self->data.status = NULL;
    self->data.info = NULL;
    self->data.request = NULL;

    switch (msg->type)
    {
    case CIOT__MSG_TYPE__MSG_TYPE_CONFIG:
        self->data.config = &self->cfg;
        break;
    case CIOT__MSG_TYPE__MSG_TYPE_STATUS:
        self->data.status = &self->status;
        break;
    case CIOT__MSG_TYPE__MSG_TYPE_INFO:
        self->data.info = &self->info;
        break;
    default:
        break;
    }

    self->iface.data.ble = &self->data;
    msg->data = &self->iface.data;

    return CIOT__ERR__OK;
}

static ciot_err_t ciot_iface_send_data(ciot_iface_t *iface, uint8_t *data, int size)
{
    return CIOT__ERR__NOT_IMPLEMENTED;
}

ciot_err_t ciot_ble_process_req(ciot_ble_t self, ciot_ble_req_t *req)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(req);
    ciot_ble_base_t *base = (ciot_ble_base_t*)self;
    switch (req->type)
    {
    case CIOT__BLE_REQ_TYPE__BLE_REQ_TYPE_SET_MAC:
        CIOT_ERR_RETURN(ciot_ble_set_mac(self, req->set_mac.data));
        if(memcmp(base->macs.sw, req->set_mac.data, sizeof(base->macs.sw)) == 0)
        {
            base->iface.req_status.state = CIOT_IFACE_REQ_STATE_IDLE;
        }
        return CIOT__ERR__OK;
    default:
        break;
    }
    return CIOT__ERR__INVALID_TYPE;
}

ciot_err_t ciot_ble_get_cfg(ciot_ble_t self, ciot_ble_cfg_t *cfg)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(cfg);
    ciot_ble_base_t *base = (ciot_ble_base_t*)self;
    *cfg = base->cfg;
    return CIOT__ERR__OK;
}

ciot_err_t ciot_ble_get_status(ciot_ble_t self, ciot_ble_status_t *status)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(status);
    ciot_ble_base_t *base = (ciot_ble_base_t*)self;
    *status = base->status;
    return CIOT__ERR__OK;
}

ciot_err_t ciot_ble_get_info(ciot_ble_t self, ciot_ble_info_t *info)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(info);
    ciot_ble_base_t *base = (ciot_ble_base_t*)self;
    *info = base->info;
    return CIOT__ERR__OK;
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
    return CIOT__ERR__OK;
}