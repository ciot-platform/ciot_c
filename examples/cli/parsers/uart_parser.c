/**
 * @file uart_parser.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-05-16
 *
 * @copyright Copyright (c) 2025
 *
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "common.h"
#include "uart_parser.h"

ciot_err_t parse_uart_args(int argc, char **argv, ciot_uart_data_t *uart_data)
{
    memset(uart_data, 0, sizeof(ciot_uart_data_t));

    uart_data->which_type = CIOT_UART_DATA_CONFIG_TAG;
    for (int i = 2; i < argc; i++)
    {
        const char *val;
        if ((val = get_arg_value(argv[i], "--baud-rate=")) != NULL)
        {
            uart_data->config.baud_rate = atoi(val);
        }
        else if ((val = get_arg_value(argv[i], "--num=")) != NULL)
        {
            uart_data->config.num = atoi(val);
        }
        else if ((val = get_arg_value(argv[i], "--gpio-rx=")) != NULL)
        {
            uart_data->config.gpio.rx = atoi(val);
        }
        else if ((val = get_arg_value(argv[i], "--gpio-tx=")) != NULL)
        {
            uart_data->config.gpio.tx = atoi(val);
        }
        else if ((val = get_arg_value(argv[i], "--gpio-rts=")) != NULL)
        {
            uart_data->config.gpio.rts = atoi(val);
        }
        else if ((val = get_arg_value(argv[i], "--gpio-cts=")) != NULL)
        {
            uart_data->config.gpio.cts = atoi(val);
        }
        else if ((val = get_arg_value(argv[i], "--parity=")) != NULL)
        {
            uart_data->config.parity = atoi(val);
        }
        else if ((val = get_arg_value(argv[i], "--flow-control=")) != NULL)
        {
            uart_data->config.flow_control = parse_bool(val);
        }
        else if ((val = get_arg_value(argv[i], "--dtr=")) != NULL)
        {
            uart_data->config.dtr = parse_bool(val);
        }
        else if ((val = get_arg_value(argv[i], "--mode=")) != NULL)
        {
            uart_data->config.mode = atoi(val);
        }
    }
    return CIOT_ERR_OK;
}

ciot_err_t print_uart_data(ciot_uart_data_t *uart_data)
{
    switch (uart_data->which_type)
    {
    case CIOT_UART_DATA_STOP_TAG:
        printf("UART Stop\n");
        break;
    case CIOT_UART_DATA_CONFIG_TAG:
        printf("UART Config\n");
        printf("--baud-rate=%u\n", uart_data->config.baud_rate);
        printf("--num=%u\n", uart_data->config.num);
        printf("--gpio-rx=%u\n", uart_data->config.gpio.rx);
        printf("--gpio-tx=%u\n", uart_data->config.gpio.tx);
        printf("--gpio-rts=%u\n", uart_data->config.gpio.rts);
        printf("--gpio-cts=%u\n", uart_data->config.gpio.cts);
        printf("--parity=%u\n", uart_data->config.parity);
        printf("--flow-control=%s\n", uart_data->config.flow_control ? "true" : "false");
        printf("--dtr=%s\n", uart_data->config.dtr ? "true" : "false");
        printf("--mode=%u\n", uart_data->config.mode);
        break;
    case CIOT_UART_DATA_STATUS_TAG:
        printf("UART Status\n");
        printf("State: %u\n", uart_data->status.state);
        printf("Error: %u\n", uart_data->status.error);
        break;
    case CIOT_UART_DATA_REQUEST_TAG:
        printf("UART Request\n");
        if (uart_data->request.which_type == CIOT_UART_REQ_SEND_DATA_TAG)
        {
            printf("Send Data: %s\n", uart_data->request.send_data.bytes);
        }
        break;
    default:
        printf("Unknown UART data type\n");
        return CIOT_ERR_INVALID_TYPE;
    }
    return CIOT_ERR_OK;
}