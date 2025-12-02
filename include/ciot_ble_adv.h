/**
 * @file ciot_ble_adv.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef __CIOT_BLE_ADV__H__
#define __CIOT_BLE_ADV__H__

#ifdef __cplusplus
extern "C" {
#endif

#include "ciot_types.h"
#include "ciot_iface.h"

typedef struct ciot_ble_adv *ciot_ble_adv_t;

typedef struct ciot_ble_adv_base
{
    ciot_iface_t iface;
    ciot_ble_adv_cfg_t cfg;
    ciot_ble_adv_status_t status;
    // ciot_ble_adv_req_t req;
    // ciot_ble_adv_data_t data;
} ciot_ble_adv_base_t;

ciot_ble_adv_t ciot_ble_adv_new(void *handle);
ciot_err_t ciot_ble_adv_init(ciot_ble_adv_t self);
ciot_err_t ciot_ble_adv_start(ciot_ble_adv_t self, ciot_ble_adv_cfg_t *cfg);
ciot_err_t ciot_ble_adv_stop(ciot_ble_adv_t self);
ciot_err_t ciot_ble_adv_process_req(ciot_ble_adv_t self, ciot_ble_adv_req_t *req);
ciot_err_t ciot_ble_adv_get_cfg(ciot_ble_adv_t self, ciot_ble_adv_cfg_t *cfg);
ciot_err_t ciot_ble_adv_get_status(ciot_ble_adv_t self, ciot_ble_adv_status_t *status);
ciot_err_t ciot_ble_adv_handle_event(ciot_ble_adv_t self, void *event, void *event_args);
ciot_err_t ciot_ble_adv_send_bytes(ciot_ble_adv_t self, uint8_t *data, int size);

#ifdef __cplusplus
}
#endif

#endif  //!__CIOT_BLE_SCN__H__
