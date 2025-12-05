/**
 * @file ciot_wifi.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "ciot_config.h"

#if CIOT_CONFIG_FEATURE_WIFI == 1 && defined(CIOT_PLATFORM_WIN)

#include <stdlib.h>
#include <string.h>
#include "ciot_wifi.h"
#include "ciot_err.h"

struct ciot_wifi
{
    ciot_wifi_base_t base;
};

static const char *TAG = "ciot_wifi";

static ciot_err_t ciot_wifi_set_cfg(ciot_wifi_t self, ciot_wifi_cfg_t *cfg);
static ciot_err_t ciot_wifi_update_ap_list(ciot_wifi_t self);

ciot_wifi_t ciot_wifi_new(ciot_wifi_type_t type)
{
    ciot_wifi_t self = calloc(1, sizeof(struct ciot_wifi));
    ciot_wifi_base_t *base = &self->base;

    if (type == CIOT_WIFI_TYPE_AP)
    {
        CIOT_LOGI(TAG, "Creating wifi ap");
        base->tcp = ciot_tcp_new(&base->iface, CIOT_TCP_TYPE_WIFI_AP);
    }

    if (type == CIOT_WIFI_TYPE_STA)
    {
        CIOT_LOGI(TAG, "Creating wifi sta");
        base->tcp = ciot_tcp_new(&base->iface, CIOT_TCP_TYPE_WIFI_STA);
    }

    ciot_wifi_init(self);
    base->cfg.type = type;

    return self;
}

ciot_err_t ciot_wifi_start(ciot_wifi_t self, ciot_wifi_cfg_t *cfg)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(cfg);

    ciot_wifi_base_t *base = &self->base;
    ciot_tcp_base_t *tcp = (ciot_tcp_base_t *)&base->tcp;

    CIOT_ERR_RETURN(ciot_wifi_set_cfg(self, cfg));

    return ciot_tcp_start(base->tcp);
}

ciot_err_t ciot_wifi_stop(ciot_wifi_t self)
{
    return CIOT_ERR_NOT_SUPPORTED;
}

ciot_err_t ciot_wifi_task(ciot_wifi_t self)
{
    return CIOT_ERR_NOT_SUPPORTED;
}

ciot_err_t ciot_wifi_send_bytes(ciot_iface_t *iface, uint8_t *bytes, int size)
{
    return CIOT_ERR_NOT_SUPPORTED;
}

static ciot_err_t ciot_wifi_set_cfg(ciot_wifi_t self, ciot_wifi_cfg_t *cfg)
{
    CIOT_LOGI(TAG, "Configuring");

    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(cfg);

    ciot_wifi_base_t *base = &self->base;
    ciot_tcp_base_t *tcp = (ciot_tcp_base_t *)&base->tcp;

    if (self->base.cfg.type != cfg->type)
    {
        return CIOT_ERR_INVALID_TYPE;
    }

    CIOT_ERR_RETURN(ciot_tcp_init_netif(base->tcp));

    memcpy(&base->cfg, cfg, sizeof(*cfg));

    if (cfg->has_tcp)
    {
        CIOT_LOGI(TAG, "Set tcp config");
        CIOT_ERR_RETURN(ciot_tcp_set_cfg(base->tcp, &cfg->tcp));
    }

    return CIOT_ERR_OK;
}

ciot_err_t ciot_wifi_scan(ciot_wifi_t self)
{
    CIOT_LOGI(TAG, "WIFI_EVENT_SCAN_DONE");
    ciot_wifi_base_t *base = &self->base;
    base->status.scan_state = CIOT_WIFI_SCAN_STATE_DONE;
    base->ap_list.count = 5; // Mock 5 access points found
    ciot_event_t *event = calloc(1, sizeof(ciot_event_t));
    event->type = CIOT_EVENT_TYPE_DONE;
    event->which_data = CIOT_EVENT_MSG_TAG;
    event->msg.id = base->iface.req_status.id;
    event->msg.has_iface = true;
    event->msg.iface = base->iface.info;
    event->msg.has_data = true;
    event->msg.data.which_type = CIOT_MSG_DATA_WIFI_TAG;
    event->msg.data.wifi.which_type = CIOT_WIFI_DATA_REQUEST_TAG;
    event->msg.data.wifi.request.which_type = CIOT_WIFI_REQ_SCAN_RESULT_TAG;
    event->msg.data.wifi.request.scan_result.count = base->ap_list.count;
    ciot_wifi_update_ap_list(self);
    ciot_iface_send_event(&base->iface, event);
    free(event);
    return CIOT_ERR_OK;
}

static ciot_err_t ciot_wifi_update_ap_list(ciot_wifi_t self)
{
    ciot_wifi_base_t *base = &self->base;
    if(base->ap_list.count > 0 && base->ap_list.items != NULL) {
        free(base->ap_list.items);
        base->ap_list.items = NULL;
    }
    base->ap_list.items = calloc(base->ap_list.count, sizeof(ciot_wifi_ap_info_t));
    for (size_t i = 0; i < base->ap_list.count; i++)
    {
        base->ap_list.items[i].rssi = -30 - (i * 10);
        snprintf(base->ap_list.items[i].ssid, sizeof(base->ap_list.items[i].ssid), "Mocked_AP_%d", (int)i);
        base->ap_list.items[i].authmode = 3; // WPA2
    }
    return CIOT_ERR_OK;
}

#endif // CIOT_CONFIG_FEATURE_WIFI == 1
