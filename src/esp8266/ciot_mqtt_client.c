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

#if CIOT_CONFIG_FEATURE_MQTT_CLIENT == 1 && defined(CIOT_PLATFORM_ESP8266)

#include "ciot_mqtt_client.h"

#include "ciot_err.h"
#include "ciot_timer.h"
#ifdef CONFIG_MBEDTLS_CERTIFICATE_BUNDLE
#include "esp_crt_bundle.h"
#endif
#include "mqtt_client.h"

struct ciot_mqtt_client
{
    ciot_mqtt_client_base_t base;
    esp_mqtt_client_handle_t handle;
};

static void ciot_mqtt_event_handler(void *handler_args, esp_event_base_t event_base, int32_t event_id, void *event_data);

static const char *TAG = "ciot_mqtt_client";

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
    ciot_mqtt_client_base_t *base = &self->base;

    if(cfg->has_last_will == false && base->cfg.has_last_will)
    {
        cfg->has_last_will = true;
        cfg->last_will = base->cfg.last_will;
    }

    if(cfg->has_topics == false && base->cfg.has_topics)
    {
        cfg->has_topics = true;
        cfg->topics = base->cfg.topics;
    }

    base->cfg = *cfg;
    if (base->cfg.has_topics)
    {
        strcpy(base->topic_sub, base->cfg.topics.sub);
        strcpy(base->topic_pub, base->cfg.topics.pub);
        base->topic_len = strlen(base->cfg.topics.pub);
    }

    const esp_mqtt_client_config_t mqtt_client_cfg = {
        .uri = base->cfg.url,
        .client_id = base->cfg.client_id,
        .username = base->cfg.user,
        .password = base->cfg.password,
        .keepalive = base->cfg.has_session ? base->cfg.session.keep_alive : 120,
        .disable_clean_session = base->cfg.has_session ? !base->cfg.session.clean_session : false,
        .lwt_msg = base->cfg.has_last_will ? (const char*)base->cfg.last_will.payload.bytes : NULL,
        .lwt_msg_len = base->cfg.has_last_will ? base->cfg.last_will.payload.size : 0,
        .lwt_qos = base->cfg.has_last_will ? base->cfg.last_will.qos : 0,
        .lwt_topic = base->cfg.has_last_will ? base->cfg.last_will.topic : NULL,
#ifdef CONFIG_MBEDTLS_CERTIFICATE_BUNDLE
        .broker.verification.crt_bundle_attach = esp_crt_bundle_attach,
#endif
    };

    base->status.state = CIOT_MQTT_CLIENT_STATE_CONNECTING;

    esp_mqtt_client_destroy(self->handle);
    self->handle = esp_mqtt_client_init(&mqtt_client_cfg);
    esp_mqtt_client_register_event(self->handle, ESP_EVENT_ANY_ID, ciot_mqtt_event_handler, self);
    esp_err_t err = esp_mqtt_client_start(self->handle);

    return err == ESP_OK ? CIOT_ERR_OK : CIOT_ERR_FAIL;
}

ciot_err_t ciot_mqtt_client_stop(ciot_mqtt_client_t self)
{
    CIOT_ERR_NULL_CHECK(self);
    self->base.status.state = CIOT_MQTT_CLIENT_STATE_DISCONNECTING;
    esp_err_t err = esp_mqtt_client_stop(self->handle);
    return err == ESP_OK ? CIOT_ERR_OK : CIOT_ERR_FAIL;
}

ciot_err_t ciot_mqtt_client_sub(ciot_mqtt_client_t self, char *topic, int qos)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(topic);
    esp_err_t err = esp_mqtt_client_subscribe(self->handle, topic, qos);
    return err == ESP_OK ? CIOT_ERR_OK : CIOT_ERR_FAIL;
}

ciot_err_t ciot_mqtt_client_pub(ciot_mqtt_client_t self, char *topic, uint8_t *data, int size, int qos)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(topic);
    CIOT_ERR_NULL_CHECK(self->handle);
    CIOT_ERR_VALUE_CHECK(self->base.status.state, CIOT_MQTT_CLIENT_STATE_CONNECTED, CIOT_ERR_INVALID_STATE);
    int err = esp_mqtt_client_publish(self->handle, topic, (char *)data, size, qos, false);
    if (err == 0)
        ciot_mqtt_client_update_data_rate(self, size);
    return err == ESP_OK ? CIOT_ERR_OK : CIOT_ERR_FAIL;
}

static void ciot_mqtt_event_handler(void *handler_args, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    ciot_mqtt_client_t self = (ciot_mqtt_client_t)handler_args;
    ciot_mqtt_client_base_t *base = &self->base;
    esp_mqtt_event_t *mqtt_event = (esp_mqtt_event_t *)event_data;
    ciot_event_t event = {0};

    switch (event_id)
    {
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        base->status.state = CIOT_MQTT_CLIENT_STATE_ERROR;
        base->status.error.code = mqtt_event->error_handle->connect_return_code;
        base->status.error.type = mqtt_event->error_handle->error_type;
        ciot_iface_send_event_type(&base->iface, CIOT_EVENT_TYPE_ERROR);
        break;
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        base->status.state = CIOT_MQTT_CLIENT_STATE_CONNECTED;
        base->status.conn_count++;
        ciot_mqtt_client_sub(self, base->cfg.topics.sub, base->cfg.qos);
        ciot_iface_send_event_type(&base->iface, CIOT_EVENT_TYPE_STARTED);
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        ciot_iface_send_event_type(&base->iface, CIOT_EVENT_TYPE_STOPPED);
        break;
    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED");
        ciot_iface_send_event_type(&base->iface, CIOT_EVENT_TYPE_INTERNAL);
        return;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED");
        ciot_iface_send_event_type(&base->iface, CIOT_EVENT_TYPE_INTERNAL);
        return;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        if (strncmp(mqtt_event->topic, base->cfg.topics.sub, mqtt_event->topic_len) == 0 || base->process_all_topics)
        {
            event.type = CIOT_EVENT_TYPE_REQUEST;
            event.raw.size = mqtt_event->data_len;
            memcpy(event.raw.bytes, (uint8_t *)mqtt_event->data, mqtt_event->data_len);
            ciot_iface_send_event(&base->iface, &event);
        }
        else
        {
            ciot_mqtt_client_event_data_t *event_data = (ciot_mqtt_client_event_data_t *)&event.raw.bytes[0];
            event.type = CIOT_EVENT_TYPE_DATA;
            event.raw.size = mqtt_event->data_len;
            memcpy(event_data->topic, mqtt_event->topic, mqtt_event->topic_len);
            memcpy(event_data->data, mqtt_event->data, mqtt_event->data_len);
            ciot_iface_send_event(&base->iface, &event);
        }
        break;
    default:
        return;
    }
}

#endif //! CIOT_CONFIG_FEATURE_MQTT_CLIENT == 1