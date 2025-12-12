/**
 * @file main.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-12-16
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef __MAIN__H__
#define __MAIN__H__

#include "ciot.h"
#include "ciot_sys.h"
#include "ciot_http_server.h"
#include "ciot_http_client.h"
#include "ciot_mqtt_client.h"

#ifdef IDF_VER
#define main app_main
#endif

typedef enum device_iface_id
{
    DEVICE_IFACE_ID_NONE = -1,
    DEVICE_IFACE_ID_CIOT,
    DEVICE_IFACE_ID_SYS,
    DEVICE_IFACE_ID_HTTP_SERVER_1,
    DEVICE_IFACE_ID_HTTP_SERVER_2,
    DEVICE_IFACE_ID_HTTP_CLIENT,
    DEVICE_IFACE_ID_MQTT_CLIENT,
    DEVICE_IFACE_ID_COUNT
} device_iface_id_t;

typedef struct device_ifaces
{
    ciot_t ciot;
    ciot_sys_t sys;
    ciot_http_server_t http_server_1;
    ciot_http_server_t http_server_2;
    ciot_http_client_t http_client_1;
    ciot_http_client_t http_client_2;
    ciot_mqtt_client_t mqtt_client;
    ciot_iface_t *list[DEVICE_IFACE_ID_COUNT];
    ciot_msg_data_t *cfgs[DEVICE_IFACE_ID_COUNT];
} device_ifaces_t;

typedef struct device
{
    device_ifaces_t ifaces;
} device_t;

#endif  //!__MAIN__H__