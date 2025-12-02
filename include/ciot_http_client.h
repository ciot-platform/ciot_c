/**
 * @file ciot_http_client.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-12-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef __CIOT_HTTP_CLIENT__H__
#define __CIOT_HTTP_CLIENT__H__

#ifdef __cplusplus
extern "C" {
#endif

#include "ciot_types.h"
#include "ciot_iface.h"

typedef struct ciot_http_client *ciot_http_client_t;

#ifndef CIOT_CONFIG_URL_SIZE
#define CIOT_CONFIG_URL_SIZE 48
#endif

typedef struct ciot_http_client_base
{
    ciot_iface_t iface;
    ciot_http_client_cfg_t cfg;
    ciot_http_client_status_t status;
    // ciot_http_client_info_t info;
} ciot_http_client_base_t;

ciot_http_client_t ciot_http_client_new(void *handle);
ciot_err_t ciot_http_client_init(ciot_http_client_t self);
ciot_err_t ciot_http_client_start(ciot_http_client_t self, ciot_http_client_cfg_t *cfg);
ciot_err_t ciot_http_client_stop(ciot_http_client_t self);
ciot_err_t ciot_http_client_process_req(ciot_http_client_t self, ciot_http_client_req_t *req);
ciot_err_t ciot_http_client_get_cfg(ciot_http_client_t self, ciot_http_client_cfg_t *cfg);
ciot_err_t ciot_http_client_get_status(ciot_http_client_t self, ciot_http_client_status_t *status);
ciot_err_t ciot_http_client_send_bytes(ciot_http_client_t self, uint8_t *data, int size);

#ifdef __cplusplus
}
#endif

#endif  //!__CIOT_HTTP_CLIENT__H__
