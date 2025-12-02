/**
 * @file ciot_mqtt_client.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-12-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef __CIOT_MQTT_CLIENT__H__
#define __CIOT_MQTT_CLIENT__H__

#ifdef __cplusplus
extern "C" {
#endif

#include "ciot_config.h"
#include "ciot_types.h"
#include "ciot_iface.h"

typedef struct ciot_mqtt_client *ciot_mqtt_client_t;

#ifndef CIOT_CONFIG_MQTT_TOPIC_SIZE
#define CIOT_CONFIG_MQTT_TOPIC_SIZE 48
#endif

typedef struct ciot_mqtt_client_event_data
{
    char topic[CIOT_CONFIG_MQTT_TOPIC_SIZE];
    uint8_t data[CIOT_CONFIG_MSG_SIZE];
} ciot_mqtt_client_event_data_t;

typedef struct ciot_mqtt_client_base
{
    ciot_iface_t iface;
    ciot_mqtt_client_cfg_t cfg;
    ciot_mqtt_client_status_t status;
    uint16_t data_rate_aux;
    char topic_pub[CIOT_CONFIG_MQTT_TOPIC_SIZE];
    char topic_sub[CIOT_CONFIG_MQTT_TOPIC_SIZE];
    uint16_t topic_len;
    bool process_all_topics;
} ciot_mqtt_client_base_t;

ciot_mqtt_client_t ciot_mqtt_client_new(void *handle);
ciot_err_t ciot_mqtt_client_init(ciot_mqtt_client_t self);
ciot_err_t ciot_mqtt_client_start(ciot_mqtt_client_t self, ciot_mqtt_client_cfg_t *cfg);
ciot_err_t ciot_mqtt_client_stop(ciot_mqtt_client_t self);
ciot_err_t ciot_mqtt_client_process_req(ciot_mqtt_client_t self, ciot_mqtt_client_req_t *req);
ciot_err_t ciot_mqtt_client_get_cfg(ciot_mqtt_client_t self, ciot_mqtt_client_cfg_t *cfg);
ciot_err_t ciot_mqtt_client_get_status(ciot_mqtt_client_t self, ciot_mqtt_client_status_t *status);
ciot_err_t ciot_mqtt_client_set_process_all_topics(ciot_mqtt_client_t self, bool process_all_topics);

ciot_err_t ciot_mqtt_client_sub(ciot_mqtt_client_t self, char *topic, int qos);
ciot_err_t ciot_mqtt_client_pub(ciot_mqtt_client_t self, char *topic, uint8_t *data, int size, int qos);
ciot_err_t ciot_mqtt_client_subtopic_pub(ciot_mqtt_client_t self, char *subtopic, int subtopic_len, uint8_t *data, int size, int qos);
ciot_err_t ciot_mqtt_client_subtopic_sub(ciot_mqtt_client_t self, char *subtopic, int subtopic_len, int qos);
ciot_err_t ciot_mqtt_client_send(ciot_mqtt_client_t self, uint8_t *data, int size);
ciot_err_t ciot_mqtt_client_set_topics(ciot_mqtt_client_t self, char *d2b, char *b2d);
ciot_err_t ciot_mqtt_client_set_subtopic(ciot_mqtt_client_t self, char *subtopic, int len);
ciot_err_t ciot_mqtt_client_update_data_rate(ciot_mqtt_client_t self, int bytes_sended);
ciot_mqtt_client_state_t ciot_mqtt_client_get_state(ciot_mqtt_client_t self);

#ifdef __cplusplus
}
#endif

#endif  //!__CIOT_MQTT_CLIENT__H__
