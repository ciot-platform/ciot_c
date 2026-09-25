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

typedef ciot_err_t (ciot_http_server_custom_api_handler_fn)(ciot_http_server_t self, const char *uri, size_t uri_len, char *method, uint8_t *data, size_t size, void *args);

#ifndef CIOT_HTTP_SERVER_MAX_RESPONSE_HEADERS
#define CIOT_HTTP_SERVER_MAX_RESPONSE_HEADERS 4
#endif

typedef struct ciot_http_server_request ciot_http_server_request_t;

typedef bool (ciot_http_server_get_header_fn)(const ciot_http_server_request_t *request, const char *name, char *value, size_t value_size);

struct ciot_http_server_request
{
    const char *method;
    size_t method_len;
    const char *uri;
    size_t uri_len;
    char local_address[48];
    char remote_address[48];
    ciot_http_server_get_header_fn *get_header;
    void *context;
};

typedef struct ciot_http_server_header
{
    const char *name;
    const char *value;
} ciot_http_server_header_t;

typedef struct ciot_http_server_response
{
    uint16_t status_code;
    const char *content_type;
    const uint8_t *body;
    size_t body_size;
    ciot_http_server_header_t headers[CIOT_HTTP_SERVER_MAX_RESPONSE_HEADERS];
    size_t headers_count;
} ciot_http_server_response_t;

typedef bool (ciot_http_server_request_interceptor_fn)(ciot_http_server_t self, const ciot_http_server_request_t *request, ciot_http_server_response_t *response, void *args);

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

typedef struct ciot_http_server_custom_api
{
    bool enabled;
    const char *uri;
    ciot_http_server_custom_api_handler_fn *handler;
    void *args;
} ciot_http_server_custom_api_t;

typedef ciot_err_t (ciot_http_server_upload_handler_fn)(void *args, uint8_t *data, size_t size);

/**
 * @brief A single raw-body upload route (e.g. "POST /data.bin").
 *
 * Unlike ciot_http_server_custom_api_t and the generic message-channel route,
 * this bypasses the fixed-size CIOT_CONFIG_MSG_SIZE buffers and ciot_event_t's
 * raw field, so it can carry payloads up to max_size bytes.
 *
 * This layer only enforces the size limit (max_size); it does not inspect or
 * validate the body's content in any way. Content validation, if the use
 * case needs it, is the responsibility of whoever sends the request and/or
 * of the registered handler - keep that in mind before trusting the payload
 * on a resource-constrained target.
 */
typedef struct ciot_http_server_upload_api
{
    bool enabled;
    const char *uri;
    const char *method;
    size_t max_size;
    ciot_http_server_upload_handler_fn *handler;
    void *args;
} ciot_http_server_upload_api_t;

typedef struct ciot_http_server_base
{
    ciot_iface_t iface;
    ciot_http_server_cfg_t cfg;
    ciot_http_server_status_t status;
    ciot_http_server_homepage_cfg_t homepage;
    ciot_http_server_custom_api_t custom_api;
    ciot_http_server_upload_api_t upload_api;
    ciot_http_server_request_interceptor_fn *request_interceptor;
    void *request_interceptor_args;
} ciot_http_server_base_t;

ciot_http_server_t ciot_http_server_new(void *handle);
ciot_err_t ciot_http_server_init(ciot_http_server_t self);
ciot_err_t ciot_http_server_start(ciot_http_server_t self, ciot_http_server_cfg_t *cfg);
ciot_err_t ciot_http_server_stop(ciot_http_server_t self);
ciot_err_t ciot_http_server_process_req(ciot_http_server_t self, ciot_http_server_req_t *req);
ciot_err_t ciot_http_server_get_cfg(ciot_http_server_t self, ciot_http_server_cfg_t *cfg);
ciot_err_t ciot_http_server_get_status(ciot_http_server_t self, ciot_http_server_status_t *status);
ciot_err_t ciot_http_server_send_bytes(ciot_http_server_t self, uint8_t *data, int size);
ciot_err_t ciot_http_server_send_response(ciot_http_server_t self, const ciot_http_server_response_t *response);
ciot_err_t ciot_http_server_set_homepage(ciot_http_server_t self, ciot_http_server_homepage_cfg_t *homepage);
ciot_err_t ciot_http_server_set_custom_api(ciot_http_server_t self, ciot_http_server_custom_api_t *custom_api);
ciot_err_t ciot_http_server_set_upload_api(ciot_http_server_t self, ciot_http_server_upload_api_t *upload_api);
ciot_err_t ciot_http_server_set_request_interceptor(ciot_http_server_t self, ciot_http_server_request_interceptor_fn *interceptor, void *args);

#ifdef __cplusplus
}
#endif

#endif  //!__CIOT_HTTP_SERVER__H__
