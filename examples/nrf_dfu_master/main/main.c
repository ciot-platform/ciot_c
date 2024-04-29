/**
 * @file main.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-04-22
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "ciot.h"
#include "ciot_uart.h"
#include "ciot_nrf_dfu.h"

#define UART_BAUD_RATE 115200
#define UART_PORT 10
#define UART_DTR true
#define UART_BRIDGE_MODE true

#define DFU_TYPE CIOT_DFU_TYPE_USB
#define DFU_MAX_PACKAGE_LEN CIOT_NRF_DFU_MAX_DFU_PKT_LEN_UART
#define DFU_INIT_PACKET_FILE "nrf52840_xxaa.dat"
#define DFU_APP_IMAGE_FILE "nrf52840_xxaa.bin"

static const char *TAG = "main";

typedef struct ndm
{
    ciot_uart_t uart;
    ciot_dfu_t dfu;
} ndm_t;

static ciot_err_t uart_event_handler(ciot_iface_t *iface, ciot_iface_event_t *event, void *args)
{
    ndm_t *self = (ndm_t *)args;

    if (event->type == CIOT_IFACE_EVENT_STARTED)
    {
        CIOT_LOGI(TAG, "Uart interface started");
        ciot_nrf_dfu_send_firmware(self->dfu);
    }

    if (event->type == CIOT_IFACE_EVENT_ERROR)
    {
        CIOT_LOGI(TAG, "Error received: 0x%x", event->data->msg.data.uart.status.error);
        exit(0);
    }
}

static ciot_err_t dfu_event_handler(ciot_iface_t *iface, ciot_iface_event_t *event, void *args)
{
    ndm_t *self = (ndm_t *)args;

    if (event->type == CIOT_DFU_EVENT_STATE_CHANGED)
    {
        if(event->data->msg.data.dfu.status.state == CIOT_DFU_STATE_ERROR ||
           event->data->msg.data.dfu.status.state == CIOT_DFU_STATE_COMPLETED)
        {
            CIOT_LOGI(TAG, "Application end");
            exit(0);
        }
    }
}

int app_main(void)
{
    CIOT_LOGI(TAG, "Application start");

    ndm_t self;

    ciot_uart_cfg_t uart_cfg = {
        .bridge_mode = UART_BRIDGE_MODE,
        .baud_rate = UART_BAUD_RATE,
        .dtr = UART_DTR,
        .num = UART_PORT};
    self.uart = ciot_uart_new(CIOT_HANDLE);

    ciot_nrf_dfu_cfg_t dfu_cfg = {
        .dfu.type = DFU_TYPE,
        .max_package_len = DFU_MAX_PACKAGE_LEN,
        .iface = (ciot_iface_t*)self.uart};
    CIOT_ERROR_RETURN(ciot_nrf_dfu_read_file(&dfu_cfg.init_packet, DFU_INIT_PACKET_FILE));
    CIOT_ERROR_RETURN(ciot_nrf_dfu_read_file(&dfu_cfg.app_image, DFU_APP_IMAGE_FILE));
    self.dfu = ciot_nrf_dfu_new(&dfu_cfg);
    ciot_nrf_dfu_start(self.dfu, &dfu_cfg.dfu);

    ciot_iface_register_event((ciot_iface_t *)self.uart, uart_event_handler, &self);
    ciot_iface_register_event((ciot_iface_t *)self.dfu, dfu_event_handler, &self);

    ciot_uart_start(self.uart, &uart_cfg);

    while (true)
    {
        ciot_uart_task(self.uart);
        ciot_nrf_dfu_task(self.dfu);
    }
}
