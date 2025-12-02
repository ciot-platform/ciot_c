/**
 * @file ciot_http_client.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-12-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "ciot_config.h"

#if CIOT_CONFIG_FEATURE_HTTP_CLIENT == 1 && defined(CIOT_PLATFORM_MONGOOSE)
 
#include "ciot_http_client.h"

#include <stdlib.h>
#include <string.h>
#include "mongoose.h"

typedef struct ciot_http_data_to_send
{
    uint8_t data[CIOT_CONFIG_MSG_SIZE];
    int size;
} ciot_http_data_to_send_t;

struct ciot_http_client
{
    ciot_http_client_base_t base;
    struct mg_mgr *mgr;
    ciot_http_data_to_send_t data_to_send;
};

static const char *TAG = "ciot_http_client";

static void ciot_httpc_event_handler(struct mg_connection *c, int ev, void *ev_data);

static const char *ciot_httpc_get_method(ciot_http_client_method_t method);

ciot_http_client_t ciot_http_client_new(void *handle)
{
    ciot_http_client_t self = calloc(1, sizeof(struct ciot_http_client));
    ciot_http_client_init(self);
    self->mgr = handle;
    return self;
}

ciot_err_t ciot_http_client_start(ciot_http_client_t self, ciot_http_client_cfg_t *cfg)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(cfg);
    self->base.cfg = *cfg;
    ciot_iface_send_event_type(&self->base.iface, CIOT_EVENT_TYPE_STARTED);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_http_client_stop(ciot_http_client_t self)
{
    CIOT_ERR_NULL_CHECK(self);
    return CIOT_ERR_NOT_SUPPORTED;
}

ciot_err_t ciot_http_client_send_bytes(ciot_http_client_t self, uint8_t *data, int size)
{
    ciot_http_client_base_t *base = (ciot_http_client_base_t *)self;
    if(base->status.state == CIOT_HTTP_CLIENT_STATE_IDLE || base->status.state == CIOT_HTTP_CLIENT_STATE_TIMEOUT)
    {
        base->status.state = CIOT_HTTP_CLIENT_STATE_CONNECTING;
        self->data_to_send.size = size;
        memcpy(self->data_to_send.data, data, size);
        mg_http_connect(self->mgr, base->cfg.url, ciot_httpc_event_handler, self);
        return CIOT_ERR_OK;
    }
    return CIOT_ERR_BUSY;
}

static void ciot_httpc_event_handler(struct mg_connection *c, int ev, void *ev_data)
{
    ciot_http_client_t self = c->fn_data;
    ciot_http_client_base_t *base = (ciot_http_client_base_t *)self;
    ciot_event_t event = {0};
    mg_event_t mg_event = ev;

    switch (mg_event)
    {
    case MG_EV_ERROR:
        CIOT_LOGE(TAG, "MG_EV_ERROR %s", (char *)ev_data);
        base->status.state = CIOT_HTTP_CLIENT_STATE_ERROR;
        event.type = CIOT_EVENT_TYPE_ERROR;
        ciot_iface_send_event(&base->iface, &event);
        break;
    case MG_EV_OPEN:
    {
        CIOT_LOGI(TAG, "MG_EV_OPEN url:%s", base->cfg.url);
        base->status.state = CIOT_HTTP_CLIENT_STATE_CONNECTING;
        *(uint64_t *)c->data = mg_millis() + base->cfg.timeout;
        break;
    }
    case MG_EV_POLL:
        CIOT_LOGD(TAG, "MG_EV_POLL");
        if (mg_millis() > *(uint64_t *)c->data && (base->status.state != CIOT_HTTP_CLIENT_STATE_IDLE))
        {
            mg_error(c, "Connect timeout");
            base->status.state = CIOT_HTTP_CLIENT_STATE_TIMEOUT;
            ciot_iface_send_event_type(&self->base.iface, CIOT_EVENT_TYPE_ERROR);
        }
        else
        {
            return;
        }
        break;
    case MG_EV_CONNECT:
    {
        CIOT_LOGI(TAG, "MG_EV_CONNECT");
        struct mg_str host = mg_url_host(base->cfg.url);
        mg_printf(c,
                  "%s %s HTTP/1.0\r\n"
                  "Host: %.*s\r\n"
                  "Content-Type: octet-stream\r\n"
                  "Content-Length: %d\r\n"
                  "\r\n",
                  ciot_httpc_get_method(base->cfg.method), mg_url_uri(base->cfg.url), (int)host.len,
                  host.buf, self->data_to_send.size);
             mg_send(c, self->data_to_send.data, self->data_to_send.size);
        base->status.state = CIOT_HTTP_CLIENT_STATE_CONNECTED;
        break;
    }
    case MG_EV_HTTP_MSG:
    {
        CIOT_LOGI(TAG, "MG_EV_HTTP_MSG");
        struct mg_http_message *hm = ev_data, tmp = {0};
        mg_http_parse((char *)c->recv.buf, c->recv.len, &tmp);
        base->status.state = CIOT_HTTP_CLIENT_STATE_IDLE;
        event.type = CIOT_EVENT_TYPE_REQUEST;
        memcpy(&event.raw.bytes, hm->body.buf, hm->body.len);
        event.raw.size = hm->body.len;
        ciot_iface_send_event(&base->iface, &event);
        break;
    }
    case MG_EV_CLOSE:
    {
        CIOT_LOGI(TAG, "MG_EV_CLOSE");
        base->status.state = CIOT_HTTP_CLIENT_STATE_IDLE;
        event.type = CIOT_EVENT_TYPE_STOPPED;
        ciot_iface_send_event(&base->iface, &event);
        break;
    }
    default:
        return;
    }
}

static const char *ciot_httpc_get_method(ciot_http_client_method_t method)
{
    switch (method)
    {
        case CIOT_HTTP_METHOD_GET:
            return "GET";
        case CIOT_HTTP_METHOD_POST:
            return "POST";
        case CIOT_HTTP_METHOD_PUT:
            return "PUT";
        case CIOT_HTTP_METHOD_PATCH:
            return "PATCH";
        case CIOT_HTTP_METHOD_DELETE:
            return "DELETE";
        case CIOT_HTTP_METHOD_HEAD:
            return "HEAD";
        case CIOT_HTTP_METHOD_NOTIFY:
            return "NOTIFY";
        case CIOT_HTTP_METHOD_SUBSCRIBE:
            return "SUBSCRIBE";
        case CIOT_HTTP_METHOD_UNSUBSCRIBE:
            return "UNSUBSCRIBE";
        case CIOT_HTTP_METHOD_OPTIONS:
            return "OPTIONS";
        case CIOT_HTTP_METHOD_COPY:
            return "COPY";
        case CIOT_HTTP_METHOD_MOVE:
            return "MOVE";
        case CIOT_HTTP_METHOD_LOCK:
            return "LOCK";
        case CIOT_HTTP_METHOD_UNLOCK:
            return "UNLOCK";
        case CIOT_HTTP_METHOD_PROPFIND:
            return "PROPFIND";
        case CIOT_HTTP_METHOD_PROPPATCH:
            return "PROPPATCH";
        case CIOT_HTTP_METHOD_MKCOL:
            return "MKCOL";
        default:
            return "UNKNOWN"; // Tratar métodos desconhecidos
    }
}

#endif // CIOT_CONFIG_FEATURE_HTTP_CLIENT == 1
