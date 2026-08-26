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

#include <time.h>

#include "main.h"
#include "ciot_storage_nvs.h"

device_t self;

ciot_msg_data_t sys_cfg = {};

ciot_msg_data_t uart_cfg = TARGET_RS485_CFG;

ciot_msg_data_t mbus_server_cfg = TARGET_MBUS_SERVER_CFG;
ciot_msg_data_t mbus_server_tcp_cfg = TARGET_MBUS_SERVER_TCP_CFG;

#ifdef TARGET_ESP32
ciot_msg_data_t wifi_sta_cfg = TARGET_WIFI_STA_CFG;
#endif

static const char *TAG = "main";

// static ciot_err_t event_handler(ciot_iface_t *sender, ciot_event_t *event, void *args);

static void device_start()
{
    self.ifaces.storage = ciot_storage_new();

    self.ifaces.ciot = ciot_new();
    self.ifaces.list[DEVICE_IFACE_ID_CIOT] = (ciot_iface_t *)self.ifaces.ciot;
    self.ifaces.cfgs[DEVICE_IFACE_ID_CIOT] = NULL;

    self.ifaces.sys = ciot_sys_new(CIOT_HANDLE);
    self.ifaces.list[DEVICE_IFACE_ID_SYS] = (ciot_iface_t *)self.ifaces.sys;
    self.ifaces.cfgs[DEVICE_IFACE_ID_SYS] = NULL;

    self.ifaces.uart = ciot_uart_new(CIOT_HANDLE);
    self.ifaces.list[DEVICE_IFACE_ID_UART] = (ciot_iface_t *)self.ifaces.uart;
    self.ifaces.cfgs[DEVICE_IFACE_ID_UART] = &uart_cfg;

    ciot_mbus_data_t mbus_data = {
        .coils.values = self.mbus_data.coils,
        .coils.count = TARGET_MBUS_COILS_COUNT,
        .regs.values = self.mbus_data.regs,
        .regs.count = TARGET_MBUS_REGS_COUNT,
    };
    self.ifaces.mbus_server = ciot_mbus_server_new(CIOT_HANDLE, &mbus_data, (ciot_iface_t*)self.ifaces.uart);
    self.ifaces.list[DEVICE_IFACE_ID_MBUS_SERVER] = (ciot_iface_t*)self.ifaces.mbus_server;
    self.ifaces.cfgs[DEVICE_IFACE_ID_MBUS_SERVER] = &mbus_server_cfg;

#ifdef TARGET_ESP32
    // WiFi needs to be brought up before the Modbus TCP socket below can be reached
    self.ifaces.wifi_sta = ciot_wifi_new(CIOT_WIFI_TYPE_STA);
    self.ifaces.list[DEVICE_IFACE_ID_WIFI_STA] = (ciot_iface_t *)self.ifaces.wifi_sta;
    self.ifaces.cfgs[DEVICE_IFACE_ID_WIFI_STA] = &wifi_sta_cfg;
#endif

#if defined(TARGET_ESP32) || defined(CIOT_PLATFORM_MONGOOSE)
    // Modbus TCP server: ciot_socket picks its backend (ESP32/lwIP or Mongoose) at build time
    self.ifaces.mbus_socket = ciot_socket_new(CIOT_HANDLE);
    self.ifaces.list[DEVICE_IFACE_ID_MBUS_SOCKET] = (ciot_iface_t *)self.ifaces.mbus_socket;
    self.ifaces.cfgs[DEVICE_IFACE_ID_MBUS_SOCKET] = NULL; // started internally by mbus_server_tcp, not through generic config

    self.ifaces.mbus_server_tcp = ciot_mbus_server_new(CIOT_HANDLE, &mbus_data, (ciot_iface_t *)self.ifaces.mbus_socket);
    self.ifaces.list[DEVICE_IFACE_ID_MBUS_SERVER_TCP] = (ciot_iface_t *)self.ifaces.mbus_server_tcp;
    self.ifaces.cfgs[DEVICE_IFACE_ID_MBUS_SERVER_TCP] = &mbus_server_tcp_cfg;
#endif

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
    ciot_mbus_server_task(self.ifaces.mbus_server);
#if defined(TARGET_ESP32) || defined(CIOT_PLATFORM_MONGOOSE)
    ciot_mbus_server_task(self.ifaces.mbus_server_tcp);
#endif
}

static void device_update_data_task()
{
    for (size_t i = 0; i < TARGET_MBUS_REGS_COUNT; i++)
    {
        self.mbus_data.regs[i] = time(NULL);
    }
}

int main(void)
{
    device_start();

    while (true)
    {
        device_task();
        device_update_data_task();
    }

    return 0;
}
