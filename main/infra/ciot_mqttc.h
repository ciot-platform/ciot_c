/**
 * @file ciot_mqtt.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-10-09
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __CIOT_MQTT__H__
#define __CIOT_MQTT__H__

#include "ciot_mqttc_data.h"
#include "ciot_iface.h"
#include "ciot_err.h"

typedef struct ciot_mqttc *ciot_mqttc_t;

typedef enum ciot_mqttc_event_id
{
    CIOT_MQTT_EVENT_CONNECTING = CIOT_IFACE_EVENT_CUSTOM,
    CIOT_MQTT_EVENT_DATA,
    CIOT_MQTT_EVENT_SUBSCRIBED,
    CIOT_MQTT_EVENT_UNSUBCRIBED,
} ciot_mqttc_event_id_t;

ciot_mqttc_t ciot_mqttc_new(void *handle);
ciot_err_t ciot_mqttc_start(ciot_mqttc_t self, ciot_mqttc_cfg_t *cfg);
ciot_err_t ciot_mqttc_stop(ciot_mqttc_t self);
ciot_err_t ciot_mqttc_process_req(ciot_mqttc_t self, ciot_mqttc_req_t *req);
ciot_err_t ciot_mqttc_send_data(ciot_mqttc_t self, uint8_t *data, int size);

ciot_err_t ciot_mqttc_publish(ciot_mqttc_t self, ciot_mqttc_req_publish_t *req);
ciot_err_t ciot_mqttc_subscribe(ciot_mqttc_t self, ciot_mqttc_req_subscribe_t *req);

#endif  //!__CIOT_MQTT__H__