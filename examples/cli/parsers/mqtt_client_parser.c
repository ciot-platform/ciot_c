/**
 * @file mqtt_client_parser.c
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
#include "mqtt_client_parser.h"

ciot_err_t parse_mqtt_client_args(int argc, char **argv, ciot_mqtt_client_data_t *mqtt_client_data) {
    memset(mqtt_client_data, 0, sizeof(ciot_mqtt_client_data_t));

    mqtt_client_data->which_type = CIOT_MQTT_CLIENT_DATA_CONFIG_TAG;
    for (int i = 2; i < argc; i++) {
        const char *val;
        if ((val = get_arg_value(argv[i], "--url=")) != NULL) {
            strncpy(mqtt_client_data->config.url, val, sizeof(mqtt_client_data->config.url) - 1);
        }
        else if ((val = get_arg_value(argv[i], "--client-id=")) != NULL) {
            strncpy(mqtt_client_data->config.client_id, val, sizeof(mqtt_client_data->config.client_id) - 1);
        }
        else if ((val = get_arg_value(argv[i], "--user=")) != NULL) {
            strncpy(mqtt_client_data->config.user, val, sizeof(mqtt_client_data->config.user) - 1);
        }
        else if ((val = get_arg_value(argv[i], "--password=")) != NULL) {
            strncpy(mqtt_client_data->config.password, val, sizeof(mqtt_client_data->config.password) - 1);
        }
        else if ((val = get_arg_value(argv[i], "--qos=")) != NULL) {
            mqtt_client_data->config.qos = atoi(val);
        }
        // else if ((val = get_arg_value(argv[i], "--broker-kind=")) != NULL) {
        //     mqtt_client_data->config.broker_kind = atoi(val); // Enum ciot_mqtt_client_broker_kind_t
        // }
        else if ((val = get_arg_value(argv[i], "--topic-pub=")) != NULL) {
            mqtt_client_data->config.has_topics = true;
            strncpy(mqtt_client_data->config.topics.pub, val, sizeof(mqtt_client_data->config.topics.pub) - 1);
        }
        else if ((val = get_arg_value(argv[i], "--topic-sub=")) != NULL) {
            mqtt_client_data->config.has_topics = true;
            strncpy(mqtt_client_data->config.topics.sub, val, sizeof(mqtt_client_data->config.topics.sub) - 1);
        }
    }

    return CIOT_ERR_OK;
}

ciot_err_t print_mqtt_client_data(ciot_mqtt_client_data_t *mqtt_client_data)
{
    switch (mqtt_client_data->which_type)
    {
    case CIOT_MQTT_CLIENT_DATA_CONFIG_TAG:
        printf("MQTT Client Config:\n");
        printf("--url=%s\n", mqtt_client_data->config.url);
        printf("--client-id=%s\n", mqtt_client_data->config.client_id);
        printf("--user=%s\n", mqtt_client_data->config.user);
        printf("--password=%s\n", mqtt_client_data->config.password);
        printf("--qos=%u\n", mqtt_client_data->config.qos);
        if (mqtt_client_data->config.has_topics) {
            printf("  Topics:\n");
            printf("--topic-pub=%s\n", mqtt_client_data->config.topics.pub);
            printf("--topic-sub=%s\n", mqtt_client_data->config.topics.sub);
        }
        break;
    default:
        break;
    }
}
