/**
 * @file main.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-12-01
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "main.h"
#include "ciot_storage_nvs.h"

device_t self;

ciot_msg_data_t sys_cfg = {};

ciot_msg_data_t http_server_1_cfg = {
    .which_type = CIOT_MSG_DATA_HTTP_SERVER_TAG,
    .http_server = {
        .which_type = CIOT_HTTP_SERVER_DATA_CONFIG_TAG,
        .config = {
            .address = "http://127.0.0.1",
            .route = "/v1/main",
            .port = 5050,
        },
    },
};

ciot_msg_data_t http_server_2_cfg = {
    .which_type = CIOT_MSG_DATA_HTTP_SERVER_TAG,
    .http_server = {
        .which_type = CIOT_HTTP_SERVER_DATA_CONFIG_TAG,
        .config = {
            .address = "http://127.0.0.1",
            .route = "/v1/proxy",
            .port = 5151,
        },
    },
};

ciot_msg_data_t http_client_1_cfg = {
    .which_type = CIOT_MSG_DATA_HTTP_CLIENT_TAG,
    .http_client = {
        .which_type = CIOT_HTTP_CLIENT_DATA_CONFIG_TAG,
        .config = {
            .url = "http://127.0.0.1:5050/v1/main",
            .method = CIOT_HTTP_METHOD_POST,
            .transport = CIOT_HTTP_TRANSPORT_TCP,
            .timeout = 5000,
        },
    },
};

ciot_msg_data_t http_client_2_cfg = {
    .which_type = CIOT_MSG_DATA_HTTP_CLIENT_TAG,
    .http_client = {
        .which_type = CIOT_HTTP_CLIENT_DATA_CONFIG_TAG,
        .config = {
            .url = "http://127.0.0.1:5151/v1/proxy",
            .method = CIOT_HTTP_METHOD_POST,
            .transport = CIOT_HTTP_TRANSPORT_TCP,
            .timeout = 5000,
        },
    },
};

static const char *TAG = "main";

static void device_start()
{
    // self.ifaces.storage = ciot_storage_nvs_new();

    self.ifaces.ciot = ciot_new();
    self.ifaces.list[DEVICE_IFACE_ID_CIOT] = (ciot_iface_t *)self.ifaces.ciot;
    self.ifaces.cfgs[DEVICE_IFACE_ID_CIOT] = NULL;

    self.ifaces.sys = ciot_sys_new(CIOT_HANDLE);
    self.ifaces.list[DEVICE_IFACE_ID_SYS] = (ciot_iface_t *)self.ifaces.sys;
    self.ifaces.cfgs[DEVICE_IFACE_ID_SYS] = NULL;

    self.ifaces.http_server_1 = ciot_http_server_new(CIOT_HANDLE);
    self.ifaces.list[DEVICE_IFACE_ID_HTTP_SERVER_1] = (ciot_iface_t *)self.ifaces.http_server_1;
    self.ifaces.cfgs[DEVICE_IFACE_ID_HTTP_SERVER_1] = &http_server_1_cfg;

    self.ifaces.http_server_2 = ciot_http_server_new(CIOT_HANDLE);
    self.ifaces.list[DEVICE_IFACE_ID_HTTP_SERVER_2] = (ciot_iface_t *)self.ifaces.http_server_2;
    self.ifaces.cfgs[DEVICE_IFACE_ID_HTTP_SERVER_2] = &http_server_2_cfg;

    self.ifaces.http_client_1 = ciot_http_client_new(CIOT_HANDLE);
    self.ifaces.list[DEVICE_IFACE_ID_HTTP_CLIENT] = (ciot_iface_t *)self.ifaces.http_client_1;
    self.ifaces.cfgs[DEVICE_IFACE_ID_HTTP_CLIENT] = &http_client_1_cfg;

    self.ifaces.http_client_2 = ciot_http_client_new(CIOT_HANDLE);
    self.ifaces.list[DEVICE_IFACE_ID_HTTP_CLIENT] = (ciot_iface_t *)self.ifaces.http_client_2;
    self.ifaces.cfgs[DEVICE_IFACE_ID_HTTP_CLIENT] = &http_client_2_cfg;

    self.ifaces.mqtt_client = ciot_mqtt_client_new(CIOT_HANDLE);
    self.ifaces.list[DEVICE_IFACE_ID_MQTT_CLIENT] = (ciot_iface_t *)self.ifaces.mqtt_client;
    self.ifaces.cfgs[DEVICE_IFACE_ID_MQTT_CLIENT] = NULL;

    ciot_cfg_t ciot_cfg = {
        .ifaces = {
            .list = self.ifaces.list,
            .cfgs = self.ifaces.cfgs,
            .count = DEVICE_IFACE_ID_COUNT,
        },
    };
    ciot_start(self.ifaces.ciot, &ciot_cfg);
}

static void device_task()
{
    ciot_task(self.ifaces.ciot);
    ciot_sys_task(self.ifaces.sys);
}

int main(void)
{
    device_start();

    while (true)
    {
        device_task();
    }

    return 0;
}
