/**
 * @file iot_device.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-04-14
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "ciot_err.h"

#include "ciot_device.h"
#include "proto_serializer.h"

static const char *TAG = "iot_device";

static ciot_sys_cfg_t sys_cfg = CIOT_CONFIG_SYS;
static ciot_storage_cfg_t storage_cfg = CIOT_CONFIG_STORAGE;
static ciot_wifi_cfg_t wifi_ap_cfg = CIOT_CONFIG_WIFI_AP;
static ciot_wifi_cfg_t wifi_sta_cfg = CIOT_CONFIG_WIFI_STA;
static ciot_mqttc_cfg_t mqttc_cfg = CIOT_CONFIG_MQTTC;
static ciot_uart_cfg_t uart_cfg = CIOT_CONFIG_UART;
static ciot_https_cfg_t https_cfg = CIOT_CONFIG_HTTPS;
static ciot_httpc_cfg_t httpc_cfg = CIOT_CONFIG_HTTPC;

ciot_device_t ciot_device_new(void)
{
    ciot_device_t self = calloc(1, sizeof(struct iot_device));
    return self;
}

ciot_err_t ciot_device_start(ciot_device_t self)
{
    CIOT_LOGI(TAG, "Application starting...");

    self->ifaces.ciot = ciot_new();
    self->ifaces.list[IOT_DEVICE_IFACE_CIOT] = (ciot_iface_t*)self->ifaces.ciot;

    self->ifaces.sys = ciot_sys_new(CIOT_HANDLE);
    self->ifaces.list[IOT_DEVICE_IFACE_SYS] = (ciot_iface_t*)self->ifaces.sys;
    self->ifaces.cfgs[IOT_DEVICE_IFACE_SYS] = &sys_cfg;

    self->ifaces.wifi_ap = ciot_wifi_new(CIOT_HANDLE);
    self->ifaces.list[IOT_DEVICE_IFACE_WIFI_AP] = (ciot_iface_t*)self->ifaces.wifi_ap;
    self->ifaces.cfgs[IOT_DEVICE_IFACE_WIFI_AP] = &wifi_ap_cfg;

    self->ifaces.wifi_sta = ciot_wifi_new(CIOT_HANDLE);
    self->ifaces.list[IOT_DEVICE_IFACE_WIFI_STA] = (ciot_iface_t*)self->ifaces.wifi_sta;
    self->ifaces.cfgs[IOT_DEVICE_IFACE_WIFI_STA] = &wifi_sta_cfg;

    self->ifaces.mqttc = ciot_mqttc_new(CIOT_HANDLE);
    self->ifaces.list[IOT_DEVICE_IFACE_MQTTC] = (ciot_iface_t*)self->ifaces.mqttc;
    self->ifaces.cfgs[IOT_DEVICE_IFACE_MQTTC] = &mqttc_cfg;

    self->ifaces.storage = ciot_storage_new(CIOT_HANDLE);
    self->ifaces.list[IOT_DEVICE_IFACE_STORAGE] = (ciot_iface_t*)self->ifaces.storage;
    self->ifaces.cfgs[IOT_DEVICE_IFACE_STORAGE] = &storage_cfg;

    self->ifaces.uart = ciot_uart_new(CIOT_HANDLE);
    self->ifaces.list[IOT_DEVICE_IFACE_UART] = (ciot_iface_t*)self->ifaces.uart;
    self->ifaces.cfgs[IOT_DEVICE_IFACE_UART] = &uart_cfg;

    self->ifaces.https = ciot_https_new(CIOT_HANDLE);
    self->ifaces.list[IOT_DEVICE_IFACE_HTTPS] = (ciot_iface_t*)self->ifaces.https;
    self->ifaces.cfgs[IOT_DEVICE_IFACE_HTTPS] = &https_cfg;

    self->ifaces.httpc = ciot_httpc_new(CIOT_HANDLE);
    self->ifaces.list[IOT_DEVICE_IFACE_HTTPC] = (ciot_iface_t*)self->ifaces.httpc;
    self->ifaces.cfgs[IOT_DEVICE_IFACE_HTTPC] = &httpc_cfg;

    self->serializer = proto_serializer_new();
    ciot_iface_set_serializer((ciot_iface_t*)self->ifaces.httpc, self->serializer);
    ciot_iface_set_serializer((ciot_iface_t*)self->ifaces.https, self->serializer);
    ciot_iface_set_serializer((ciot_iface_t*)self->ifaces.mqttc, self->serializer);

    ciot_cfg_t ciot_cfg = {
        .ifaces = self->ifaces.list,
        .cfgs = self->ifaces.cfgs,
        .count = IOT_DEVICE_IFACE_COUNT
    };
    return ciot_start(self->ifaces.ciot, &ciot_cfg);
}

ciot_err_t ciot_device_task(ciot_device_t self)
{
    CIOT_ERROR_RETURN(ciot_sys_task(self->ifaces.sys));
    CIOT_ERROR_RETURN(ciot_uart_task(self->ifaces.uart));
    return CIOT_OK;
}
