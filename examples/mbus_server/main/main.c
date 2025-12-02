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

#define REGS_COUNT 32
#define SERVER_ADDR 1

device_t self;

ciot_msg_data_t sys_cfg = {};

ciot_msg_data_t uart_cfg = {
    .which_type = CIOT_MSG_DATA_UART_TAG,
    .uart = {
        .which_type = CIOT_UART_DATA_CONFIG_TAG,
        .config = {
            .baud_rate = 9600,
            .num = 9,
            .gpio.rx = 34,
            .gpio.tx = 32,
            .gpio.rts = 33,
            .gpio.cts = -1,
            .mode = 1,
            .read_timeout = 0,
            .write_timeout = 0,
        },
    },
};

ciot_msg_data_t mbus_server_cfg = {
    .which_type = CIOT_MSG_DATA_MBUS_SERVER_TAG,
    .mbus_server = {
        .which_type = CIOT_MBUS_SERVER_DATA_CONFIG_TAG,
        .config = {
            .which_type = CIOT_MBUS_SERVER_CFG_RTU_TAG,
            .rtu = {
                .server_id = 1,
            },
        },
    },
};

static const char *TAG = "main";

static ciot_err_t event_handler(ciot_iface_t *sender, ciot_event_t *event, void *args);

static void device_start()
{
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
        .coils.count = DEVICE_MBUS_COILS_COUNT,
        .regs.values = self.mbus_data.regs,
        .regs.count = DEVICE_MBUS_REGS_COUNT,
    };
    self.ifaces.mbus_server = ciot_mbus_server_new(CIOT_HANDLE, &mbus_data, (ciot_iface_t*)self.ifaces.uart);
    self.ifaces.list[DEVICE_IFACE_ID_MBUS_SERVER] = (ciot_iface_t*)self.ifaces.mbus_server;
    self.ifaces.cfgs[DEVICE_IFACE_ID_MBUS_SERVER] = &mbus_server_cfg;

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
}

static void device_update_data_task()
{
    self.mbus_data.regs[0] = time(NULL);
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
