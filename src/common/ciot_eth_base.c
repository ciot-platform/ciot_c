/**
 * @file ciot_eth_base.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */

 
#include "ciot_config.h"

#if CIOT_CONFIG_FEATURE_ETH == 1

#include <stdlib.h>
#include <string.h>
#include "ciot_eth.h"
#include "ciot_tcp.h"
#include "ciot_config.h"

// static const char *TAG = "ciot_tcp";

static ciot_err_t ciot_eth_process_data(ciot_iface_t *iface, ciot_msg_data_t *data);
static ciot_err_t ciot_eth_get_data(ciot_iface_t *iface, ciot_msg_data_t *msg);
static ciot_err_t ciot_eth_send_data(ciot_iface_t *iface, uint8_t *data, int size);

ciot_err_t ciot_eth_init(ciot_eth_t self)
{
    ciot_eth_base_t *base = (ciot_eth_base_t*)self;
    ciot_tcp_base_t *tcp = (ciot_tcp_base_t *)base->tcp;

    base->iface.ptr = self;
    base->iface.process_data = ciot_eth_process_data;
    base->iface.get_data = ciot_eth_get_data;
    base->iface.send_data = ciot_eth_send_data;
    base->iface.info.type = CIOT_IFACE_TYPE_ETH;

    tcp->cfg = &base->cfg;
    tcp->status = &base->status;
    tcp->info = &base->info;

    return CIOT_ERR_OK;
}

static ciot_err_t ciot_eth_process_data(ciot_iface_t *iface, ciot_msg_data_t *data)
{
    CIOT_ERR_TYPE_CHECK(data->which_type, CIOT_MSG_DATA_ETH_TAG);

    ciot_eth_t self = iface->ptr;
    ciot_tcp_data_t *eth = &data->eth;

    switch (eth->which_type)
    {
    case CIOT_TCP_DATA_STOP_TAG:
        return ciot_eth_stop(self);
    case CIOT_TCP_DATA_CONFIG_TAG:
        return ciot_eth_start(self, &eth->config);
    case CIOT_TCP_DATA_REQUEST_TAG:
        return ciot_eth_process_req(self, &eth->request);
    default:
        return CIOT_ERR_INVALID_TYPE;
    }

    return CIOT_ERR_INVALID_TYPE;
}

static ciot_err_t ciot_eth_get_data(ciot_iface_t *iface, ciot_msg_data_t *data)
{
    CIOT_ERR_TYPE_CHECK(data->which_type, CIOT_MSG_DATA_GET_DATA_TAG);

    ciot_eth_base_t *self = iface->ptr;
    ciot_data_type_t data_type = data->get_data.type;
    data->which_type = CIOT_MSG_DATA_ETH_TAG;

    switch (data_type)
    {
    case CIOT_DATA_TYPE_CONFIG:
        data->eth.which_type = CIOT_TCP_DATA_CONFIG_TAG;
        data->eth.config = self->cfg;
        break;
    case CIOT_DATA_TYPE_STATUS:
        data->eth.which_type = CIOT_TCP_DATA_STATUS_TAG;
        data->eth.status = self->status;
        break;
    default:
        return CIOT_ERR_NOT_FOUND;
    }

    return CIOT_ERR_OK;
}

static ciot_err_t ciot_eth_send_data(ciot_iface_t *iface, uint8_t *data, int size)
{
    return CIOT_ERR_NOT_IMPLEMENTED;
}

ciot_err_t ciot_eth_process_req(ciot_eth_t self, ciot_tcp_req_t *req)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(req);
    return CIOT_ERR_NOT_IMPLEMENTED;
}

ciot_err_t ciot_eth_get_cfg(ciot_eth_t self, ciot_tcp_cfg_t *cfg)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(cfg);
    ciot_eth_base_t *base = (ciot_eth_base_t*)self;
    return ciot_tcp_get_cfg(base->tcp, cfg);
}

ciot_err_t ciot_eth_get_status(ciot_eth_t self, ciot_tcp_status_t *status)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(status);
    ciot_eth_base_t *base = (ciot_eth_base_t*)self;
    return ciot_tcp_get_status(base->tcp, status);
}

ciot_err_t ciot_eth_get_info(ciot_eth_t self, ciot_tcp_info_t *info)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(info);
    ciot_eth_base_t *base = (ciot_eth_base_t*)self;
    return ciot_tcp_get_info(base->tcp, info);
}

ciot_err_t ciot_eth_get_mac(ciot_eth_t self, uint8_t mac[6])
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(mac);
    ciot_eth_base_t *base = (ciot_eth_base_t*)self;
    ciot_tcp_base_t *tcp = (ciot_tcp_base_t*)base->tcp;
    memcpy(mac, tcp->info->mac, 6);
    return CIOT_ERR_OK;
}

#endif // CIOT_CONFIG_FEATURE_ETH == 1