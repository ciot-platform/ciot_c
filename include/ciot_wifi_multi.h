/**
 * @file ciot_wifi_multi.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2026-04-23
 *
 * @copyright Copyright (c) 2026
 *
 */

#ifndef __CIOT_WIFI_MULTI__H__
#define __CIOT_WIFI_MULTI__H__

#ifdef __cplusplus
extern "C" {
#endif

#include "ciot_types.h"
#include "ciot_iface.h"
#include "ciot_wifi.h"

#if CIOT_CONFIG_FEATURE_WIFI_MULTI == 1

#define CIOT_WIFI_MULTI_ACTIVE_INDEX_NONE UINT32_MAX

typedef enum ciot_wifi_multi_event
{
    CIOT_WIFI_MULTI_EVENT_SWITCHING = CIOT_WIFI_EVENT_AP_STA_DISCONNECTED + 1,
    CIOT_WIFI_MULTI_EVENT_ITEM_INVALIDATED,
    CIOT_WIFI_MULTI_EVENT_NO_VALID_NETWORK,
} ciot_wifi_multi_event_t;

typedef struct ciot_wifi_multi *ciot_wifi_multi_t;

typedef struct ciot_wifi_multi_base
{
    ciot_iface_t iface;
    ciot_wifi_t wifi;
    bool own_wifi;
    ciot_iface_event_handler_fn *wifi_event_handler;
    void *wifi_event_args;
    ciot_wifi_multi_cfg_t cfg;
    ciot_wifi_multi_status_t status;
    ciot_wifi_multi_info_t info;
} ciot_wifi_multi_base_t;

ciot_wifi_multi_t ciot_wifi_multi_new(ciot_wifi_t wifi_sta);
ciot_err_t ciot_wifi_multi_init(ciot_wifi_multi_t self);
ciot_err_t ciot_wifi_multi_start(ciot_wifi_multi_t self, ciot_wifi_multi_cfg_t *cfg);
ciot_err_t ciot_wifi_multi_stop(ciot_wifi_multi_t self);
ciot_err_t ciot_wifi_multi_task(ciot_wifi_multi_t self);
ciot_err_t ciot_wifi_multi_next(ciot_wifi_multi_t self);
ciot_err_t ciot_wifi_multi_mark_invalid(ciot_wifi_multi_t self, uint32_t index, ciot_err_t reason);
ciot_err_t ciot_wifi_multi_mark_active_invalid(ciot_wifi_multi_t self, ciot_err_t reason);
ciot_err_t ciot_wifi_multi_reset_invalid(ciot_wifi_multi_t self);
ciot_err_t ciot_wifi_multi_process_req(ciot_wifi_multi_t self, ciot_wifi_multi_req_t *req);
ciot_err_t ciot_wifi_multi_get_cfg(ciot_wifi_multi_t self, ciot_wifi_multi_cfg_t *cfg);
ciot_err_t ciot_wifi_multi_get_status(ciot_wifi_multi_t self, ciot_wifi_multi_status_t *status);
ciot_err_t ciot_wifi_multi_get_info(ciot_wifi_multi_t self, ciot_wifi_multi_info_t *info);

#endif // CIOT_CONFIG_FEATURE_WIFI_MULTI

#ifdef __cplusplus
}
#endif

#endif  //!__CIOT_WIFI_MULTI__H__