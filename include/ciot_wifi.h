/**
 * @file ciot_wifi.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef __CIOT_WIFI__H__
#define __CIOT_WIFI__H__

#ifdef __cplusplus
extern "C" {
#endif

#include "ciot_types.h"
#include "ciot_tcp.h"
#include "ciot_iface.h"

typedef enum ciot_wifi_event
{
    CIOT_WIFI_EVENT_AP_STA_CONNECTED = CIOT_EVENT_TYPE_CUSTOM,
    CIOT_WIFI_EVENT_AP_STA_DISCONNECTED,
} ciot_wifi_event_t;

typedef struct ciot_wifi *ciot_wifi_t;

typedef struct ciot_wifi_ap_list
{
    ciot_wifi_ap_info_t *items;
    int count;
} ciot_wifi_ap_list_t;

typedef struct ciot_wifi_base
{
    ciot_iface_t iface;
    ciot_tcp_t tcp;
    ciot_wifi_cfg_t cfg;
    ciot_wifi_status_t status;
    ciot_wifi_info_t info;
    ciot_wifi_ap_list_t ap_list;
} ciot_wifi_base_t;

ciot_wifi_t ciot_wifi_new(ciot_wifi_type_t type);
ciot_err_t ciot_wifi_init(ciot_wifi_t self);
ciot_err_t ciot_wifi_start(ciot_wifi_t self, ciot_wifi_cfg_t *cfg);
ciot_err_t ciot_wifi_stop(ciot_wifi_t self);
ciot_err_t ciot_wifi_process_req(ciot_wifi_t self, ciot_wifi_req_t *req);
ciot_err_t ciot_wifi_get_cfg(ciot_wifi_t self, ciot_wifi_cfg_t *cfg);
ciot_err_t ciot_wifi_get_status(ciot_wifi_t self, ciot_wifi_status_t *status);
ciot_err_t ciot_wifi_get_info(ciot_wifi_t self, ciot_wifi_info_t *info);
ciot_err_t ciot_wifi_get_mac(ciot_wifi_t self, uint8_t mac[6]);
ciot_err_t ciot_wifi_toggle(ciot_wifi_t self);
ciot_err_t ciot_wifi_scan(ciot_wifi_t self);
ciot_err_t ciot_wifi_get_scanned_ap(ciot_wifi_t self, int id, ciot_wifi_ap_info_t *ap_info);
ciot_err_t ciot_wifi_get_scanned_aps(ciot_wifi_t self, ciot_wifi_ap_list_t *ap_list);

#ifdef __cplusplus
}
#endif

#endif  //!__CIOT_WIFI__H__
