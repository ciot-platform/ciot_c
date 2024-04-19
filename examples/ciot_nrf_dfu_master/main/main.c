/**
 * @file main.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-04-19
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <stdio.h>

#include "ciot.h"
#include "ciot_uart.h"
#include "ciot_nrf_dfu.h"

#define UART_PORT_NUM 1
#define UART_BAUD_RATE 115200  

ciot_uart_cfg_t uart_cfg = {
    .num = UART_PORT_NUM,
    .baud_rate = UART_BAUD_RATE
};

ciot_nrf_dfu_cfg_t dfu_cfg = {
    .dfu.type = CIOT_DFU_TYPE_UART,
};

int app_main(int argc, char *argv[])
{
    ciot_uart_t uart = ciot_uart_new(CIOT_HANDLE);
    ciot_dfu_t dfu = ciot_nrf_dfu_new(&dfu_cfg, (ciot_iface_t*)uart);

    ciot_uart_start(uart, &uart_cfg);
    ciot_nrf_dfu_start(dfu, &dfu_cfg.dfu);

    while (true)
    {
        ciot_uart_task(uart);
        ciot_nrf_dfu_task(dfu);
    }

    return 0;
}
