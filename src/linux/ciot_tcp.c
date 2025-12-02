/**
 * @file ciot_tcp.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "ciot_config.h"

#if CIOT_CONFIG_FEATURE_TCP == 1 && defined(CIOT_PLATFORM_LINUX)

#include <stdlib.h>
#include <string.h>
#include "ciot_tcp.h"
#include "ciot_types.h"

struct ciot_tcp_netif
{
    void *ptr;
};

struct ciot_tcp
{
    ciot_tcp_base_t base;
    struct ciot_tcp_netif netif;
};

static bool init = false;
static const char *TAG = "ciot_tcp";

ciot_tcp_t ciot_tcp_new(ciot_iface_t *iface, ciot_tcp_type_t type)
{
    ciot_tcp_t self = calloc(1, sizeof(struct ciot_tcp));
    ciot_tcp_init(self);
    self->base.type = type;
    self->base.iface = iface;
    return self;
}

ciot_err_t ciot_tcp_init_netif(ciot_tcp_t self)
{
    return CIOT_ERR_OK;;
}

ciot_err_t ciot_tcp_start(ciot_tcp_t self)
{
    return CIOT_ERR_OK;
}

ciot_err_t ciot_tcp_stop(ciot_tcp_t self)
{
    return CIOT_ERR_OK;
}

#endif // CIOT_CONFIG_FEATURE_TCP == 1
