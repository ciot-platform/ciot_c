/**
 * @file ciot_ble.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef __CIOT_BLE__H__
#define __CIOT_BLE__H__

#ifdef __cplusplus
extern "C" {
#endif

#include "ciot_types.h"
#include "ciot_iface.h"
#include "ciot_ble_scn.h"
#include "ciot_ble_adv.h"

typedef enum ciot_ble_mac_type
{
    CIOT_BLE_MAC_TYPE_UNKNOWN,
    CIOT_BLE_MAC_TYPE_HARDWARE,
    CIOT_BLE_MAC_TYPE_SOFTWARE,
    CIOT_BLE_MAC_TYPE_REAL,
} ciot_ble_mac_type_t;

typedef struct ciot_ble *ciot_ble_t;

typedef struct ciot_ble_ifaces
{
    ciot_ble_scn_t scn;
    ciot_ble_adv_t adv;
} ciot_ble_ifaces_t;

typedef struct ciot_ble_base
{
    ciot_iface_t iface;
    ciot_ble_cfg_t cfg;
    ciot_ble_status_t status;
    ciot_ble_info_t info;
    // ciot_ble_req_t req;
    // ciot_ble_data_t data;
    ciot_ble_ifaces_t ifaces;
} ciot_ble_base_t;

ciot_ble_t ciot_ble_new(void *handle);
ciot_err_t ciot_ble_init(ciot_ble_t self);
ciot_err_t ciot_ble_start(ciot_ble_t self, ciot_ble_cfg_t *cfg);
ciot_err_t ciot_ble_stop(ciot_ble_t self);
ciot_err_t ciot_ble_process_req(ciot_ble_t self, ciot_ble_req_t *req);
ciot_err_t ciot_ble_get_cfg(ciot_ble_t self, ciot_ble_cfg_t *cfg);
ciot_err_t ciot_ble_get_status(ciot_ble_t self, ciot_ble_status_t *status);
ciot_err_t ciot_ble_get_info(ciot_ble_t self, ciot_ble_info_t *info);
ciot_err_t ciot_ble_task(ciot_ble_t self);
ciot_err_t ciot_ble_set_mac(ciot_ble_t self, uint8_t mac[6]);
ciot_err_t ciot_ble_get_mac(ciot_ble_t self, ciot_ble_mac_type_t type, uint8_t mac[6]);
ciot_err_t ciot_ble_set_ifaces(ciot_ble_t self, ciot_ble_ifaces_t *ifaces);
bool ciot_ble_mac_is_valid(uint8_t mac[6]);

#ifdef __cplusplus
}
#endif

#endif  //!__CIOT_BLE__H__
