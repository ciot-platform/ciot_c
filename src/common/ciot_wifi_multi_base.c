/**
 * @file ciot_wifi_multi_base.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2026-04-23
 *
 * @copyright Copyright (c) 2026
 *
 */

#include "ciot_config.h"

#if CIOT_CONFIG_FEATURE_WIFI_MULTI == 1

#include <stdlib.h>
#include <string.h>

#include "ciot_wifi_multi.h"
#include "ciot_err.h"
#include "ciot_timer.h"

struct ciot_wifi_multi
{
    ciot_wifi_multi_base_t base;
};

static const char *TAG = "ciot_wifi_multi";

static ciot_err_t ciot_wifi_multi_process_data(ciot_iface_t *iface, ciot_msg_data_t *data);
static ciot_err_t ciot_wifi_multi_get_data(ciot_iface_t *iface, ciot_msg_data_t *data);
static ciot_err_t ciot_wifi_multi_send_data(ciot_iface_t *iface, uint8_t *data, int size);
static ciot_err_t ciot_wifi_multi_wifi_event_handler(ciot_iface_t *sender, ciot_event_t *event, void *args);
static ciot_err_t ciot_wifi_multi_select_next_valid(ciot_wifi_multi_t self, uint32_t start, bool include_start, uint32_t *index);
static ciot_err_t ciot_wifi_multi_switch_to(ciot_wifi_multi_t self, uint32_t index, bool emit_switching_event);
static ciot_err_t ciot_wifi_multi_refresh_status(ciot_wifi_multi_t self);
static ciot_err_t ciot_wifi_multi_refresh_info(ciot_wifi_multi_t self);
static void ciot_wifi_multi_update_valid_count(ciot_wifi_multi_t self);
static void ciot_wifi_multi_schedule_next_switch(ciot_wifi_multi_t self);
static void ciot_wifi_multi_clear_next_switch(ciot_wifi_multi_t self);
static uint64_t ciot_wifi_multi_now_ms(void);

ciot_wifi_multi_t ciot_wifi_multi_new(ciot_wifi_t wifi_sta)
{
    ciot_wifi_multi_t self = calloc(1, sizeof(struct ciot_wifi_multi));
    if (self == NULL)
    {
        return NULL;
    }

    self->base.wifi = wifi_sta;
    self->base.own_wifi = wifi_sta == NULL;
    if (self->base.own_wifi)
    {
        self->base.wifi = ciot_wifi_new(CIOT_WIFI_TYPE_STA);
        if (self->base.wifi == NULL)
        {
            free(self);
            return NULL;
        }
    }

    if (ciot_wifi_multi_init(self) != CIOT_ERR_OK)
    {
        free(self);
        return NULL;
    }

    return self;
}

ciot_err_t ciot_wifi_multi_init(ciot_wifi_multi_t self)
{
    CIOT_ERR_NULL_CHECK(self);
    ciot_wifi_multi_base_t *base = &self->base;

    base->iface.ptr = self;
    base->iface.process_data = ciot_wifi_multi_process_data;
    base->iface.get_data = ciot_wifi_multi_get_data;
    base->iface.send_data = ciot_wifi_multi_send_data;
    base->iface.info.type = CIOT_IFACE_TYPE_WIFI_MULTI;

    base->status.active_index = CIOT_WIFI_MULTI_ACTIVE_INDEX_NONE;
    base->info.active_index = CIOT_WIFI_MULTI_ACTIVE_INDEX_NONE;
    base->status.has_tcp = true;
    base->info.has_tcp = true;
    base->info.has_active_wifi_info = true;
    base->status.next_switch_ms = 0;

    return CIOT_ERR_OK;
}

ciot_err_t ciot_wifi_multi_start(ciot_wifi_multi_t self, ciot_wifi_multi_cfg_t *cfg)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(cfg);

    ciot_wifi_multi_base_t *base = &self->base;

    base->cfg = *cfg;
    if (base->cfg.items_count > (sizeof(base->cfg.items) / sizeof(base->cfg.items[0])))
    {
        base->cfg.items_count = sizeof(base->cfg.items) / sizeof(base->cfg.items[0]);
    }

    if (base->cfg.items_count == 0)
    {
        return CIOT_ERR_INVALID_ARG;
    }

    bool explicit_valid = false;
    uint32_t i;
    for (i = 0; i < base->cfg.items_count; i++)
    {
        if (base->cfg.items[i].valid)
        {
            explicit_valid = true;
            break;
        }
    }

    base->status.items_count = base->cfg.items_count;
    for (i = 0; i < base->status.items_count; i++)
    {
        bool has_ssid = base->cfg.items[i].ssid[0] != '\0';
        bool valid = has_ssid && (base->cfg.items[i].valid || !explicit_valid);
        base->status.items[i].valid = valid;
        base->status.items[i].selected = false;
        base->status.items[i].last_error = CIOT_ERR_OK;
        base->status.items[i].fail_count = 0;
        base->status.items[i].last_attempt_ms = 0;
        base->cfg.items[i].valid = valid;
    }

    ciot_wifi_multi_update_valid_count(self);

    ciot_iface_t *wifi_iface = (ciot_iface_t *)base->wifi;
    if (wifi_iface->event_handler != ciot_wifi_multi_wifi_event_handler)
    {
        base->wifi_event_handler = wifi_iface->event_handler;
        base->wifi_event_args = wifi_iface->event_args;
        CIOT_ERR_RETURN(ciot_iface_set_event_handler(wifi_iface, ciot_wifi_multi_wifi_event_handler, self));
    }

    uint32_t active_index = base->cfg.initial_index;
    if (active_index >= base->status.items_count || !base->status.items[active_index].valid)
    {
        CIOT_ERR_RETURN(ciot_wifi_multi_select_next_valid(self, 0, true, &active_index));
    }

    return ciot_wifi_multi_switch_to(self, active_index, false);
}

ciot_err_t ciot_wifi_multi_stop(ciot_wifi_multi_t self)
{
    CIOT_ERR_NULL_CHECK(self);
    ciot_wifi_multi_base_t *base = &self->base;
    ciot_iface_t *wifi_iface = (ciot_iface_t *)base->wifi;

    if (wifi_iface->event_handler == ciot_wifi_multi_wifi_event_handler)
    {
        CIOT_ERR_PRINT(TAG, ciot_iface_set_event_handler(wifi_iface, base->wifi_event_handler, base->wifi_event_args));
    }

    base->status.active_index = CIOT_WIFI_MULTI_ACTIVE_INDEX_NONE;
    base->info.active_index = CIOT_WIFI_MULTI_ACTIVE_INDEX_NONE;
    ciot_wifi_multi_clear_next_switch(self);
    memset(base->info.active_ssid, 0, sizeof(base->info.active_ssid));

    return ciot_wifi_stop(base->wifi);
}

ciot_err_t ciot_wifi_multi_task(ciot_wifi_multi_t self)
{
    CIOT_ERR_NULL_CHECK(self);
    ciot_wifi_multi_base_t *base = &self->base;

    if (base->status.items_count == 0 || base->cfg.switch_interval_ms == 0)
    {
        ciot_wifi_multi_clear_next_switch(self);
        return CIOT_ERR_OK;
    }

    if (base->status.next_switch_ms == 0)
    {
        ciot_wifi_multi_schedule_next_switch(self);
        return CIOT_ERR_OK;
    }

    if (ciot_wifi_multi_now_ms() < base->status.next_switch_ms)
    {
        return CIOT_ERR_OK;
    }

    ciot_err_t err = ciot_wifi_multi_next(self);
    if (err != CIOT_ERR_OK)
    {
        ciot_wifi_multi_schedule_next_switch(self);
        return err;
    }

    return CIOT_ERR_OK;
}

ciot_err_t ciot_wifi_multi_next(ciot_wifi_multi_t self)
{
    CIOT_ERR_NULL_CHECK(self);
    ciot_wifi_multi_base_t *base = &self->base;

    if (base->status.items_count == 0)
    {
        return CIOT_ERR_INVALID_STATE;
    }

    uint32_t start = 0;
    if (base->status.active_index != CIOT_WIFI_MULTI_ACTIVE_INDEX_NONE)
    {
        start = base->status.active_index + 1;
    }

    uint32_t next_index = 0;
    ciot_err_t err = ciot_wifi_multi_select_next_valid(self, start, true, &next_index);
    if (err != CIOT_ERR_OK)
    {
        base->status.last_error = err;
        ciot_iface_send_event_type(&base->iface, CIOT_WIFI_MULTI_EVENT_NO_VALID_NETWORK);
        return err;
    }

    return ciot_wifi_multi_switch_to(self, next_index, true);
}

ciot_err_t ciot_wifi_multi_mark_invalid(ciot_wifi_multi_t self, uint32_t index, ciot_err_t reason)
{
    CIOT_ERR_NULL_CHECK(self);
    ciot_wifi_multi_base_t *base = &self->base;

    if (index >= base->status.items_count)
    {
        return CIOT_ERR_INVALID_INDEX;
    }

    base->cfg.items[index].valid = false;
    base->status.items[index].valid = false;
    base->status.items[index].selected = false;
    base->status.items[index].last_error = reason;
    base->status.items[index].fail_count++;
    base->status.last_error = reason;
    ciot_wifi_multi_update_valid_count(self);

    ciot_iface_send_event_type(&base->iface, CIOT_WIFI_MULTI_EVENT_ITEM_INVALIDATED);

    if (base->status.active_index != index)
    {
        return CIOT_ERR_OK;
    }

    uint32_t next_index = 0;
    ciot_err_t err = ciot_wifi_multi_select_next_valid(self, index + 1, true, &next_index);
    if (err != CIOT_ERR_OK)
    {
        base->status.active_index = CIOT_WIFI_MULTI_ACTIVE_INDEX_NONE;
        base->info.active_index = CIOT_WIFI_MULTI_ACTIVE_INDEX_NONE;
        memset(base->info.active_ssid, 0, sizeof(base->info.active_ssid));
        ciot_iface_send_event_type(&base->iface, CIOT_WIFI_MULTI_EVENT_NO_VALID_NETWORK);
        CIOT_ERR_PRINT(TAG, ciot_wifi_stop(base->wifi));
        return err;
    }

    return ciot_wifi_multi_switch_to(self, next_index, true);
}

ciot_err_t ciot_wifi_multi_mark_active_invalid(ciot_wifi_multi_t self, ciot_err_t reason)
{
    CIOT_ERR_NULL_CHECK(self);
    ciot_wifi_multi_base_t *base = &self->base;
    if (base->status.active_index == CIOT_WIFI_MULTI_ACTIVE_INDEX_NONE)
    {
        return CIOT_ERR_INVALID_STATE;
    }
    return ciot_wifi_multi_mark_invalid(self, base->status.active_index, reason);
}

ciot_err_t ciot_wifi_multi_reset_invalid(ciot_wifi_multi_t self)
{
    CIOT_ERR_NULL_CHECK(self);
    ciot_wifi_multi_base_t *base = &self->base;

    for (uint32_t i = 0; i < base->status.items_count; i++)
    {
        bool valid = base->cfg.items[i].ssid[0] != '\0';
        base->cfg.items[i].valid = valid;
        base->status.items[i].valid = valid;
        base->status.items[i].last_error = CIOT_ERR_OK;
        if (!base->status.items[i].selected)
        {
            base->status.items[i].fail_count = 0;
        }
    }

    ciot_wifi_multi_update_valid_count(self);

    if (base->status.active_index == CIOT_WIFI_MULTI_ACTIVE_INDEX_NONE && base->status.valid_count > 0)
    {
        uint32_t next_index = 0;
        CIOT_ERR_RETURN(ciot_wifi_multi_select_next_valid(self, 0, true, &next_index));
        return ciot_wifi_multi_switch_to(self, next_index, true);
    }

    return CIOT_ERR_OK;
}

ciot_err_t ciot_wifi_multi_process_req(ciot_wifi_multi_t self, ciot_wifi_multi_req_t *req)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(req);

    switch (req->which_type)
    {
    case CIOT_WIFI_MULTI_REQ_NEXT_TAG:
        return ciot_wifi_multi_next(self);
    case CIOT_WIFI_MULTI_REQ_SET_INVALID_TAG:
        return ciot_wifi_multi_mark_invalid(self, req->set_invalid.index, req->set_invalid.reason);
    case CIOT_WIFI_MULTI_REQ_MARK_ACTIVE_INVALID_TAG:
        return ciot_wifi_multi_mark_active_invalid(self, req->mark_active_invalid.reason);
    case CIOT_WIFI_MULTI_REQ_RESET_INVALID_TAG:
        return ciot_wifi_multi_reset_invalid(self);
    case CIOT_WIFI_MULTI_REQ_GET_ITEM_TAG:
    {
        ciot_wifi_multi_base_t *base = &self->base;
        if (req->get_item.index >= base->status.items_count)
        {
            return CIOT_ERR_INVALID_INDEX;
        }
        return CIOT_ERR_OK;
    }
    default:
        return CIOT_ERR_NOT_SUPPORTED;
    }
}

ciot_err_t ciot_wifi_multi_get_cfg(ciot_wifi_multi_t self, ciot_wifi_multi_cfg_t *cfg)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(cfg);
    *cfg = self->base.cfg;
    return CIOT_ERR_OK;
}

ciot_err_t ciot_wifi_multi_get_status(ciot_wifi_multi_t self, ciot_wifi_multi_status_t *status)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(status);
    CIOT_ERR_PRINT(TAG, ciot_wifi_multi_refresh_status(self));
    *status = self->base.status;
    return CIOT_ERR_OK;
}

ciot_err_t ciot_wifi_multi_get_info(ciot_wifi_multi_t self, ciot_wifi_multi_info_t *info)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(info);
    CIOT_ERR_PRINT(TAG, ciot_wifi_multi_refresh_info(self));
    *info = self->base.info;
    return CIOT_ERR_OK;
}

static ciot_err_t ciot_wifi_multi_process_data(ciot_iface_t *iface, ciot_msg_data_t *data)
{
    CIOT_ERR_TYPE_CHECK(data->which_type, CIOT_MSG_DATA_WIFI_MULTI_TAG);

    ciot_wifi_multi_t self = iface->ptr;
    ciot_wifi_multi_data_t *wifi_multi = &data->wifi_multi;

    switch (wifi_multi->which_type)
    {
    case CIOT_WIFI_MULTI_DATA_STOP_TAG:
        return ciot_wifi_multi_stop(self);
    case CIOT_WIFI_MULTI_DATA_CONFIG_TAG:
        return ciot_wifi_multi_start(self, &wifi_multi->config);
    case CIOT_WIFI_MULTI_DATA_REQUEST_TAG:
        return ciot_wifi_multi_process_req(self, &wifi_multi->request);
    default:
        return CIOT_ERR_INVALID_TYPE;
    }
}

static ciot_err_t ciot_wifi_multi_get_data(ciot_iface_t *iface, ciot_msg_data_t *data)
{
    CIOT_ERR_TYPE_CHECK(data->which_type, CIOT_MSG_DATA_GET_DATA_TAG);

    ciot_wifi_multi_t self = iface->ptr;
    ciot_wifi_multi_base_t *base = &self->base;

    data->which_type = CIOT_MSG_DATA_WIFI_MULTI_TAG;

    switch (data->get_data.type)
    {
    case CIOT_DATA_TYPE_CONFIG:
        data->wifi_multi.which_type = CIOT_WIFI_MULTI_DATA_CONFIG_TAG;
        data->wifi_multi.config = base->cfg;
        break;
    case CIOT_DATA_TYPE_STATUS:
        CIOT_ERR_PRINT(TAG, ciot_wifi_multi_refresh_status(self));
        data->wifi_multi.which_type = CIOT_WIFI_MULTI_DATA_STATUS_TAG;
        data->wifi_multi.status = base->status;
        break;
    case CIOT_DATA_TYPE_INFO:
        CIOT_ERR_PRINT(TAG, ciot_wifi_multi_refresh_info(self));
        data->wifi_multi.which_type = CIOT_WIFI_MULTI_DATA_INFO_TAG;
        data->wifi_multi.info = base->info;
        break;
    default:
        return CIOT_ERR_NOT_FOUND;
    }

    return CIOT_ERR_OK;
}

static ciot_err_t ciot_wifi_multi_send_data(ciot_iface_t *iface, uint8_t *data, int size)
{
    CIOT_ERR_NULL_CHECK(iface);
    CIOT_ERR_NULL_CHECK(data);
    (void)size;
    return CIOT_ERR_NOT_SUPPORTED;
}

static ciot_err_t ciot_wifi_multi_wifi_event_handler(ciot_iface_t *sender, ciot_event_t *event, void *args)
{
    CIOT_ERR_NULL_CHECK(sender);
    CIOT_ERR_NULL_CHECK(event);
    CIOT_ERR_NULL_CHECK(args);

    ciot_wifi_multi_t self = (ciot_wifi_multi_t)args;
    ciot_wifi_multi_base_t *base = &self->base;

    CIOT_ERR_PRINT(TAG, ciot_wifi_multi_refresh_status(self));
    CIOT_ERR_PRINT(TAG, ciot_wifi_multi_refresh_info(self));

    if (event->type == CIOT_EVENT_TYPE_STOPPED && base->status.active_index != CIOT_WIFI_MULTI_ACTIVE_INDEX_NONE)
    {
        CIOT_ERR_PRINT(TAG, ciot_wifi_multi_mark_active_invalid(self, base->status.last_error));
    }

    CIOT_ERR_PRINT(TAG, ciot_iface_send_event_type(&base->iface, event->type));

    if (base->wifi_event_handler != NULL)
    {
        return base->wifi_event_handler(sender, event, base->wifi_event_args);
    }

    return CIOT_ERR_OK;
}

static ciot_err_t ciot_wifi_multi_select_next_valid(ciot_wifi_multi_t self, uint32_t start, bool include_start, uint32_t *index)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(index);

    ciot_wifi_multi_base_t *base = &self->base;
    if (base->status.items_count == 0)
    {
        return CIOT_ERR_NOT_FOUND;
    }

    uint32_t count = base->status.items_count;
    uint32_t offset = include_start ? 0 : 1;
    uint32_t normalized_start = start % count;

    for (uint32_t i = 0; i < count; i++)
    {
        uint32_t current = (normalized_start + i + offset) % count;
        if (base->status.items[current].valid)
        {
            *index = current;
            return CIOT_ERR_OK;
        }
    }

    return CIOT_ERR_NOT_FOUND;
}

static ciot_err_t ciot_wifi_multi_switch_to(ciot_wifi_multi_t self, uint32_t index, bool emit_switching_event)
{
    CIOT_ERR_NULL_CHECK(self);
    ciot_wifi_multi_base_t *base = &self->base;

    if (index >= base->status.items_count)
    {
        return CIOT_ERR_INVALID_INDEX;
    }

    if (!base->status.items[index].valid)
    {
        return CIOT_ERR_INVALID_STATE;
    }

    ciot_wifi_cfg_t wifi_cfg = { 0 };
    CIOT_ERR_PRINT(TAG, ciot_wifi_get_cfg(base->wifi, &wifi_cfg));

    wifi_cfg.disabled = false;
    wifi_cfg.type = CIOT_WIFI_TYPE_STA;
    strncpy(wifi_cfg.ssid, base->cfg.items[index].ssid, sizeof(wifi_cfg.ssid) - 1);
    strncpy(wifi_cfg.password, base->cfg.items[index].password, sizeof(wifi_cfg.password) - 1);

    if (emit_switching_event)
    {
        ciot_iface_send_event_type(&base->iface, CIOT_WIFI_MULTI_EVENT_SWITCHING);
    }

    CIOT_ERR_RETURN(ciot_wifi_start(base->wifi, &wifi_cfg));

    if (base->status.active_index != CIOT_WIFI_MULTI_ACTIVE_INDEX_NONE &&
        base->status.active_index < base->status.items_count)
    {
        base->status.items[base->status.active_index].selected = false;
    }

    base->status.active_index = index;
    base->status.items[index].selected = true;
    base->status.items[index].last_attempt_ms = (uint32_t)ciot_wifi_multi_now_ms();
    base->info.active_index = index;
    strncpy(base->info.active_ssid, base->cfg.items[index].ssid, sizeof(base->info.active_ssid) - 1);
    ciot_wifi_multi_schedule_next_switch(self);

    CIOT_ERR_PRINT(TAG, ciot_wifi_multi_refresh_status(self));
    CIOT_ERR_PRINT(TAG, ciot_wifi_multi_refresh_info(self));

    return CIOT_ERR_OK;
}

static ciot_err_t ciot_wifi_multi_refresh_status(ciot_wifi_multi_t self)
{
    CIOT_ERR_NULL_CHECK(self);
    ciot_wifi_multi_base_t *base = &self->base;
    ciot_wifi_status_t wifi_status = { 0 };
    ciot_err_t err = ciot_wifi_get_status(base->wifi, &wifi_status);
    if (err == CIOT_ERR_OK)
    {
        base->status.has_tcp = true;
        base->status.tcp = wifi_status.tcp;
        base->status.last_error = wifi_status.disconnect_reason;
    }
    return err;
}

static ciot_err_t ciot_wifi_multi_refresh_info(ciot_wifi_multi_t self)
{
    CIOT_ERR_NULL_CHECK(self);
    ciot_wifi_multi_base_t *base = &self->base;
    ciot_wifi_info_t wifi_info = { 0 };
    ciot_err_t err = ciot_wifi_get_info(base->wifi, &wifi_info);
    if (err == CIOT_ERR_OK)
    {
        base->info.has_active_wifi_info = true;
        base->info.active_wifi_info = wifi_info;
        base->info.has_tcp = true;
        base->info.tcp = wifi_info.tcp;
    }
    return err;
}

static void ciot_wifi_multi_update_valid_count(ciot_wifi_multi_t self)
{
    ciot_wifi_multi_base_t *base = &self->base;
    base->status.valid_count = 0;
    for (uint32_t i = 0; i < base->status.items_count; i++)
    {
        if (base->status.items[i].valid)
        {
            base->status.valid_count++;
        }
    }
}

static void ciot_wifi_multi_schedule_next_switch(ciot_wifi_multi_t self)
{
    ciot_wifi_multi_base_t *base = &self->base;

    if (base->cfg.switch_interval_ms == 0 ||
        base->status.items_count == 0 ||
        base->status.active_index == CIOT_WIFI_MULTI_ACTIVE_INDEX_NONE)
    {
        base->status.next_switch_ms = 0;
        return;
    }

    base->status.next_switch_ms = ciot_wifi_multi_now_ms() + base->cfg.switch_interval_ms;
}

static void ciot_wifi_multi_clear_next_switch(ciot_wifi_multi_t self)
{
    self->base.status.next_switch_ms = 0;
}

static uint64_t ciot_wifi_multi_now_ms(void)
{
    return ciot_timer_now() * 1000ULL;
}

#endif // CIOT_CONFIG_FEATURE_WIFI == 1