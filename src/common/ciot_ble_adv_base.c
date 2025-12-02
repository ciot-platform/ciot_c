/**
 * @file ciot_ble_adv_base.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "ciot_config.h"

#if CIOT_CONFIG_FEATURE_BLE_ADV == 1

#include <stdlib.h>
#include "ciot_ble_adv.h"
#include "ciot_config.h"

// static const char *TAG = "ciot_ble_adv";

static ciot_err_t ciot_ble_adv_process_data(ciot_iface_t *iface, ciot_msg_data_t *data);
static ciot_err_t ciot_ble_adv_get_data(ciot_iface_t *iface, ciot_msg_data_t *msg);
static ciot_err_t ciot_ble_adv_send_data(ciot_iface_t *iface, uint8_t *data, int size);

ciot_err_t ciot_ble_adv_init(ciot_ble_adv_t self)
{
    ciot_ble_adv_base_t *base = (ciot_ble_adv_base_t*)self;

    base->iface.ptr = self;
    base->iface.process_data = ciot_ble_adv_process_data;
    base->iface.get_data = ciot_ble_adv_get_data;
    base->iface.send_data = ciot_ble_adv_send_data;
    base->iface.info.type = CIOT_IFACE_TYPE_BLE_ADV;

    return CIOT_ERR_OK;
}

static ciot_err_t ciot_ble_adv_process_data(ciot_iface_t *iface, ciot_msg_data_t *data)
{
    CIOT_ERR_TYPE_CHECK(data->which_type, CIOT_MSG_DATA_BLE_ADV_TAG);

    ciot_ble_adv_t self = iface->ptr;
    ciot_ble_adv_data_t *ble_adv = &data->ble_adv;

    switch (ble_adv->which_type)
    {
    case CIOT_BLE_ADV_DATA_STOP_TAG:
        return ciot_ble_adv_stop(self);
    case CIOT_BLE_ADV_DATA_CONFIG_TAG:
        return ciot_ble_adv_start(self, &ble_adv->config);
    case CIOT_BLE_ADV_DATA_REQUEST_TAG:
        return ciot_ble_adv_process_req(self, &ble_adv->request);
    default:
        return CIOT_ERR_INVALID_TYPE;
    }

    return CIOT_ERR_NOT_IMPLEMENTED;
}

static ciot_err_t ciot_ble_adv_get_data(ciot_iface_t *iface, ciot_msg_data_t *data)
{
    CIOT_ERR_TYPE_CHECK(data->which_type, CIOT_MSG_DATA_GET_DATA_TAG);

    ciot_ble_adv_base_t *self = iface->ptr;
    ciot_data_type_t data_type = data->get_data.type;
    data->which_type = CIOT_MSG_DATA_BLE_ADV_TAG;

    switch (data_type)
    {
    case CIOT_DATA_TYPE_CONFIG:
        data->ble_adv.which_type = CIOT_BLE_ADV_DATA_CONFIG_TAG;
        data->ble_adv.config = self->cfg;
        break;
    case CIOT_DATA_TYPE_STATUS:
        data->ble_adv.which_type = CIOT_BLE_ADV_DATA_STATUS_TAG;
        data->ble_adv.status = self->status;
        break;
    default:
        return CIOT_ERR_NOT_FOUND;
    }

    return CIOT_ERR_OK;
}

static ciot_err_t ciot_ble_adv_send_data(ciot_iface_t *iface, uint8_t *data, int size)
{
    ciot_ble_adv_t self = (ciot_ble_adv_t)iface;
    return ciot_ble_adv_send_bytes(self, data, size); 
}

ciot_err_t ciot_ble_adv_process_req(ciot_ble_adv_t self, ciot_ble_adv_req_t *req)
{
    return CIOT_ERR_NOT_IMPLEMENTED;
}

ciot_err_t ciot_ble_adv_get_cfg(ciot_ble_adv_t self, ciot_ble_adv_cfg_t *cfg)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(cfg);
    ciot_ble_adv_base_t *base = (ciot_ble_adv_base_t*)self;
    *cfg = base->cfg;
    return CIOT_ERR_OK;
}

ciot_err_t ciot_ble_adv_get_status(ciot_ble_adv_t self, ciot_ble_adv_status_t *status)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(status);
    ciot_ble_adv_base_t *base = (ciot_ble_adv_base_t*)self;
    *status = base->status;
    return CIOT_ERR_OK;
}

#endif  //!CIOT_CONFIG_FEATURE_BLE_ADV == 1