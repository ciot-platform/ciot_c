/**
 * @file ciot_eth.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-25
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "ciot_config.h"

#if CIOT_CONFIG_FEATURE_BLE_ETH == 1 && defined(CIOT_PLATFORM_DEFAULT)
 
#include <stdlib.h>
#include "ciot_eth.h"
#include "ciot_tcp.h"

struct ciot_eth
{
    ciot_eth_base_t base;
};

// static const char *TAG = "ciot_eth";

ciot_eth_t ciot_eth_new(void *handle)
{
    ciot_eth_t self = calloc(1, sizeof(struct ciot_eth));
    ciot_eth_base_t *base = &self->base;
    base->tcp = ciot_tcp_new(&base->iface, CIOT_TCP_TYPE_ETHERNET);
    ciot_eth_init(self);
    return self;
}

ciot_err_t ciot_eth_start(ciot_eth_t self, ciot_tcp_cfg_t *cfg)
{
    return CIOT_ERR_NOT_IMPLEMENTED;
}

ciot_err_t ciot_eth_stop(ciot_eth_t self)
{
    return CIOT_ERR_NOT_IMPLEMENTED;
}

#endif // CIOT_CONFIG_FEATURE_BLE_ETH == 1
