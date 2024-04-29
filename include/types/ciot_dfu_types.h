/**
 * @file ciot_dfu_types.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-10-10
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef __CIOT_DFU_TYPES__H__
#define __CIOT_DFU_TYPES__H__

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>

typedef enum __attribute__((packed))
{
    CIOT_DFU_STATE_ERROR=-1,
    CIOT_DFU_STATE_IDLE,
    CIOT_DFU_STATE_IN_PROGRESS,
    CIOT_DFU_STATE_COMPLETED,
} ciot_dfu_state_t;

typedef enum __attribute__((packed))
{
    CIOT_DFU_REQ_UNKNOWN,
    CIOT_DFU_REQ_SEND_FIRMWARE,
} ciot_dfu_req_type_t;

typedef enum __attribute__((packed))
{
    CIOT_DFU_TYPE_UNKNOWN,
    CIOT_DFU_TYPE_UART,
    CIOT_DFU_TYPE_USB,
    CIOT_DFU_TYPE_BLE
} ciot_dfu_type_t;

typedef struct
{
    ciot_dfu_type_t type;
} ciot_dfu_cfg_t;

typedef struct __attribute__((packed))
{
    ciot_dfu_state_t state;
    int code;
    int error;
    uint32_t image_size;
    uint32_t image_read;
} ciot_dfu_status_t;

typedef union __attribute__((packed))
{
    
} ciot_dfu_req_data_u;

typedef struct __attribute__((packed))
{
    ciot_dfu_req_type_t type;
    ciot_dfu_req_data_u data;
} ciot_dfu_req_t;

typedef union __attribute__((packed))
{
    ciot_dfu_cfg_t config;
    ciot_dfu_status_t status;
    ciot_dfu_req_t request;
} ciot_dfu_data_u;

#ifdef __cplusplus
}
#endif

#endif //!__CIOT_DFU_TYPES__H__