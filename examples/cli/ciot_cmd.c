/**
 * @file ciot_cmd.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-05-16
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "ciot_cmd.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ciot.h"
#include "ciot_timer.h"
#include "ciot_storage_fs.h"
#include "ciot_uart.h"
#include "ciot_http_client.h"
#include "ciot_mqtt_client.h"

#include "parsers/common.h"
#include "parsers/create_parser.h"
#include "parsers/get_parser.h"
#include "parsers/uart_parser.h"
#include "parsers/http_client_parser.h"
#include "parsers/mqtt_client_parser.h"
#include "parsers/ota_parser.h"

static ciot_err_t create_iface(uint16_t id, ciot_cmd_context_t *ctx);
static ciot_err_t iface_event_handler(ciot_iface_t *sender, ciot_event_t *event, void *args);

ciot_err_t parse_ciot_cmd(int argc, char **argv, ciot_cmd_context_t *ctx) {
    if (argc < 3) {
        fprintf(stderr, "Usage: ciot <interface> --sender=... --iface-id=... [parameters]\n");
        return -1;
    }

    const char *iface_name = argv[1];
    // const char *sender_str = NULL;
    // const char *iface_id_str = NULL;

    ctx->sender = -1;
    ctx->msg.iface.id = -1;

    for (int i = 2; i < argc; ++i) {
        const char *val;
        if ((val = get_arg_value(argv[i], "--sender=")) != NULL) {
            ctx->sender = atoi(val);
        } else if ((val = get_arg_value(argv[i], "--iface-id=")) != NULL) {
            ctx->msg.iface.id = atoi(val);
        } else if ((val = get_arg_value(argv[i], "--get-data=")) != NULL) {
            ctx->msg.data.which_type = CIOT_MSG_DATA_GET_DATA_TAG;
            ctx->msg.data.get_data.type = parse_data_type(val);
        }
    }

    ctx->msg.has_iface = true;
    ctx->msg.error = CIOT_ERR_OK;
    ctx->msg.has_data = true;

    if (strcmp(iface_name, "create") == 0) {
        return parse_create_args(argc, argv);
    }

    if (strcmp(iface_name, "get") == 0) {
        return parse_get_args(argc, argv);
    }

    if (ctx->sender == -1) {
        fprintf(stderr, "Error: --sender argument is required.\n");
        return CIOT_ERR_INVALID_ARG;
    }

    if (ctx->msg.iface.id == -1) {
        fprintf(stderr, "Error: --iface-id argument is required.\n");
        return CIOT_ERR_INVALID_ARG;
    }

    ciot_err_t err = create_iface(ctx->sender, ctx);
    if (err != CIOT_ERR_OK) {
        fprintf(stderr, "Error: failed to create interface '%s'.\n", iface_name);
        return err;
    }

    if(strcmp(iface_name, "ciot") == 0) {
        ctx->msg.iface.type = CIOT_IFACE_TYPE_CIOT;
        ctx->msg.data.which_type = CIOT_MSG_DATA_CIOT_TAG;
        // ctx->err = parse_ciot_args(argc, argv, &ctx->msg.data.ciot);
    }

    if(strcmp(iface_name, "uart") == 0) {
        ctx->msg.iface.type = CIOT_IFACE_TYPE_UART;
        ctx->msg.data.which_type = CIOT_MSG_DATA_UART_TAG;
        ctx->err = parse_uart_args(argc, argv, &ctx->msg.data.uart);
    }

    if(strcmp(iface_name, "http_client") == 0) {
        ctx->msg.iface.type = CIOT_IFACE_TYPE_HTTP_CLIENT;
        ctx->msg.data.which_type = CIOT_MSG_DATA_HTTP_CLIENT_TAG;
        ctx->err = parse_http_client_args(argc, argv, &ctx->msg.data.http_client);
    }

    if(strcmp(iface_name, "mqtt_client") == 0) {
        ctx->msg.iface.type = CIOT_IFACE_TYPE_MQTT_CLIENT;
        ctx->msg.data.which_type = CIOT_MSG_DATA_MQTT_CLIENT_TAG;
        ctx->err = parse_mqtt_client_args(argc, argv, &ctx->msg.data.mqtt_client);
    }

    if (strcmp(iface_name, "ota") == 0) {
        ctx->msg.iface.type = CIOT_IFACE_TYPE_OTA;
        ctx->msg.data.which_type = CIOT_MSG_DATA_OTA_TAG;
        ctx->err = parse_ota_args(argc, argv, &ctx->msg.data.ota);
    }

    if(ctx->msg.iface.type == CIOT_IFACE_TYPE_UNDEFINED) {
        fprintf(stderr, "Error: Unknown interface type '%s'.\n", iface_name);
        return CIOT_ERR_INVALID_TYPE;
    }

    if(ctx->err == CIOT_ERR_OK)
    {
        ciot_iface_set_event_handler(ctx->iface, iface_event_handler, ctx);
        ciot_iface_send_msg(ctx->iface->ptr, &ctx->msg);
        time_t t = ciot_timer_now();
        while (ciot_timer_now() > t + 10 || ctx->completed)
        {
            ciot_task(NULL);
            if(ctx->iface->info.type == CIOT_IFACE_TYPE_UART)
            {
                ciot_uart_task(ctx->iface->ptr);
            }
        }
        if(ctx->completed == false)
        {
            ctx->err = CIOT_ERR_TIMEOUT;
        }
    }

    return ctx->err;
}

static ciot_err_t create_iface(uint16_t id, ciot_cmd_context_t *ctx)
{
    ciot_msg_data_t data = { 0 };
    ciot_storage_t storage = ciot_storage_fs_new();
    char filename[16];
    sprintf(filename, "%d.dat", id);
    ciot_err_t err = ciot_storage_load_data(storage, filename, &data);
    if(err == CIOT_ERR_OK)
    {
        switch (data.which_type)
        {
        case CIOT_MSG_DATA_UART_TAG:
            ctx->iface =  (ciot_iface_t*)ciot_uart_new(CIOT_HANDLE);
            break;
        case CIOT_MSG_DATA_HTTP_CLIENT_TAG:
            ctx->iface = (ciot_iface_t*)ciot_http_client_new(CIOT_HANDLE);
            break;
        case CIOT_MSG_DATA_MQTT_CLIENT_TAG:
            ctx->iface = (ciot_iface_t*)ciot_mqtt_client_new(CIOT_HANDLE);
            break;
        default:
            break;
        }
        if(ctx->iface->ptr == NULL)
        {
            return CIOT_ERR_NO_MEMORY;
        }
        return ctx->iface->process_data(ctx->iface, &data);
    }
    return err;
}

static ciot_err_t iface_event_handler(ciot_iface_t *sender, ciot_event_t *event, void *args)
{
    ciot_cmd_context_t *ctx = (ciot_cmd_context_t *)args;

    return CIOT_ERR_OK;
}