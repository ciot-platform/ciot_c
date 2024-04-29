/**
 * @file ciot_msg_data.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-10-09
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __CIOT_MSG_DATA__H__
#define __CIOT_MSG_DATA__H__

#ifdef __cplusplus
extern "C" {
#endif

#include "ciot_err.h"
#include "types/ciot_types.h"
#include "types/ciot_storage_types.h"
#include "types/ciot_sys_types.h"
#include "types/ciot_uart_types.h"
#include "types/ciot_usb_types.h"
#include "types/ciot_tcp_types.h"
#include "types/ciot_tcp_types.h"
#include "types/ciot_wifi_types.h"
#include "types/ciot_ble_types.h"
#include "types/ciot_ble_scn_types.h"
#include "types/ciot_ntp_types.h"
#include "types/ciot_dfu_types.h"
#include "types/ciot_ota_types.h"
#include "types/ciot_https_types.h"
#include "types/ciot_httpc_types.h"
#include "types/ciot_mqttc_types.h"
#include "types/ciot_bridge_types.h"

#define CIOT_MSG_HEADER_SIZE (sizeof(ciot_msg_header_t))
#define CIOT_MSG_GET_SIZE(data) (CIOT_MSG_HEADER_SIZE + sizeof(data))
#define CIOT_MSG_MAX_SIZE sizeof(ciot_msg_t);

typedef enum __attribute__((packed))
{
    CIOT_MSG_TYPE_UNKNOWN,
    CIOT_MSG_TYPE_START,
    CIOT_MSG_TYPE_STOP,
    CIOT_MSG_TYPE_GET_CONFIG,
    CIOT_MSG_TYPE_GET_STATUS,
    CIOT_MSG_TYPE_REQUEST,
    CIOT_MSG_TYPE_ERROR,
    CIOT_MSG_TYPE_EVENT,
    CIOT_MSG_TYPE_CUSTOM,
} ciot_msg_type_t;

typedef enum __attribute__((packed))
{
    CIOT_IFACE_TYPE_UNKNOWN,
    CIOT_IFACE_TYPE_CIOT,
    CIOT_IFACE_TYPE_STORAGE,
    CIOT_IFACE_TYPE_SYSTEM,
    CIOT_IFACE_TYPE_UART,
    CIOT_IFACE_TYPE_USB,
    CIOT_IFACE_TYPE_TCP,
    CIOT_IFACE_TYPE_ETH,
    CIOT_IFACE_TYPE_WIFI,
    CIOT_IFACE_TYPE_BLE,
    CIOT_IFACE_TYPE_BLE_SCN,
    CIOT_IFACE_TYPE_GPIO,
    CIOT_IFACE_TYPE_NTP=127,
    CIOT_IFACE_TYPE_OTA,
    CIOT_IFACE_TYPE_DFU,
    CIOT_IFACE_TYPE_HTTP_CLIENT,
    CIOT_IFACE_TYPE_HTTP_SERVER,
    CIOT_IFACE_TYPE_MQTT,
    CIOT_IFACE_TYPE_CUSTOM,
    CIOT_IFACE_TYPE_BRIDGE,
} ciot_msg_iface_type_t;

typedef struct __attribute__((packed))
{
    uint8_t id;
    ciot_msg_iface_type_t type;
} ciot_msg_iface_info_t;

typedef struct __attribute__((packed))
{
    ciot_msg_type_t msg_type;
    uint32_t code;
} ciot_msg_error_t;

typedef union __attribute__((packed))
{
    ciot_data_u ciot;
    ciot_storage_data_u storage;
    ciot_sys_data_u system;
    ciot_uart_data_u uart;
    ciot_usb_data_u usb;
    ciot_tcp_data_u tcp;
    ciot_tcp_data_u eth;
    ciot_wifi_data_u wifi;
    ciot_ble_data_u ble;
    ciot_ble_scn_data_u ble_scn;
    ciot_ntp_data_u ntp;
    ciot_ota_data_u ota;
    ciot_dfu_data_u dfu;
    ciot_https_data_u https;
    ciot_httpc_data_u httpc;
    ciot_mqttc_data_u mqtt;
    ciot_bridge_data_u bridge;
    ciot_msg_error_t error;
    uint8_t payload[256];
} ciot_msg_data_u;

typedef struct __attribute__((packed))
{
    uint8_t id;
    ciot_msg_type_t type;
    ciot_msg_iface_info_t iface;
} ciot_msg_header_t;

typedef struct __attribute__((packed))
{
    uint8_t id;
    ciot_msg_type_t type;
    ciot_msg_iface_info_t iface;
    ciot_msg_data_u data;
} ciot_msg_t;

const char *ciot_msg_type_to_str(ciot_msg_t *msg);

#ifdef __cplusplus
}
#endif

#endif  //!__CIOT_MSG_DATA__H__