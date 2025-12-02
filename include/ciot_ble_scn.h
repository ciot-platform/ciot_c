/**
 * @file ciot_ble_scn.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef __CIOT_BLE_SCN__H__
#define __CIOT_BLE_SCN__H__

#ifdef __cplusplus
extern "C" {
#endif

#include "ciot_types.h"
#include "ciot_iface.h"

typedef struct ciot_ble_scn *ciot_ble_scn_t;
typedef bool (ciot_ble_scn_filter_fn)(ciot_ble_scn_t self, ciot_ble_scn_adv_t *adv, void *args);

#ifdef CIOT_CONFIG_BLE_SCN_ADV_FIFO_SIZE

typedef struct ciot_ble_scn_adv_fifo
{
    ciot_ble_scn_adv_t list[CIOT_CONFIG_BLE_SCN_ADV_FIFO_SIZE];
    int wp;
    int rp;
} ciot_ble_scn_adv_fifo_t;
#endif

typedef struct ciot_ble_scn_filter
{
    ciot_ble_scn_filter_fn *handler;
    void *args;
} ciot_ble_scn_filter_t;

typedef struct ciot_ble_scn_base
{
    ciot_iface_t iface;
    ciot_ble_scn_cfg_t cfg;
    ciot_ble_scn_status_t status;
    ciot_ble_scn_adv_t recv;
    // ciot_ble_scn_req_t req;
    // ciot_ble_scn_data_t data;
    // ciot_ble_scn_adv_info_t recv_info;
    ciot_ble_scn_filter_t filter;
#ifdef CIOT_CONFIG_BLE_SCN_ADV_FIFO_SIZE
    ciot_ble_scn_adv_fifo_t adv_fifo;
#endif
} ciot_ble_scn_base_t;

ciot_ble_scn_t ciot_ble_scn_new(void *handle);
ciot_err_t ciot_ble_scn_init(ciot_ble_scn_t self);
ciot_err_t ciot_ble_scn_start(ciot_ble_scn_t self, ciot_ble_scn_cfg_t *cfg);
ciot_err_t ciot_ble_scn_stop(ciot_ble_scn_t self);
ciot_err_t ciot_ble_scn_process_req(ciot_ble_scn_t self, ciot_ble_scn_req_t *req);
ciot_err_t ciot_ble_scn_get_cfg(ciot_ble_scn_t self, ciot_ble_scn_cfg_t *cfg);
ciot_err_t ciot_ble_scn_get_status(ciot_ble_scn_t self, ciot_ble_scn_status_t *status);
ciot_err_t ciot_ble_scn_task(ciot_ble_scn_t self);
ciot_err_t ciot_ble_scn_base_task(ciot_ble_scn_t self);
void ciot_ble_scn_handle_adv_report(ciot_ble_scn_t self, ciot_ble_scn_adv_t *adv);
ciot_err_t ciot_ble_scn_handle_event(ciot_ble_scn_t self, void *event, void *event_args);
ciot_err_t ciot_ble_scn_set_filter(ciot_ble_scn_t self, ciot_ble_scn_filter_fn *filter, void *args);

#ifdef __cplusplus
}
#endif

#endif  //!__CIOT_BLE_SCN__H__
