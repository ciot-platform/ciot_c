/**
 * @file msg_data.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-05-16
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include <stdio.h>

#include "msg_data_parser.h"
#include "uart_parser.h"
#include "http_client_parser.h"
#include "mqtt_client_parser.h"

ciot_err_t parse_msg_data_args(int argc, char **argv, ciot_msg_data_t *msg_data)
{
    return CIOT_ERR_NOT_IMPLEMENTED;
}

ciot_err_t print_msg_data(ciot_msg_data_t *msg_data)
{
    switch (msg_data->which_type)
    {
    case CIOT_MSG_DATA_UART_TAG:
        return print_uart_data(&msg_data->uart);
    case CIOT_MSG_DATA_HTTP_CLIENT_TAG:
        return print_http_client_data(&msg_data->http_client);
    case CIOT_MSG_DATA_MQTT_CLIENT_TAG:
        return print_mqtt_client_data(&msg_data->mqtt_client);
    default:
        printf("Unknown message type: %d\n", msg_data->which_type);
        return CIOT_ERR_INVALID_TYPE;
    }
    return CIOT_ERR_OK;
}

static ciot_err_t parse_common_args(int argc, char **argv, ciot_common_t *common_data)
{

}