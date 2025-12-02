/**
 * @file ciot_tcp_base.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "ciot_config.h"

#if CIOT_CONFIG_FEATURE_TCP == 1

#include <stdlib.h>
#include <string.h>
#include "ciot_tcp.h"

ciot_err_t ciot_tcp_init(ciot_tcp_t self)
{
    return CIOT_ERR_OK;
}

ciot_err_t ciot_tcp_get_cfg(ciot_tcp_t self, ciot_tcp_cfg_t *cfg)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(cfg);
    ciot_tcp_base_t *base = (ciot_tcp_base_t*)self;
    *cfg = *base->cfg;
    return CIOT_ERR_OK;
}

ciot_err_t ciot_tcp_set_cfg(ciot_tcp_t self, ciot_tcp_cfg_t *cfg)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(cfg);
    ciot_tcp_base_t *base = (ciot_tcp_base_t*)self;
    *base->cfg = *cfg;
    return CIOT_ERR_OK;
}

ciot_err_t ciot_tcp_get_status(ciot_tcp_t self, ciot_tcp_status_t *status)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(status);
    ciot_tcp_base_t *base = (ciot_tcp_base_t*)self;
    *status = *base->status;
    return CIOT_ERR_OK;
}

ciot_err_t ciot_tcp_get_info(ciot_tcp_t self, ciot_tcp_info_t *info)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(info);
    ciot_tcp_base_t *base = (ciot_tcp_base_t*)self;
    *info = *base->info;
    return CIOT_ERR_OK;
}

#endif // CIOT_CONFIG_FEATURE_TCP == 1