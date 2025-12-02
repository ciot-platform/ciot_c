/**
 * @file ciot_http_server.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-12-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef __CIOT_HTTP_SERVER__H__
#define __CIOT_HTTP_SERVER__H__

#ifdef __cplusplus
extern "C" {
#endif

#include "ciot_types.h"
#include "ciot_iface.h"

typedef struct ciot_http_server *ciot_http_server_t;

#ifndef CIOT_CONFIG_URL_SIZE
#define CIOT_CONFIG_URL_SIZE 48
#endif

typedef struct ciot_http_server_event_data
{
    char uri[CIOT_CONFIG_URL_SIZE];
    uint8_t body[CIOT_CONFIG_MSG_SIZE];
} ciot_http_server_event_data_t;

typedef struct ciot_http_server_homepage_cfg
{
    bool gz;
    const uint8_t *data;
    size_t size;
} ciot_http_server_homepage_cfg_t;

typedef struct ciot_http_server_base
{
    ciot_iface_t iface;
    ciot_http_server_cfg_t cfg;
    ciot_http_server_status_t status;
    ciot_http_server_homepage_cfg_t homepage;
} ciot_http_server_base_t;

ciot_http_server_t ciot_http_server_new(void *handle);
ciot_err_t ciot_http_server_init(ciot_http_server_t self);
ciot_err_t ciot_http_server_start(ciot_http_server_t self, ciot_http_server_cfg_t *cfg);
ciot_err_t ciot_http_server_stop(ciot_http_server_t self);
ciot_err_t ciot_http_server_process_req(ciot_http_server_t self, ciot_http_server_req_t *req);
ciot_err_t ciot_http_server_get_cfg(ciot_http_server_t self, ciot_http_server_cfg_t *cfg);
ciot_err_t ciot_http_server_get_status(ciot_http_server_t self, ciot_http_server_status_t *status);
ciot_err_t ciot_http_server_send_bytes(ciot_http_server_t self, uint8_t *data, int size);
ciot_err_t ciot_http_server_set_homepage(ciot_http_server_t self, ciot_http_server_homepage_cfg_t *homepage);

#ifdef __cplusplus
}
#endif

#endif  //!__CIOT_HTTP_SERVER__H__
