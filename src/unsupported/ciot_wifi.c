/**
 * @file ciot_wifi.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-10-18
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "ciot_wifi.h"
#include <string.h>

#if CIOT_CONFIG_FEATURE_WIFI && (defined(CIOT_TARGET_WIN) || defined(CIOT_TARGET_LINUX))

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "ciot_tcp.h"

#ifndef CIOT_CONFIG_WIFI_AP_MAC
#define CIOT_CONFIG_WIFI_AP_MAC
#endif

#ifndef CIOT_CONFIG_WIFI_STA_MAC
#define CIOT_CONFIG_WIFI_STA_MAC
#endif

struct ciot_wifi
{
    ciot_iface_t iface;
    ciot_tcp_t tcp;
    ciot_wifi_cfg_t cfg;
    ciot_wifi_status_t status;
};

ciot_wifi_t ciot_wifi_new(void *handle)
{
    ciot_wifi_t self = calloc(1, sizeof(struct ciot_wifi));
    self->iface.base.ptr = self;
    self->iface.base.start = (ciot_iface_start_fn *)ciot_wifi_start;
    self->iface.base.stop = (ciot_iface_stop_fn *)ciot_wifi_stop;
    self->iface.base.process_req = (ciot_iface_process_req_fn *)ciot_wifi_process_req;
    self->iface.base.send_data = (ciot_iface_send_data_fn *)ciot_wifi_send_data;
    self->iface.base.cfg.ptr = &self->cfg;
    self->iface.base.cfg.size = sizeof(self->cfg);
    self->iface.base.status.ptr = &self->status;
    self->iface.base.status.size = sizeof(self->status);
    self->iface.info.type = CIOT_IFACE_TYPE_WIFI;
    return self;
}

ciot_wifi_t ciot_wifi_ap_new(void *handle)
{
    ciot_wifi_t self = ciot_wifi_new(handle);
    uint8_t mac_ptr[] = {CIOT_CONFIG_WIFI_AP_MAC};
    memcpy(self->status.tcp.info.mac, mac_ptr, 6);
    self->cfg.type = CIOT_WIFI_IF_AP;
    return self;
}

ciot_wifi_t ciot_wifi_sta_new(void *handle)
{
    ciot_wifi_t self = ciot_wifi_new(handle);
    uint8_t mac_ptr[] = {CIOT_CONFIG_WIFI_STA_MAC};
    memcpy(self->status.tcp.info.mac, mac_ptr, 6);
    self->cfg.type = CIOT_WIFI_IF_STA;
    return self;
}

ciot_err_t ciot_wifi_start(ciot_wifi_t self, ciot_wifi_cfg_t *cfg)
{
    return CIOT_ERR_NOT_SUPPORTED;
}

ciot_err_t ciot_wifi_stop(ciot_wifi_t self)
{
    return CIOT_ERR_NOT_SUPPORTED;
}

ciot_err_t ciot_wifi_process_req(ciot_wifi_t self, ciot_wifi_req_t *req)
{
    return CIOT_ERR_NOT_SUPPORTED;
}

ciot_err_t ciot_wifi_send_data(ciot_wifi_t self, uint8_t *data, int size)
{
    return CIOT_ERR_NOT_SUPPORTED;
}

ciot_err_t ciot_wifi_get_mac(ciot_wifi_t self, ciot_wifi_type_t type, uint8_t mac[6])
{
    switch (type)
    {
    case CIOT_WIFI_IF_AP:
    {
        uint8_t mac_ptr[] = {CIOT_CONFIG_WIFI_AP_MAC};
        memcpy(mac, mac_ptr, 6);
        break;
    }
    case CIOT_WIFI_IF_STA:
    {
        uint8_t mac_ptr[] = {CIOT_CONFIG_WIFI_STA_MAC};
        memcpy(mac, mac_ptr, 6);
        break;
    }
    default:
        break;
    }
    return CIOT_OK;
}

#endif
