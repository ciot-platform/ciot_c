/**
 * @file ciot_tcp.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef __CIOT_TCP__H__
#define __CIOT_TCP__H__

#ifdef __cplusplus
extern "C" {
#endif

#include "ciot_types.h"
#include "ciot_iface.h"

typedef struct ciot_tcp *ciot_tcp_t;

typedef enum ciot_tcp_type
{
    CIOT_TCP_TYPE_UNKNOWN,
    CIOT_TCP_TYPE_WIFI_STA,
    CIOT_TCP_TYPE_WIFI_AP,
    CIOT_TCP_TYPE_ETHERNET,
} ciot_tcp_type_t;

typedef struct ciot_tcp_base
{
    ciot_iface_t *iface;
    ciot_tcp_cfg_t *cfg;
    ciot_tcp_status_t *status;
    ciot_tcp_info_t *info;
    ciot_tcp_type_t type;
} ciot_tcp_base_t;

ciot_tcp_t ciot_tcp_new(ciot_iface_t *iface, ciot_tcp_type_t type);
ciot_err_t ciot_tcp_init(ciot_tcp_t self);
ciot_err_t ciot_tcp_init_netif(ciot_tcp_t self);
ciot_err_t ciot_tcp_start(ciot_tcp_t self);
ciot_err_t ciot_tcp_stop(ciot_tcp_t self);
ciot_err_t ciot_tcp_process_req(ciot_tcp_t self, ciot_tcp_req_t *req);
ciot_err_t ciot_tcp_set_cfg(ciot_tcp_t self, ciot_tcp_cfg_t *cfg);
ciot_err_t ciot_tcp_get_cfg(ciot_tcp_t self, ciot_tcp_cfg_t *cfg);
ciot_err_t ciot_tcp_get_status(ciot_tcp_t self, ciot_tcp_status_t *status);
ciot_err_t ciot_tcp_get_info(ciot_tcp_t self, ciot_tcp_info_t *info);
ciot_err_t ciot_tcp_get_ip(ciot_tcp_t self, uint8_t *ip);
void *ciot_tcp_get_netif(ciot_tcp_t self);

#ifdef __cplusplus
}
#endif

#endif  //!__CIOT_TCP__H__
