/**
 * @file ciot_sys_data.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-10-09
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __CIOT_SYS_DATA__H__
#define __CIOT_SYS_DATA__H__

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>

typedef enum __attribute__((packed))
{
    CIOT_SYS_REQ_UNKNONW,
    CIOT_SYS_REQ_RESTART,
    CIOT_SYS_REQ_INIT_DFU,
} ciot_sys_req_type_t;

typedef enum __attribute__((packed))
{
    CIOT_SYS_HW_UNKNOWN,
    CIOT_SYS_HW_ESP8266,
    CIOT_SYS_HW_ARDUINO,
    CIOT_SYS_HW_ESP32,
    CIOT_SYS_HW_NRF51,
    CIOT_SYS_HW_NRF52,
    CIOT_SYS_HW_LINUX,
    CIOT_SYS_HW_WIN32
} ciot_sys_hw_t;

typedef struct __attribute__((packed))
{
    uint16_t storage : 1;
    uint16_t system : 1;
    uint16_t uart : 1;
    uint16_t usb: 1;
    uint16_t ethernet : 1;
    uint16_t wifi : 1;
    uint16_t ble_scn : 1;
    uint16_t gpio : 1;
    uint16_t reserved : 8;
} ciot_sys_hw_features_t;

typedef struct __attribute__((packed))
{
    uint16_t ntp : 1;
    uint16_t ota: 1;
    uint16_t dfu: 1;
    uint16_t http_client : 1;
    uint16_t http_server : 1;
    uint16_t mqtt_client : 1;
    uint16_t timer : 1;
    uint16_t serializer : 1;
    uint16_t reserved : 8;
} ciot_sys_sw_features_t;

typedef struct __attribute__((packed))
{
    ciot_sys_hw_features_t hw;
    ciot_sys_sw_features_t sw;
} ciot_sys_features_t;

typedef struct __attribute__((packed))
{
    char hw_name[16];
    uint8_t app_ver[3];
    ciot_sys_hw_t hardware;
    ciot_sys_features_t features;
} ciot_sys_info_t;

typedef struct __attribute__((packed))
{
    uint8_t rst_reason;
    uint8_t rst_count;
    uint32_t free_memory;
    uint32_t lifetime;
    ciot_sys_info_t info;
} ciot_sys_status_t;

typedef struct __attribute__((packed))
{

} ciot_sys_cfg_t;

typedef union __attribute__((packed))
{

} ciot_sys_req_data_u;

typedef struct __attribute__((packed))
{
    ciot_sys_req_type_t type;
    ciot_sys_req_data_u data;
} ciot_sys_req_t;

typedef union __attribute__((packed))
{
    ciot_sys_cfg_t config;
    ciot_sys_status_t status;
    ciot_sys_req_t request;
} ciot_sys_data_u;

#ifdef __cplusplus
}
#endif

#endif  //!__CIOT_SYS_DATA__H__