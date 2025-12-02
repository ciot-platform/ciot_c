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
#include "ciot_ble_scn.h"
#include "ciot_config.h"

// static const char *TAG = "ciot_ble_scn";

static ciot_err_t ciot_ble_scn_process_data(ciot_iface_t *iface, ciot_msg_data_t *data);
static ciot_err_t ciot_ble_scn_get_data(ciot_iface_t *iface, ciot_msg_data_t *msg);
static ciot_err_t ciot_ble_scn_send_data(ciot_iface_t *iface, uint8_t *data, int size);

#ifdef CIOT_CONFIG_BLE_SCN_ADV_FIFO_SIZE
static ciot_err_t ciot_ble_scn_base_init_fifo(ciot_ble_scn_adv_fifo_t *adv_fifo);
#endif

ciot_err_t ciot_ble_scn_init(ciot_ble_scn_t self)
{
    ciot_ble_scn_base_t *base = (ciot_ble_scn_base_t*)self;

    base->iface.ptr = self;
    base->iface.process_data = ciot_ble_scn_process_data;
    base->iface.get_data = ciot_ble_scn_get_data;
    base->iface.send_data = ciot_ble_scn_send_data;
    base->iface.info.type = CIOT_IFACE_TYPE_BLE_SCN;
    base->recv.has_info = true;

#if CIOT_CONFIG_BLE_SCN_ADV_FIFO_SIZE
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
    data->which_type = CIOT_MSG_DATA_BLE_TAG;

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
    ciot_ble_scn_base_t *base = (ciot_ble_scn_base_t*)self;
    *cfg = base->cfg;
    return CIOT_ERR_OK;
}

ciot_err_t ciot_ble_scn_get_status(ciot_ble_scn_t self, ciot_ble_scn_status_t *status)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(status);
    ciot_ble_scn_base_t *base = (ciot_ble_scn_base_t*)self;
    *status = base->status;
    return CIOT_ERR_OK;
}

ciot_err_t ciot_ble_scn_base_task(ciot_ble_scn_t self)
{
    #if CIOT_CONFIG_BLE_SCN_ADV_FIFO_SIZE
    CIOT_ERR_NULL_CHECK(self);
    ciot_ble_scn_base_t *base = (ciot_ble_scn_base_t*)self;
    ciot_ble_scn_adv_fifo_t *adv_fifo = &base->adv_fifo;
	if(base->status.fifo_len > 0)
	{
		if(adv_fifo->list[adv_fifo->rp].info.rssi != 0)
		{
			// ciot_iface_event_t event = {0};
            // event.type = CIOT_IFACE_EVENT_DATA;
            // event.data = (uint8_t*)&adv_fifo->list[adv_fifo->rp];
            ciot_event_t event = { 0 };
            event.type = CIOT_EVENT_TYPE_DATA,
            memcpy(event.raw.bytes, (uint8_t*)&adv_fifo->list[adv_fifo->rp], sizeof(ciot_ble_scn_adv_t));
            event.raw.size = sizeof(ciot_ble_scn_adv_t);
            ciot_iface_send_event(&base->iface, &event);
			adv_fifo->list[adv_fifo->rp].info.rssi = 0;
			adv_fifo->rp++;
			base->status.fifo_len--;
			if(adv_fifo->rp >= CIOT_CONFIG_BLE_SCN_ADV_FIFO_SIZE)
			{
				adv_fifo->rp = 0;
			}
		}
        else
        {
            base->status.err_code = CIOT_ERR_FIFO_READ;
            adv_fifo->rp++;
        }
	}
#endif
    return CIOT_ERR_OK;
}

void ciot_ble_scn_handle_adv_report(ciot_ble_scn_t self, ciot_ble_scn_adv_t *adv)
{
    ciot_ble_scn_base_t *base = (ciot_ble_scn_base_t*)self;
#ifdef CIOT_CONFIG_BLE_SCN_ADV_FIFO_SIZE
    ciot_ble_scn_adv_fifo_t *adv_fifo = &base->adv_fifo;
    if(adv_fifo->list[adv_fifo->wp].info.rssi == 0)
    {
        adv_fifo->list[adv_fifo->wp] = *adv;
        adv_fifo->wp++;
        base->status.fifo_len++;
        if(base->status.fifo_len > base->status.fifo_max)
        {
            base->status.fifo_max = base->status.fifo_len;
        }
        if(adv_fifo->wp == CIOT_CONFIG_BLE_SCN_ADV_FIFO_SIZE)
        {
            adv_fifo->wp = 0;
        }
    }
    else
    {
        adv_fifo->rp = adv_fifo->wp;
        base->status.advs_losted++;
        if(base->status.fifo_len == 0)
        {
            base->status.err_code = CIOT_ERR_INVALID_SIZE;
            for (size_t i = 0; i < BOARD_BLE_SCN_ADV_FIFO_SIZE; i++)
            {
                if(adv_fifo->list[i].info.rssi != 0) base->status.fifo_len++;
            }
        }
    }
#else
    ciot_event_t event = {0};
    event.type = CIOT_EVENT_TYPE_DATA;
    event.raw.size = sizeof(*adv);
    memcpy(event.raw.bytes, adv, sizeof(*adv));
    ciot_iface_send_event(&base->iface, &event);
#endif
}


ciot_err_t ciot_ble_scn_set_filter(ciot_ble_scn_t self, ciot_ble_scn_filter_fn *filter, void *args)
{
    CIOT_ERR_NULL_CHECK(self);
	CIOT_ERR_NULL_CHECK(filter);
    ciot_ble_scn_base_t *base = (ciot_ble_scn_base_t*)self;
    base->filter.handler = filter;
    base->filter.args = args;
    // base->filter.handler = filter;
    // base->filter.args = args;
    return CIOT_ERR_OK;
}

#if CIOT_CONFIG_BLE_SCN_ADV_FIFO_SIZE
static ciot_err_t ciot_ble_scn_base_init_fifo(ciot_ble_scn_adv_fifo_t *adv_fifo)
{
    // for (size_t i = 0; i < CIOT_CONFIG_BLE_SCN_ADV_FIFO_SIZE; i++)
    // {
    //     ciot__ble_scn_adv__init(&adv_fifo->list[i]);
    //     ciot__ble_scn_adv_info__init(&adv_fifo->data.infos[i]);
    //     adv_fifo->list[i].info = &adv_fifo->data.infos[i];
    //     adv_fifo->list[i].info->mac.data = adv_fifo->data.macs[i];
    //     adv_fifo->list[i].payload.data = adv_fifo->data.advs[i];
    // }
    return CIOT_ERR_OK;
}
#endif

#endif  //!CIOT_CONFIG_FEATURE_BLE_SCN == 1