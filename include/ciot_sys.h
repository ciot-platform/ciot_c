/**
 * @file ciot_sys.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-12-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef __CIOT_SYS__H__
#define __CIOT_SYS__H__

#ifdef __cplusplus
extern "C" {
#endif

#include "ciot_types.h"
#include "ciot_iface.h"

#define CIOT_SYS_EVT_BIT_POOLING 0x00000001

typedef struct ciot_sys *ciot_sys_t;

typedef struct ciot_sys_base
{
    ciot_iface_t iface;
    ciot_sys_cfg_t cfg;
    ciot_sys_status_t status;
    ciot_sys_info_t info;
} ciot_sys_base_t;

ciot_sys_t ciot_sys_new(void *handle);
ciot_err_t ciot_sys_init(ciot_sys_t self);
ciot_err_t ciot_sys_start(ciot_sys_t self, ciot_sys_cfg_t *cfg);
ciot_err_t ciot_sys_stop(ciot_sys_t self);
ciot_err_t ciot_sys_process_req(ciot_sys_t self, ciot_sys_req_t *req);
ciot_err_t ciot_sys_get_cfg(ciot_sys_t self, ciot_sys_cfg_t *cfg);
ciot_err_t ciot_sys_get_status(ciot_sys_t self, ciot_sys_status_t *status);
ciot_err_t ciot_sys_get_info(ciot_sys_t self, ciot_sys_info_t *info);
ciot_err_t ciot_sys_task(ciot_sys_t self);
ciot_err_t ciot_sys_set_event_bits(ciot_sys_t self, int event_bits);
ciot_err_t ciot_sys_sleep(long ms);
ciot_err_t ciot_sys_restart(void);
ciot_err_t ciot_sys_init_dfu(void);

#ifdef __cplusplus
}
#endif

#endif  //!__CIOT_SYS__H__