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
#include "ciot_timer.h"

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
            .num = 1,
            .gpio.rx = 34,
            .gpio.tx = 32,
            .gpio.rts = 33,
            .gpio.cts = -1,
            .mode = 1,
            .read_timeout = 500,
            .write_timeout = 500,
        },
    },
};

ciot_msg_data_t mbus_client_cfg = {
    .which_type = CIOT_MSG_DATA_MBUS_CLIENT_TAG,
    .mbus_client = {
        .which_type = CIOT_MBUS_CLIENT_DATA_CONFIG_TAG,
        .config = {
            .which_type = CIOT_MBUS_CLIENT_CFG_RTU_TAG,
            .rtu.server_id = 1,
            .timeout = 500,
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

    self.ifaces.mbus_client = ciot_mbus_client_new(CIOT_HANDLE, (ciot_iface_t *)self.ifaces.uart);
    self.ifaces.list[DEVICE_IFACE_ID_MBUS_CLIENT] = (ciot_iface_t *)self.ifaces.mbus_client;
    self.ifaces.cfgs[DEVICE_IFACE_ID_MBUS_CLIENT] = &mbus_client_cfg;

    ciot_iface_set_event_handler(&self.ifaces.ciot->iface, event_handler, &self);
    ciot_cfg_t ciot_cfg = {
        .ifaces = {
            .list = self.ifaces.list,
            .cfgs = self.ifaces.cfgs,
            .count = DEVICE_IFACE_ID_COUNT,
        },
    };
    ciot_start(self.ifaces.ciot, &ciot_cfg);
}

static void mbus_read_data_task()
{
    if (ciot_timer_compare(&self.timer, 1) && self.uart_started)
    {
        uint16_t reg;
        ciot_err_t err = ciot_mbus_client_read_holding_registers(self.ifaces.mbus_client, 0, 1, &reg);
        if(err != CIOT_ERR_OK)
        {
            CIOT_LOGE(TAG, "Error reading register: %s", ciot_err_to_message(err));
        }
        CIOT_LOGI(TAG, "%d", reg);
    }
}

static void device_task()
{
    ciot_task(self.ifaces.ciot);
    ciot_sys_task(self.ifaces.sys);
    mbus_read_data_task();
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

static ciot_err_t event_handler(ciot_iface_t *sender, ciot_event_t *event, void *args)
{
    device_t *device = args;
    if(sender->info.id == DEVICE_IFACE_ID_UART && event->type == CIOT_EVENT_TYPE_STARTED)
    {
        device->uart_started = true;
    }
    return CIOT_ERR_OK;
}
