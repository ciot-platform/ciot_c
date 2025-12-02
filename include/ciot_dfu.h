/**
 * @file ciot_dfu.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef __CIOT_dfu__H__
#define __CIOT_dfu__H__

#ifdef __cplusplus
extern "C" {
#endif

#include "ciot_types.h"
#include "ciot_iface.h"

typedef struct ciot_dfu
{
    ciot_iface_t iface;
    ciot_dfu_cfg_t cfg;
    ciot_dfu_status_t status;
    // ciot_dfu_req_t req;
    // ciot_dfu_data_t data;
} ciot_dfu_t;

#ifdef __cplusplus
}
#endif

#endif  //!__CIOT_dfu__H__
