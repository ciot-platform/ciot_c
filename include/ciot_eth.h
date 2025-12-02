/**
 * @file ciot_eth.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-25
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef __CIOT_ETH__H__
#define __CIOT_ETH__H__

#include "ciot_tcp.h"

typedef struct ciot_eth *ciot_eth_t;

typedef struct ciot_eth_base
{
    ciot_iface_t iface;
    ciot_tcp_t tcp;
    ciot_tcp_cfg_t cfg;
    ciot_tcp_status_t status;
    ciot_tcp_info_t info;
} ciot_eth_base_t;

ciot_eth_t ciot_eth_new(void *handle);
ciot_err_t ciot_eth_init(ciot_eth_t self);
ciot_err_t ciot_eth_start(ciot_eth_t self, ciot_tcp_cfg_t *cfg);
ciot_err_t ciot_eth_stop(ciot_eth_t self);
ciot_err_t ciot_eth_process_req(ciot_eth_t self, ciot_tcp_req_t *req);
ciot_err_t ciot_eth_get_cfg(ciot_eth_t self, ciot_tcp_cfg_t *cfg);
ciot_err_t ciot_eth_get_status(ciot_eth_t self, ciot_tcp_status_t *status);
ciot_err_t ciot_eth_get_info(ciot_eth_t self, ciot_tcp_info_t *info);
ciot_err_t ciot_eth_get_mac(ciot_eth_t self, uint8_t mac[6]);

#endif  //!__CIOT_ETH__H__