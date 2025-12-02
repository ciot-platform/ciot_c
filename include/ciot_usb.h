/**
 * @file ciot_usb.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef __CIOT_USB__H__
#define __CIOT_USB__H__

#ifdef __cplusplus
extern "C" {
#endif

#include "ciot_types.h"
#include "ciot_iface.h"

typedef struct ciot_usb *ciot_usb_t;

typedef struct ciot_usb_base
{
    ciot_iface_t iface;
    ciot_usb_cfg_t cfg;
    ciot_usb_status_t status;
    // ciot_usb_req_t req;
    // ciot_usb_data_t data;
} ciot_usb_base_t;

ciot_usb_t ciot_usb_new(void *handle);
ciot_err_t ciot_usb_init(ciot_usb_t self);
ciot_err_t ciot_usb_start(ciot_usb_t self, ciot_usb_cfg_t *cfg);
ciot_err_t ciot_usb_stop(ciot_usb_t self);
ciot_err_t ciot_usb_process_req(ciot_usb_t self, ciot_usb_req_t *req);
ciot_err_t ciot_usb_get_cfg(ciot_usb_t self, ciot_usb_cfg_t *cfg);
ciot_err_t ciot_usb_get_status(ciot_usb_t self, ciot_usb_status_t *status);
ciot_err_t ciot_usb_task(ciot_usb_t self);
ciot_err_t ciot_usb_send_bytes(ciot_usb_t self, uint8_t *bytes, int size);

#ifdef __cplusplus
}
#endif

#endif  //!__CIOT_USB__H__
