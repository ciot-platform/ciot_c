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

#if CIOT_CONFIG_FEATURE_MQTT_CLIENT == 1 && defined(CIOT_PLATFORM_MONGOOSE)

#include "ciot_mqtt_client.h"
#include "ciot_ca_crt_all.h"
#include "ciot_timer.h"
#include "mongoose.h"

struct ciot_mqtt_client
{
    ciot_mqtt_client_base_t base;
    struct mg_mgr *mgr;
    struct mg_connection *connection;
    time_t last_ping;
    bool reconnecting;
};

static const char *TAG = "ciot_mqtt_client";

static void ciot_mqtt_client_event_handler(struct mg_connection *c, int ev, void *ev_data);

ciot_mqtt_client_t ciot_mqtt_client_new(void *handle)
{
    ciot_mqtt_client_t self = calloc(1, sizeof(struct ciot_mqtt_client));
    ciot_mqtt_client_init(self);
    self->mgr = handle;
    return self;
}

ciot_err_t ciot_mqtt_client_start(ciot_mqtt_client_t self, ciot_mqtt_client_cfg_t *cfg)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(cfg);
    CIOT_ERR_NULL_CHECK(self->mgr);

    ciot_mqtt_client_base_t *base = &self->base;
    struct mg_mqtt_opts opts = { 0 };

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
    if(base->cfg.has_topics)
    {
        strcpy(base->topic_sub, base->cfg.topics.sub);
        strcpy(base->topic_pub, base->cfg.topics.pub);
        base->topic_len = strlen(base->cfg.topics.pub);
    }

    if(base->cfg.has_last_will)
    {
        opts.retain = base->cfg.last_will.retain;
        opts.topic = mg_str(base->cfg.last_will.topic);
        opts.message.buf = (char*)base->cfg.last_will.payload.bytes;
        opts.message.len = base->cfg.last_will.payload.size;
    }

    opts.client_id = mg_str(base->cfg.client_id);
    opts.user = mg_str(base->cfg.user);
    opts.pass = mg_str(base->cfg.password);
    opts.qos = base->cfg.qos;
    opts.version = 4;

    if(base->cfg.has_session)
    {
        opts.clean = base->cfg.session.clean_session;
        opts.keepalive = (uint16_t)base->cfg.session.keep_alive;
    }
    else
    {
        opts.clean = true;
        opts.keepalive = 60;
    }

    if(self->connection != NULL && self->connection->is_closing == false)
    {
        mg_mqtt_disconnect(self->connection, &opts);
        self->reconnecting = true;
    }

    base->status.state = CIOT_MQTT_CLIENT_STATE_CONNECTING;
    self->connection = mg_mqtt_connect(self->mgr, base->cfg.url, &opts, ciot_mqtt_client_event_handler, self);

    return CIOT_ERR_OK;
}

ciot_err_t ciot_mqtt_client_stop(ciot_mqtt_client_t self)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(self->connection);
    if(self->connection != NULL)
    {
        struct mg_mqtt_opts opts = {0};
        self->base.status.state = CIOT_MQTT_CLIENT_STATE_DISCONNECTING;
        mg_mqtt_disconnect(self->connection, &opts);
    }
    return CIOT_ERR_OK;
}

ciot_err_t ciot_mqtt_client_sub(ciot_mqtt_client_t self, char *topic, int qos)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(topic);
    CIOT_ERR_NULL_CHECK(self->connection);
    CIOT_ERR_EMPTY_STRING_CHECK(topic);
    struct mg_mqtt_opts opts = {0};
    opts.topic = mg_str(topic);
    opts.qos = qos;
    mg_mqtt_sub(self->connection, &opts);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_mqtt_client_pub(ciot_mqtt_client_t self, char *topic, uint8_t *data, int size, int qos)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(topic);
    CIOT_ERR_NULL_CHECK(self->connection);
    CIOT_ERR_VALUE_CHECK(self->base.status.state, CIOT_MQTT_CLIENT_STATE_CONNECTED, CIOT_ERR_INVALID_STATE);
    CIOT_ERR_EMPTY_STRING_CHECK(topic);
    struct mg_mqtt_opts opts = {0};
    struct mg_str msg = {0};
    msg.buf = (char*)data;
    msg.len = size;
    opts.topic = mg_str(topic);
    opts.message = msg;
    opts.qos = qos;
    opts.retain = false;
    mg_mqtt_pub(self->connection, &opts);
    ciot_mqtt_client_update_data_rate(self, size);
    return CIOT_ERR_OK;
}

static void ciot_mqtt_client_event_handler(struct mg_connection *c, int ev, void *ev_data)
{
    ciot_mqtt_client_t self = c->fn_data;
    ciot_mqtt_client_base_t *base = &self->base;
    ciot_event_t event = {0};
    mg_event_t mg_ev = ev;

    switch (mg_ev)
    {
    case MG_EV_ERROR:
    {
        CIOT_LOGE(TAG, "MG_EV_ERROR (%s)", (char *)ev_data);
        base->status.state = CIOT_MQTT_CLIENT_STATE_ERROR;
        base->status.error.code = atoi(&((char*)ev_data)[9]);
        ciot_iface_send_event_type(&base->iface, CIOT_EVENT_TYPE_ERROR);
        break;
    }
    case MG_EV_OPEN:
        CIOT_LOGD(TAG, "MG_EV_OPEN url:%s", base->cfg.url);
        base->status.state = CIOT_MQTT_CLIENT_STATE_CONNECTING;
        event.type = CIOT_EVENT_TYPE_INTERNAL;
        ciot_iface_send_event_type(&base->iface, CIOT_EVENT_TYPE_INTERNAL);
        break;
    case MG_EV_CONNECT:
    {
        if(mg_url_is_ssl(self->base.cfg.url))
        {
            struct mg_tls_opts opts = {
                .ca = mg_str(ca_crt_all),
                .name = mg_url_host(self->base.cfg.url),
                .skip_verification = true
            };
            mg_tls_init(c, &opts);
        }
        break;
    }
    case MG_EV_POLL:
    {
        if(base->status.state == CIOT_MQTT_CLIENT_STATE_CONNECTED &&
           ciot_timer_now() >= self->last_ping + 10)
        {
            CIOT_LOGD(TAG, "MQTT_EV_PING");
            mg_mqtt_ping(c);
            self->last_ping = ciot_timer_now();
        }
        return;
    }
    case MG_EV_MQTT_OPEN:
    {
        CIOT_LOGI(TAG, "MG_EV_MQTT_OPEN url:%s ack:%d", base->cfg.url, *(int*)ev_data);
        int ack = *(int*)ev_data;
        if(base->status.state != CIOT_MQTT_CLIENT_STATE_CONNECTED && ack == 0)
        {
            base->status.conn_count++;
            base->status.state = CIOT_MQTT_CLIENT_STATE_CONNECTED;
            event.type = CIOT_EVENT_TYPE_STARTED;
            if(base->cfg.has_topics && base->cfg.topics.sub[0] != '\0')
            {
                ciot_mqtt_client_sub(self, base->cfg.topics.sub, base->cfg.qos);
            }
            ciot_iface_send_event_type(&base->iface, CIOT_EVENT_TYPE_STARTED);
        }
        break;
    }
    case MG_EV_MQTT_MSG:
    {
        CIOT_LOGI(TAG, "MG_EV_MQTT_MSG");
        struct mg_mqtt_message *mm = (struct mg_mqtt_message *)ev_data;
        if(base->process_all_topics || (strlen(base->cfg.topics.sub) == mm->topic.len && strncmp(mm->topic.buf, base->cfg.topics.sub, mm->topic.len) == 0))
        {
            event.type = CIOT_EVENT_TYPE_MSG;
            memcpy(event.raw.bytes, mm->data.buf, mm->data.len);
            event.raw.size = mm->data.len;
            ciot_iface_send_event(&base->iface, &event);
        }
        else
        {
            ciot_mqtt_client_event_data_t *event_data = (ciot_mqtt_client_event_data_t*)&event.raw.bytes[0];
            event.type = CIOT_EVENT_TYPE_DATA;
            event.raw.size = mm->data.len;
            memcpy(event_data->topic, mm->topic.buf, mm->topic.len);
            memcpy(event_data->data, mm->data.buf, mm->data.len);
            ciot_iface_send_event(&base->iface, &event);
        }
        break;
    }
    case MG_EV_CLOSE:
    {
        CIOT_LOGI(TAG, "MG_EV_CLOSE");
        if(!self->reconnecting)
        {
            base->status.state = CIOT_MQTT_CLIENT_STATE_DISCONNECTED;
            ciot_iface_send_event_type(&base->iface, CIOT_EVENT_TYPE_STOPPED);
        }
        self->reconnecting = false;
        break;
    }
    default:
        return;
    }
}

#endif // CIOT_CONFIG_FEATURE_MQTT_CLIENT == 1

