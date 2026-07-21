/**
 * @file ciot_ble_scn_base.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-06-07
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "ciot_config.h"

#if CIOT_CONFIG_FEATURE_BLE_SCN == 1

#include <stdlib.h>
#include <string.h>
#include "app_util_platform.h"
#include "ciot_log.h"
#include "ciot_ble_scn.h"
#include "ciot_config.h"

#if CIOT_CONFIG_BLE_SCN_ADV_FIFO_ENABLED
static const char *TAG = "ciot_ble_scn";
#endif

static ciot_err_t ciot_ble_scn_process_data(ciot_iface_t *iface, ciot_msg_data_t *data);
static ciot_err_t ciot_ble_scn_get_data(ciot_iface_t *iface, ciot_msg_data_t *msg);
static ciot_err_t ciot_ble_scn_send_data(ciot_iface_t *iface, uint8_t *data, int size);

#if CIOT_CONFIG_BLE_SCN_ADV_FIFO_ENABLED
static ciot_err_t ciot_ble_scn_base_init_fifo(ciot_ble_scn_adv_fifo_t *adv_fifo);
static ciot_err_t ciot_ble_scn_adv_fifo_push(ciot_ble_scn_base_t *base, ciot_ble_scn_event_adv_report_t *adv);
#endif

ciot_err_t ciot_ble_scn_init(ciot_ble_scn_t self)
{
    ciot_ble_scn_base_t *base = (ciot_ble_scn_base_t *)self;

    base->iface.ptr = self;
    base->iface.process_data = ciot_ble_scn_process_data;
    base->iface.get_data = ciot_ble_scn_get_data;
    base->iface.send_data = ciot_ble_scn_send_data;
    base->iface.info.type = CIOT_IFACE_TYPE_BLE_SCN;

#if CIOT_CONFIG_BLE_SCN_ADV_FIFO_ENABLED
    ciot_ble_scn_base_init_fifo(&base->adv_fifo);
#endif

    return CIOT_ERR_OK;
}

static ciot_err_t ciot_ble_scn_process_data(ciot_iface_t *iface, ciot_msg_data_t *data)
{
    CIOT_ERR_TYPE_CHECK(data->which_type, CIOT_MSG_DATA_BLE_SCN_TAG);

    ciot_ble_scn_t self = iface->ptr;
    ciot_ble_scn_data_t *ble_scn = &data->ble_scn;

    switch (ble_scn->which_type)
    {
    case CIOT_BLE_SCN_DATA_STOP_TAG:
        return ciot_ble_scn_stop(self);
    case CIOT_BLE_SCN_DATA_CONFIG_TAG:
        return ciot_ble_scn_start(self, &ble_scn->config);
    case CIOT_BLE_SCN_DATA_REQUEST_TAG:
        return ciot_ble_scn_process_req(self, &ble_scn->request);
    default:
        return CIOT_ERR_INVALID_TYPE;
    }

    return CIOT_ERR_NOT_IMPLEMENTED;
}

static ciot_err_t ciot_ble_scn_get_data(ciot_iface_t *iface, ciot_msg_data_t *data)
{
    CIOT_ERR_TYPE_CHECK(data->which_type, CIOT_MSG_DATA_GET_DATA_TAG);

    ciot_ble_scn_base_t *self = iface->ptr;
    ciot_data_type_t data_type = data->get_data.type;
    data->which_type = CIOT_MSG_DATA_BLE_SCN_TAG;

    switch (data_type)
    {
    case CIOT_DATA_TYPE_CONFIG:
        data->ble_scn.which_type = CIOT_BLE_SCN_DATA_CONFIG_TAG;
        data->ble_scn.config = self->cfg;
        break;
    case CIOT_DATA_TYPE_STATUS:
        data->ble_scn.which_type = CIOT_BLE_SCN_DATA_STATUS_TAG;
        data->ble_scn.status = self->status;
        break;
    default:
        return CIOT_ERR_NOT_FOUND;
    }

    return CIOT_ERR_OK;
}

static ciot_err_t ciot_ble_scn_send_data(ciot_iface_t *iface, uint8_t *data, int size)
{
    return CIOT_ERR_NOT_IMPLEMENTED;
}

ciot_err_t ciot_ble_scn_process_req(ciot_ble_scn_t self, ciot_ble_scn_req_t *req)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(req);
    return CIOT_ERR_NOT_IMPLEMENTED;
}

ciot_err_t ciot_ble_scn_get_cfg(ciot_ble_scn_t self, ciot_ble_scn_cfg_t *cfg)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(cfg);
    ciot_ble_scn_base_t *base = (ciot_ble_scn_base_t *)self;
    *cfg = base->cfg;
    return CIOT_ERR_OK;
}

ciot_err_t ciot_ble_scn_get_status(ciot_ble_scn_t self, ciot_ble_scn_status_t *status)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(status);
    ciot_ble_scn_base_t *base = (ciot_ble_scn_base_t *)self;
    *status = base->status;
    return CIOT_ERR_OK;
}

ciot_err_t ciot_ble_scn_base_task(ciot_ble_scn_t self)
{
    return CIOT_ERR_OK;
}

void ciot_ble_scn_handle_adv_report(ciot_ble_scn_t self, ciot_ble_scn_event_adv_report_t *adv)
{
#if CIOT_CONFIG_BLE_SCN_ADV_FIFO_ENABLED
    ciot_ble_scn_adv_fifo_push((ciot_ble_scn_base_t *)self, adv);
#else
    ciot_ble_scn_base_t *base = (ciot_ble_scn_base_t *)self;
    ciot_iface_send_event_data(&base->iface, CIOT_EVENT_TYPE_DATA, (uint8_t *)adv, sizeof(*adv));
#endif
}

#if CIOT_CONFIG_BLE_SCN_ADV_FIFO_ENABLED
ciot_err_t ciot_ble_scn_adv_fifo_pop(ciot_ble_scn_t self, ciot_ble_scn_adv_t *adv)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(adv);

    ciot_ble_scn_base_t *base = (ciot_ble_scn_base_t *)self;
    ciot_ble_scn_adv_fifo_t *adv_fifo = &base->adv_fifo;
    ciot_ble_scn_adv_fifo_slot_t *slot = NULL;

    if (base->status.fifo_len == 0)
    {
        base->status.err_code = CIOT_ERR_NOT_FOUND;
        return CIOT_ERR_NOT_FOUND;
    }

    slot = &adv_fifo->list[adv_fifo->rp];
    if (slot->locked)
    {
        base->status.err_code = CIOT_ERR_BUSY;
        CIOT_LOGE(TAG, "ADV FIFO pop contention (slot=%d)", adv_fifo->rp);
        return CIOT_ERR_BUSY;
    }

    slot->locked = true;
    *adv = slot->adv; /// copy slot->adv to adv
    slot->adv.has_info = false;
    slot->locked = false;

    if (base->status.fifo_len > base->status.fifo_max)
    {
        base->status.fifo_max = base->status.fifo_len;
    }
    adv_fifo->rp++;
    base->status.fifo_len--;
    if (adv_fifo->rp >= CIOT_CONFIG_BLE_SCN_ADV_FIFO_SIZE)
    {
        adv_fifo->rp = 0;
    }

    return CIOT_ERR_OK;
}

size_t ciot_ble_scn_adv_fifo_count(ciot_ble_scn_t self)
{
    if (self == NULL)
    {
        return 0;
    }

    ciot_ble_scn_base_t *base = (ciot_ble_scn_base_t *)self;
    return base->status.fifo_len;
}

size_t ciot_ble_scn_adv_fifo_lost(ciot_ble_scn_t self)
{
    if (self == NULL)
    {
        return 0;
    }

    ciot_ble_scn_base_t *base = (ciot_ble_scn_base_t *)self;
    return base->status.advs_losted;
}

static ciot_err_t ciot_ble_scn_adv_fifo_push(ciot_ble_scn_base_t *base, ciot_ble_scn_event_adv_report_t *adv)
{
    ciot_ble_scn_adv_fifo_t *adv_fifo = &base->adv_fifo;
    ciot_ble_scn_adv_fifo_slot_t *slot = NULL;

    if (base->status.fifo_len >= CIOT_CONFIG_BLE_SCN_ADV_FIFO_SIZE)
    {
        base->status.advs_losted++;
        base->status.err_code = CIOT_ERR_NO_MEMORY;
        CIOT_LOGE(TAG, "ADV FIFO full (wp=%d rp=%d len=%lu)", adv_fifo->wp, adv_fifo->rp, (unsigned long)base->status.fifo_len);
        return CIOT_ERR_NO_MEMORY;
    }

    slot = &adv_fifo->list[adv_fifo->wp];
    if (slot->locked)
    {
        base->status.advs_losted++;
        base->status.err_code = CIOT_ERR_BUSY;
        CIOT_LOGE(TAG, "ADV FIFO push contention (slot=%d)", adv_fifo->wp);
        return CIOT_ERR_BUSY;
    }

    slot->locked = true;
    slot->adv.has_info = true;
    memcpy(slot->adv.info.mac, adv->mac, sizeof(slot->adv.info.mac));
    slot->adv.info.rssi = adv->rssi;

    const size_t max_payload = sizeof(slot->adv.payload.bytes);
    if (adv->payload_len > max_payload)
    {
        slot->locked = false;
        base->status.advs_losted++;
        base->status.err_code = CIOT_ERR_INVALID_SIZE;
        CIOT_LOGE(TAG, "ADV payload too large (%u > %lu)", adv->payload_len, (unsigned long)max_payload);
        return CIOT_ERR_INVALID_SIZE;
    }
    slot->adv.payload.size = adv->payload_len;
    memcpy(slot->adv.payload.bytes, adv->payload, slot->adv.payload.size);

    adv_fifo->wp++;
    base->status.fifo_len++;

    if (adv_fifo->wp == CIOT_CONFIG_BLE_SCN_ADV_FIFO_SIZE)
    {
        adv_fifo->wp = 0;
    }

    slot->locked = false;
    return CIOT_ERR_OK;
}

#endif

ciot_err_t ciot_ble_scn_set_filter(ciot_ble_scn_t self, ciot_ble_scn_filter_fn *filter, void *args)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(filter);
    ciot_ble_scn_base_t *base = (ciot_ble_scn_base_t *)self;
    base->filter.handler = filter;
    base->filter.args = args;
    return CIOT_ERR_OK;
}

void ciot_ble_scn_copy_mac(uint8_t destiny[6], uint8_t source[6], bool reverse)
{
    for (size_t i = 0; i < 6; i++)
    {
        destiny[i] = reverse ? source[5 - i] : source[i];
    }
}

#if CIOT_CONFIG_BLE_SCN_ADV_FIFO_ENABLED
static ciot_err_t ciot_ble_scn_base_init_fifo(ciot_ble_scn_adv_fifo_t *adv_fifo)
{
    memset(adv_fifo, 0, sizeof(*adv_fifo));
    return CIOT_ERR_OK;
}
#endif

#endif //! CIOT_CONFIG_FEATURE_BLE_SCN == 1