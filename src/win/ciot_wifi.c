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

#if CIOT_CONFIG_FEATURE_WIFI == 1

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

ciot_wifi_t ciot_wifi_new(ciot_wifi_type_t type)
{
    ciot_wifi_t self = calloc(1, sizeof(struct ciot_wifi));
    ciot_wifi_base_t *base = &self->base;

    if (base->cfg.type == CIOT_WIFI_TYPE_AP)
    {
        CIOT_LOGI(TAG, "Creating wifi ap");
        base->tcp = ciot_tcp_new(&base->iface, CIOT_TCP_TYPE_WIFI_AP);
    }

    if (base->cfg.type == CIOT_WIFI_TYPE_STA)
    {
        CIOT_LOGI(TAG, "Creating wifi sta");
        base->tcp = ciot_tcp_new(&base->iface, CIOT_TCP_TYPE_WIFI_STA);
    }

    ciot_wifi_init(self);
    base->cfg.type = type;

    if (base->cfg.type == CIOT_WIFI_TYPE_AP)
    {
        CIOT_LOGI(TAG, "Creating wifi ap");
        base->tcp = ciot_tcp_new(&base->iface, CIOT_TCP_TYPE_WIFI_AP);
    }

    if (base->cfg.type == CIOT_WIFI_TYPE_STA)
    {
        CIOT_LOGI(TAG, "Creating wifi sta");
        base->tcp = ciot_tcp_new(&base->iface, CIOT_TCP_TYPE_WIFI_STA);
    }

    return self;
}

ciot_err_t ciot_wifi_start(ciot_wifi_t self, ciot_wifi_cfg_t *cfg)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(cfg);

    ciot_wifi_base_t *base = &self->base;
    ciot_tcp_base_t *tcp = (ciot_tcp_base_t *)&base->tcp;

    CIOT_ERR_RETURN(ciot_wifi_set_cfg(self, cfg));

    return CIOT_ERR_NOT_SUPPORTED;
}

ciot_err_t ciot_wifi_stop(ciot_wifi_t self)
{
    return CIOT_ERR_NOT_SUPPORTED;
}

ciot_err_t ciot_wifi_req_scan(ciot_wifi_t self)
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

    // strcpy(base->ssid, cfg->ssid);
    // strcpy(base->password, cfg->password);

    if (cfg->has_tcp)
    {
        CIOT_LOGI(TAG, "Set tcp config");
        CIOT_ERR_RETURN(ciot_tcp_set_cfg(base->tcp, &cfg->tcp));
    }

    return CIOT_ERR_OK;
}

#endif // CIOT_CONFIG_FEATURE_WIFI == 1
