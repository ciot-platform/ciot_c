/**
 * @file iot_device.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-04-14
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef __IOT_DEVICE__H__
#define __IOT_DEVICE__H__

#include "ciot.h"
#include "ciot_sys.h"
#include "ciot_wifi.h"
#include "ciot_mqttc.h"
#include "ciot_storage.h"
#include "ciot_uart.h"
#include "ciot_https.h"
#include "ciot_httpc.h"
#include "ciot_serializer.h"

typedef enum iot_device_iface
{
    IOT_DEVICE_IFACE_NONE = -1,
    IOT_DEVICE_IFACE_CIOT,
    IOT_DEVICE_IFACE_SYS,
    IOT_DEVICE_IFACE_WIFI_AP,
    IOT_DEVICE_IFACE_WIFI_STA,
    IOT_DEVICE_IFACE_MQTTC,
    IOT_DEVICE_IFACE_STORAGE,
    IOT_DEVICE_IFACE_UART,
    IOT_DEVICE_IFACE_HTTPS,
    IOT_DEVICE_IFACE_HTTPC,
    IOT_DEVICE_IFACE_COUNT,
} iot_device_iface_t;

typedef struct iot_device_ifaces
{
    ciot_t ciot;
    ciot_sys_t sys;
    ciot_wifi_t wifi_ap;
    ciot_wifi_t wifi_sta;
    ciot_mqttc_t mqttc;
    ciot_storage_t storage;
    ciot_uart_t uart;
    ciot_https_t https;
    ciot_httpc_t httpc;
    void *cfgs[IOT_DEVICE_IFACE_COUNT];
    ciot_iface_t *list[IOT_DEVICE_IFACE_COUNT];
} iot_device_ifaces_t;

struct iot_device
{
    iot_device_ifaces_t ifaces;
    ciot_serializer_t serializer;
};

typedef struct iot_device *ciot_device_t;

ciot_device_t ciot_device_new(void);
ciot_err_t ciot_device_start(ciot_device_t self);
ciot_err_t ciot_device_task(ciot_device_t self);

#endif  //!__IOT_DEVICE__H__
