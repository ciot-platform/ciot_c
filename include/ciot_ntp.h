/**
 * @file ciot_ntp.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef __CIOT_NTP__H__
#define __CIOT_NTP__H__

#ifdef __cplusplus
extern "C" {
#endif

#include "ciot_types.h"
#include "ciot_iface.h"

#ifndef CIOT_CONFIG_NTP_SERVER_URL_LEN
#define CIOT_CONFIG_NTP_SERVER_URL_LEN 64
#endif

#ifndef CIOT_CONFIG_NTP_TIMEZONE_LEN
#define CIOT_CONFIG_NTP_TIMEZONE_LEN 16
#endif

typedef struct ciot_ntp *ciot_ntp_t;

typedef struct ciot_ntp_base
{
    ciot_iface_t iface;
    ciot_ntp_cfg_t cfg;
    ciot_ntp_status_t status;
} ciot_ntp_base_t;

ciot_ntp_t ciot_ntp_new(void *handle);
ciot_err_t ciot_ntp_init(ciot_ntp_t self);
ciot_err_t ciot_ntp_start(ciot_ntp_t self, ciot_ntp_cfg_t *cfg);
ciot_err_t ciot_ntp_stop(ciot_ntp_t self);
ciot_err_t ciot_ntp_process_req(ciot_ntp_t self, ciot_ntp_req_t *req);
ciot_err_t ciot_ntp_get_cfg(ciot_ntp_t self, ciot_ntp_cfg_t *cfg);
ciot_err_t ciot_ntp_set_cfg(ciot_ntp_t self, ciot_ntp_cfg_t *cfg);
ciot_err_t ciot_ntp_get_status(ciot_ntp_t self, ciot_ntp_status_t *status);
// ciot_err_t ciot_ntp_get_info(ciot_ntp_t self, ciot_ntp_info_t *info);

#ifdef __cplusplus
}
#endif

#endif  //!__CIOT_NTP__H__
