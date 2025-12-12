/**
 * @file ciot_http_server.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-12-11
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "ciot_config.h"

#if CIOT_CONFIG_FEATURE_HTTP_SERVER == 1 && defined(CIOT_PLATFORM_MONGOOSE)

#include "ciot_http_server.h"
#include "mongoose.h"

struct ciot_http_server
{
    ciot_http_server_base_t base;
    struct mg_mgr *mgr;
    struct mg_connection *conn_rx;
    struct mg_connection *conn_tx;
    char endpoint[64];
};

static const char *TAG = "ciot_http_server";

static bool check_method(struct mg_http_message *hm, const char *method);
static void ciot_http_server_event_handler(struct mg_connection *c, int ev, void *ev_data);

ciot_http_server_t ciot_http_server_new(void *handle)
{
    ciot_http_server_t self = calloc(1, sizeof(struct ciot_http_server));
    ciot_http_server_init(self);
    self->mgr = handle;
    return self;
}

ciot_err_t ciot_http_server_start(ciot_http_server_t self, ciot_http_server_cfg_t *cfg)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(cfg);
    CIOT_ERR_NULL_CHECK(self->mgr);

    ciot_http_server_base_t *base = &self->base;

    sprintf(self->endpoint, "%s:%ld", cfg->address, (long int)cfg->port);

    base->cfg = *cfg;

    self->conn_rx = mg_http_listen(self->mgr, self->endpoint, ciot_http_server_event_handler, self);
    if (self->conn_rx == NULL)
    {
        base->status.state = CIOT_HTTP_SERVER_STATE_ERROR;
        return CIOT_ERR_FAIL;
    }
    else
    {

        return CIOT_ERR_OK;
    }
}

ciot_err_t ciot_http_server_stop(ciot_http_server_t self)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(self->conn_rx);
    mg_close_conn(self->conn_rx);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_http_server_send_bytes(ciot_http_server_t self, uint8_t *data, int size)
{
    mg_printf(self->conn_tx,
              "HTTP/1.0 200 OK\r\n"
#ifdef CIOT_CONFIG_HTTP_SERVER_ALLOW_ORIGIN
              "Access-Control-Allow-Origin: " CIOT_CONFIG_HTTP_SERVER_ALLOW_ORIGIN "\r\n"
#endif
              "Content-Type: octet-stream\r\n"
              "Content-Length: %d\r\n\r\n",
              size);
    mg_send(self->conn_tx, data, size);
    return CIOT_ERR_OK;
}

static bool check_method(struct mg_http_message *hm, const char *method)
{
    return strncmp(hm->method.buf, method, hm->method.len) == 0;
}

static void ciot_http_server_event_handler(struct mg_connection *c, int ev, void *ev_data)
{
    ciot_http_server_t self = c->fn_data;
    ciot_http_server_base_t *base = &self->base;
    ciot_event_t event = {0};
    mg_event_t mg_ev = ev;

    switch (mg_ev)
    {
    case MG_EV_ERROR:
    {
        CIOT_LOGE(TAG, "MG_EV_ERROR (%s)", (char *)ev_data);
        base->status.state = CIOT_HTTP_SERVER_STATE_ERROR;
        event.type = CIOT_EVENT_TYPE_ERROR;
        ciot_iface_send_event(&base->iface, &event);
        break;
    }
    case MG_EV_OPEN:
    {
        CIOT_LOGI(TAG, "MG_EV_OPEN url:%s:%d", base->cfg.address, base->cfg.port);
        if (base->status.state != CIOT_HTTP_SERVER_STATE_STARTED)
        {
            base->status.state = CIOT_HTTP_SERVER_STATE_STARTED;
            event.type = CIOT_EVENT_TYPE_STARTED;
            ciot_iface_send_event(&base->iface, &event);
        }
        break;
    }
    case MG_EV_CLOSE:
    {
        CIOT_LOGI(TAG, "MG_EV_CLOSE");
        base->status.state = CIOT_HTTP_SERVER_STATE_STOPPED;
        event.type = CIOT_EVENT_TYPE_STOPPED;
        ciot_iface_send_event(&base->iface, &event);
        break;
    }
    case MG_EV_HTTP_MSG:
    {
        CIOT_LOGI(TAG, "MG_EV_HTTP_MSG");
        struct mg_http_message *hm = (struct mg_http_message *)ev_data;
        mg_http_parse((char *)c->recv.buf, c->recv.len, hm);
        self->conn_tx = c;
        if (mg_match(hm->uri, mg_str(base->cfg.route), NULL) && check_method(hm, "POST"))
        {
            event.type = CIOT_EVENT_TYPE_MSG;
            memcpy(event.raw.bytes, hm->body.buf, hm->body.len);
            event.raw.size = hm->body.len;
            ciot_iface_send_event(&base->iface, &event);
        }
        else if (mg_match(hm->uri, mg_str("/"), NULL) && check_method(hm, "GET") && base->homepage.size > 0)
        {
            mg_printf(self->conn_tx,
                      "HTTP/1.0 200 OK\r\n"
#ifdef CIOT_CONFIG_HTTP_SERVER_ALLOW_ORIGIN
                      "Access-Control-Allow-Origin: " CIOT_CONFIG_HTTP_SERVER_ALLOW_ORIGIN "\r\n"
#endif
                      "Content-Type: text/html\r\n"
                      "%s"
                      "Content-Length: %d\r\n"
                      "\r\n",
                      base->homepage.gz ? "Content-Encoding: gzip\r\n" : "",
                      (int)base->homepage.size);
            mg_send(self->conn_tx, base->homepage.data, base->homepage.size);
        }
        else if (base->cfg.root[0] != '\0' && check_method(hm, "GET"))
        {
            struct mg_http_serve_opts opts = {0};
            opts.root_dir = base->cfg.root;
            mg_http_serve_dir(c, hm, &opts);
        }
        else
        {
            ciot_http_server_event_data_t *event_data = (ciot_http_server_event_data_t *)&event.raw;
            memcpy(event_data->uri, hm->uri.buf, hm->uri.len);
            memcpy(event_data->body, hm->body.buf, hm->body.len);
            event.type = CIOT_EVENT_TYPE_DATA;
            ciot_iface_send_event(&base->iface, &event);
        }
        break;
    }
    default:
        return;
    }
}

#endif // CIOT_CONFIG_FEATURE_HTTP_SERVER == 1
