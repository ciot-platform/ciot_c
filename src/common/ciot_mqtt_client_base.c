/**
 * @file ciot_mqtt_client_base.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-12-11
 *
 * @copyright Copyright (c) 2024
 *
 */
 
#include "ciot_config.h"

#if CIOT_CONFIG_FEATURE_MQTT_CLIENT == 1
 
#include "ciot_mqtt_client.h"
#include "ciot_timer.h"
#include "ciot_types.h"

static ciot_err_t ciot_mqtt_client_process_data(ciot_iface_t *iface, ciot_msg_data_t *data);
static ciot_err_t ciot_mqtt_client_get_data(ciot_iface_t *iface, ciot_msg_data_t *data);
static ciot_err_t ciot_mqtt_client_send_data(ciot_iface_t *iface, uint8_t *data, int size);

ciot_err_t ciot_mqtt_client_init(ciot_mqtt_client_t self)
{
    ciot_mqtt_client_base_t *base = (ciot_mqtt_client_base_t *)self;
    base->iface.ptr = self;
    base->iface.process_data = ciot_mqtt_client_process_data;
    base->iface.get_data = ciot_mqtt_client_get_data;
    base->iface.send_data = ciot_mqtt_client_send_data;
    base->iface.info.type = CIOT_IFACE_TYPE_MQTT_CLIENT;
    base->status.has_error = true;
    return CIOT_ERR_OK;
}

ciot_err_t ciot_mqtt_client_process_req(ciot_mqtt_client_t self, ciot_mqtt_client_req_t *req)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(req);
    return CIOT_ERR_NOT_SUPPORTED;
}

ciot_err_t ciot_mqtt_client_update_data_rate(ciot_mqtt_client_t self, int bytes_sended)
{
    ciot_mqtt_client_base_t *base = (ciot_mqtt_client_base_t *)self;
    if (base->status.state == CIOT_MQTT_CLIENT_STATE_CONNECTED)
    {
        base->data_rate_aux += bytes_sended;
        if (ciot_timer_now() != base->status.last_msg_time)
        {
            base->status.last_msg_time = ciot_timer_now();
            base->status.data_rate = base->data_rate_aux;
            base->data_rate_aux = 0;
        }
    }
    return CIOT_ERR_OK;
}

ciot_err_t ciot_mqtt_client_subtopic_pub(ciot_mqtt_client_t self, char *subtopic, int subtopic_len, uint8_t *data, int size, int qos)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(subtopic);
    CIOT_ERR_NULL_CHECK(data);
    CIOT_ERR_RETURN(ciot_mqtt_client_set_subtopic(self, subtopic, subtopic_len));
    CIOT_ERR_RETURN(ciot_mqtt_client_send_data((ciot_iface_t*)self, data, size));
    ciot_mqtt_client_base_t *base = (ciot_mqtt_client_base_t*)self;
    base->topic_pub[base->topic_len] = '\0';
    return CIOT_ERR_OK;
}

ciot_err_t ciot_mqtt_client_subtopic_sub(ciot_mqtt_client_t self, char *subtopic, int subtopic_len, int qos)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(subtopic);
    ciot_mqtt_client_base_t *base = (ciot_mqtt_client_base_t*)self;
    char topic[sizeof(base->cfg.topics.pub) + 16];
    sprintf(topic, "%s%s", base->topic_sub, subtopic);
    return ciot_mqtt_client_sub(self, topic, qos);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_mqtt_client_set_topics(ciot_mqtt_client_t self, char *pub, char *sub)
{
    ciot_mqtt_client_base_t *base = (ciot_mqtt_client_base_t *)self;
    CIOT_ERR_NULL_CHECK(self);
    strcpy(base->cfg.topics.pub, pub);
    strcpy(base->cfg.topics.sub, sub);
    return ciot_mqtt_client_sub(self, sub, base->cfg.qos);
    ;
}

ciot_err_t ciot_mqtt_client_set_subtopic(ciot_mqtt_client_t self, char *subtopic, int len)
{
    ciot_mqtt_client_base_t *base = (ciot_mqtt_client_base_t*)self;
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_INDEX_CHECK(base->topic_len, 0, sizeof(base->topic_pub));
    memcpy(&base->topic_pub[base->topic_len], subtopic, len);
    return CIOT_ERR_OK;
}

ciot_mqtt_client_state_t ciot_mqtt_client_get_state(ciot_mqtt_client_t self)
{
    ciot_mqtt_client_base_t *base = (ciot_mqtt_client_base_t*)self;
    CIOT_ERR_NULL_CHECK(self);
    return base->status.state;
}

// bool ciot_mqtt_client_is_connected(ciot_mqtt_client_t self)
// {
//     ciot_mqtt_client_base_t *base = (ciot_mqtt_client_base_t*)self;
//     return base->status.state == CIOT_MQTT_CLIENT_STATE_CONNECTED;
// }

static ciot_err_t ciot_mqtt_client_process_data(ciot_iface_t *iface, ciot_msg_data_t *data)
{
    CIOT_ERR_TYPE_CHECK(data->which_type, CIOT_MSG_DATA_MQTT_CLIENT_TAG);

    ciot_mqtt_client_t self = iface->ptr;
    ciot_mqtt_client_data_t *mqtt_client = &data->mqtt_client;

    switch (mqtt_client->which_type)
    {
    case CIOT_MQTT_CLIENT_DATA_STOP_TAG:
        return ciot_mqtt_client_stop(self);
    case CIOT_MQTT_CLIENT_DATA_CONFIG_TAG:
        return ciot_mqtt_client_start(self, &mqtt_client->config);
    case CIOT_MQTT_CLIENT_DATA_REQUEST_TAG:
        return ciot_mqtt_client_process_req(self, &mqtt_client->request);
    default:
        return CIOT_ERR_INVALID_TYPE;
    }

    return CIOT_ERR_NOT_IMPLEMENTED;
}

static ciot_err_t ciot_mqtt_client_get_data(ciot_iface_t *iface, ciot_msg_data_t *data)
{
    CIOT_ERR_TYPE_CHECK(data->which_type, CIOT_MSG_DATA_GET_DATA_TAG);

    ciot_mqtt_client_base_t *self = iface->ptr;
    ciot_data_type_t data_type = data->get_data.type;
    data->which_type = CIOT_MSG_DATA_MQTT_CLIENT_TAG;

    switch (data_type)
    {
    case CIOT_DATA_TYPE_CONFIG:
        data->mqtt_client.which_type = CIOT_MQTT_CLIENT_DATA_CONFIG_TAG;
        data->mqtt_client.config = self->cfg;
        break;
    case CIOT_DATA_TYPE_STATUS:
        data->mqtt_client.which_type = CIOT_MQTT_CLIENT_DATA_STATUS_TAG;
        data->mqtt_client.status = self->status;
        break;
    default:
        return CIOT_ERR_NOT_FOUND;
    }

    return CIOT_ERR_OK;
}

static ciot_err_t ciot_mqtt_client_send_data(ciot_iface_t *iface, uint8_t *data, int size)
{
    ciot_mqtt_client_t self = (ciot_mqtt_client_t)iface;
    ciot_mqtt_client_base_t *base = (ciot_mqtt_client_base_t *)self;
    return ciot_mqtt_client_pub(self, base->topic_pub, data, size, base->cfg.qos);
}

ciot_err_t ciot_mqtt_client_get_cfg(ciot_mqtt_client_t self, ciot_mqtt_client_cfg_t *cfg)
{
    ciot_mqtt_client_base_t *base = (ciot_mqtt_client_base_t*)self;
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(cfg);
    *cfg = base->cfg;
    return CIOT_ERR_OK;
}

ciot_err_t ciot_mqtt_client_get_status(ciot_mqtt_client_t self, ciot_mqtt_client_status_t *status)
{
    ciot_mqtt_client_base_t *base = (ciot_mqtt_client_base_t*)self;
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(status);
    *status = base->status;
    return CIOT_ERR_OK;
}

ciot_err_t ciot_mqtt_client_set_process_all_topics(ciot_mqtt_client_t self, bool process_all_topics)
{
    ciot_mqtt_client_base_t *base = (ciot_mqtt_client_base_t*)self;
    CIOT_ERR_NULL_CHECK(self);
    base->process_all_topics = process_all_topics;
    return CIOT_ERR_OK;
}

#endif // CIOT_CONFIG_FEATURE_MQTT_CLIENT == 1