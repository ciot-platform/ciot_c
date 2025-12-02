/**
 * @file create_parser.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-05-16
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "create_parser.h"
#include "msg_data_parser.h"
#include "uart_parser.h"
#include "http_client_parser.h"
#include "mqtt_client_parser.h"

#include "ciot_err.h"
#include "ciot_iface.h"
#include "ciot_storage_fs.h"

ciot_err_t parse_create_args(int argc, char **argv)
{
    ciot_iface_type_t iface_type = CIOT_IFACE_TYPE_UNDEFINED;
    uint16_t iface_id = 0;
    ciot_msg_data_t msg_data = {0};

    for (int i = 2; i < argc; i++) {
        const char *val;
        if ((val = get_arg_value(argv[i], "--iface-id=")) != NULL) {
            iface_id = atoi(val);
        }
        if ((val = get_arg_value(argv[i], "--iface-type=")) != NULL) {
            iface_type = parse_iface_type(val);
        } 
    }

    ciot_err_t err = CIOT_ERR_OK;
    switch (iface_type) {
        case CIOT_IFACE_TYPE_UART:
            msg_data.which_type = CIOT_MSG_DATA_UART_TAG;
            err = parse_uart_args(argc, argv, &msg_data.uart);
            break;
        case CIOT_IFACE_TYPE_HTTP_CLIENT:
            msg_data.which_type = CIOT_MSG_DATA_HTTP_CLIENT_TAG;
            err = parse_http_client_args(argc, argv, &msg_data.http_client);
            break;
        case CIOT_IFACE_TYPE_MQTT_CLIENT:
            msg_data.which_type = CIOT_MSG_DATA_MQTT_CLIENT_TAG;
            err = parse_mqtt_client_args(argc, argv, &msg_data.mqtt_client);
            break;
        default:
            fprintf(stderr, "Error: invalid interface type.\n");
            return CIOT_ERR_INVALID_ARG;
    }

    if(err != CIOT_ERR_OK) {
        fprintf(stderr, "Error parsing interface arguments: %s\n", ciot_err_to_message(err));
        return err;
    }

    ciot_storage_t storage = ciot_storage_fs_new();
    char file_path[16];
    sprintf(file_path, "%d.dat", iface_id);
    err = ciot_storage_save_data(storage, file_path, &msg_data);

    if(err == CIOT_ERR_OK) {
        printf("Data saved successfully to %s:\n", file_path);
        print_msg_data(&msg_data);
    } else {
        fprintf(stderr, "Error saving data: %s\n", ciot_err_to_message(err));
        return err;
    }

    return CIOT_ERR_OK;
}
