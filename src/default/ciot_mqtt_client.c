/**
 * @file ciot_mqtt_client.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-12-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "ciot_config.h"

#if CIOT_CONFIG_FEATURE_MQTT_CLIENT == 1 && defined(CIOT_PLATFORM_DEFAULT)

#include "ciot_mqtt_client.h"

struct ciot_mqtt_client
{
    ciot_mqtt_client_base_t base;
};

// static const char *TAG = "ciot_mqtt_client";

ciot_mqtt_client_t ciot_mqtt_client_new(void *handle)
{
    ciot_mqtt_client_t self = calloc(1, sizeof(struct ciot_mqtt_client));
    ciot_mqtt_client_init(self);
    return self;
}

ciot_err_t ciot_mqtt_client_start(ciot_mqtt_client_t self, ciot_mqtt_client_cfg_t *cfg)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(cfg);
    return CIOT_ERR_NOT_IMPLEMENTED;
}

ciot_err_t ciot_mqtt_client_stop(ciot_mqtt_client_t self)
{
    CIOT_ERR_NULL_CHECK(self);
    return CIOT_ERR_NOT_IMPLEMENTED;
}

// ciot_err_t ciot_mqtt_client_send_bytes(ciot_mqtt_client_t self, uint8_t *data, int size)
// {
//     return CIOT_ERR_NOT_IMPLEMENTED;
// }

ciot_err_t ciot_mqtt_client_sub(ciot_mqtt_client_t self, char *topic, int qos)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(topic);
    return CIOT_ERR_NOT_IMPLEMENTED;
}

ciot_err_t ciot_mqtt_client_pub(ciot_mqtt_client_t self, char *topic, uint8_t *data, int size, int qos)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(topic);
    return CIOT_ERR_NOT_IMPLEMENTED;
}

#endif // CIOT_CONFIG_FEATURE_MQTT_CLIENT == 1
