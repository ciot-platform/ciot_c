/**
 * @file ciot_http_client.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-12-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "ciot_config.h"

#if CIOT_CONFIG_FEATURE_HTTP_CLIENT == 1 && defined(CIOT_PLATFORM_DEFAULT)
 
#include "ciot_http_client.h"

struct ciot_http_client
{
    ciot_http_client_base_t base;
};

// static const char *TAG = "ciot_http_client";

ciot_http_client_t ciot_http_client_new(void *handle)
{
    ciot_http_client_t self = calloc(1, sizeof(struct ciot_http_client));
    ciot_http_client_init(self);
    return self;
}

ciot_err_t ciot_http_client_start(ciot_http_client_t self, ciot_http_client_cfg_t *cfg)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(cfg);
    return CIOT_ERR_NOT_IMPLEMENTED;
}

ciot_err_t ciot_http_client_stop(ciot_http_client_t self)
{
    CIOT_ERR_NULL_CHECK(self);
    return CIOT_ERR_NOT_IMPLEMENTED;
}

ciot_err_t ciot_http_client_send_bytes(ciot_http_client_t self, uint8_t *data, int size)
{
    return CIOT_ERR_NOT_IMPLEMENTED;
}

#endif // CIOT_CONFIG_FEATURE_HTTP_CLIENT == 1
