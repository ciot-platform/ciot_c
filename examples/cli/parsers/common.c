/**
 * @file common.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-05-16
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "common.h"

const char *get_arg_value(const char *arg, const char *prefix)
{
    size_t len = strlen(prefix);
    return strncmp(arg, prefix, len) == 0 ? arg + len : NULL;
}

bool parse_bool(const char *str)
{
    return strcmp(str, "true") == 0 || strcmp(str, "1") == 0;
}

ciot_iface_type_t parse_iface_type(const char *str)
{
    if (strcmp(str, "ciot") == 0)
        return CIOT_IFACE_TYPE_CIOT;
    if (strcmp(str, "ota") == 0)
        return CIOT_IFACE_TYPE_OTA;
    if (strcmp(str, "uart") == 0)
        return CIOT_IFACE_TYPE_UART;
    if (strcmp(str, "http_client") == 0)
        return CIOT_IFACE_TYPE_HTTP_CLIENT;
    if (strcmp(str, "mqtt_client") == 0)
        return CIOT_IFACE_TYPE_MQTT_CLIENT;
    return CIOT_IFACE_TYPE_UNDEFINED;
}

ciot_data_type_t parse_data_type(const char *str)
{
    if(strcmp(str, "stop") == 0)    
        return CIOT_DATA_TYPE_STOP;
    if(strcmp(str, "config") == 0)    
        return CIOT_DATA_TYPE_CONFIG;
    if(strcmp(str, "status") == 0)    
        return CIOT_DATA_TYPE_STATUS;
    if(strcmp(str, "request") == 0)    
        return CIOT_DATA_TYPE_REQUEST;
    if(strcmp(str, "info") == 0)    
        return CIOT_DATA_TYPE_INFO;
    return CIOT_DATA_TYPE_UNKNOWN;
}