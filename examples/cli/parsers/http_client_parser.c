/**
 * @file http_client_parser.c
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
#include "http_client_parser.h"

static ciot_http_client_method_t parse_method(const char *str)
{
    if (strcmp(str, "GET") == 0)
        return CIOT_HTTP_METHOD_GET;
    if (strcmp(str, "POST") == 0)
        return CIOT_HTTP_METHOD_POST;
    if (strcmp(str, "PUT") == 0)
        return CIOT_HTTP_METHOD_PUT;
    if (strcmp(str, "DELETE") == 0)
        return CIOT_HTTP_METHOD_DELETE;
    return -1;
}

static ciot_http_client_transport_type_t parse_transport(const char *str)
{
    if (strcmp(str, "TCP") == 0)
        return CIOT_HTTP_TRANSPORT_TCP;
    if (strcmp(str, "SSL") == 0)
        return CIOT_HTTP_TRANSPORT_SSL;
    return CIOT_HTTP_TRANSPORT_UNKNOWN;
}

ciot_err_t parse_http_client_args(int argc, char **argv, ciot_http_client_data_t *http_client_data)
{
    memset(http_client_data, 0, sizeof(ciot_http_client_data_t));

    http_client_data->which_type = CIOT_HTTP_CLIENT_DATA_CONFIG_TAG;
    for (int i = 2; i < argc; i++)
    {
        const char *val;
        if ((val = get_arg_value(argv[i], "--url=")) != NULL)
        {
            strncpy(http_client_data->config.url, val, sizeof(http_client_data->config.url) - 1);
        }
        else if ((val = get_arg_value(argv[i], "--method=")) != NULL)
        {
            http_client_data->config.method = parse_method(val);
        }
        else if ((val = get_arg_value(argv[i], "--transport=")) != NULL)
        {
            http_client_data->config.transport = parse_transport(val);
        }
        else if ((val = get_arg_value(argv[i], "--timeout=")) != NULL)
        {
            http_client_data->config.timeout = atoi(val);
        }
    }

    return CIOT_ERR_OK;
}

ciot_err_t print_http_client_data(ciot_http_client_data_t *http_client_data)
{
    switch (http_client_data->which_type)
    {
    case CIOT_HTTP_CLIENT_DATA_STOP_TAG:
        printf("HTTP Client Stop\n");
        break;
    case CIOT_HTTP_CLIENT_DATA_CONFIG_TAG:
        printf("HTTP Client Config:\n");
        printf("--url=%s\n", http_client_data->config.url);
        printf("--method=%d\n", http_client_data->config.method);
        printf("--transport= %d\n", http_client_data->config.transport);
        printf("--timeout=%u\n", http_client_data->config.timeout);
        break;
    case CIOT_HTTP_CLIENT_DATA_STATUS_TAG:
        printf("HTTP Client Status:\n");
        printf("--error=%d\n", http_client_data->status.error);
        printf("--state=%d\n", http_client_data->status.state);
        break;
    case CIOT_HTTP_CLIENT_DATA_REQUEST_TAG:
        printf("HTTP Client Request:\n");
        break;
    default:
        printf("Unknown HTTP Client Data Type\n");
        return CIOT_ERR_INVALID_TYPE;
    }
    return CIOT_ERR_OK;
}