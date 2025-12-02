/**
 * @file ciot_mbus_server.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-12-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "ciot_config.h"

#if CIOT_CONFIG_FEATURE_MBUS_SERVER == 1 && defined(CIOT_PLATFORM_DEFAULT)

struct ciot_mbus_server
{
    ciot_mbus_server_base_t base;
};

// static const char *TAG = "ciot_mbus_server";

ciot_mbus_server_t ciot_mbus_server_new(void *handle)
{
    ciot_mbus_server_t self = calloc(1, sizeof(struct ciot_mbus_server));
    ciot_mbus_server_init(self);
    return self;
}

ciot_err_t ciot_mbus_server_start(ciot_mbus_server_t self, ciot_mbus_server_cfg_t *cfg)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(cfg);
    return CIOT_ERR_NOT_IMPLEMENTED;
}

ciot_err_t ciot_mbus_server_stop(ciot_mbus_server_t self)
{
    CIOT_ERR_NULL_CHECK(self);
    return CIOT_ERR_NOT_IMPLEMENTED;
}

ciot_err_t ciot_mbus_server_send_bytes(ciot_mbus_server_t self, uint8_t *data, int size)
{
    return CIOT_ERR_NOT_IMPLEMENTED;
}

#endif // CIOT_CONFIG_FEATURE_MBUS_SERVER == 1
