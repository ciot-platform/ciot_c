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
#include <ctype.h>

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
static bool ciot_http_server_get_header(const ciot_http_server_request_t *request, const char *name, char *value, size_t value_size);
static const char *ciot_http_server_status_text(uint16_t status_code);
static void ciot_http_server_write_response(ciot_http_server_t self, const ciot_http_server_response_t *response);
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
    ciot_http_server_response_t response = {
        .status_code = 200,
        .content_type = "application/octet-stream",
        .body = data,
        .body_size = size,
    };
    return ciot_http_server_send_response(self, &response);
}

ciot_err_t ciot_http_server_send_response(ciot_http_server_t self, const ciot_http_server_response_t *response)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(response);
    ciot_http_server_write_response(self, response);
    return CIOT_ERR_OK;
}

static void ciot_http_server_write_response(ciot_http_server_t self, const ciot_http_server_response_t *response)
{
    uint16_t status_code = response->status_code == 0 ? 200 : response->status_code;
    mg_printf(self->conn_tx, "HTTP/1.0 %u %s\r\n", status_code, ciot_http_server_status_text(status_code));
#ifdef CIOT_CONFIG_HTTP_SERVER_ALLOW_ORIGIN
    mg_printf(self->conn_tx, "Access-Control-Allow-Origin: " CIOT_CONFIG_HTTP_SERVER_ALLOW_ORIGIN "\r\n");
#endif
    mg_printf(self->conn_tx, "Content-Type: %s\r\n", response->content_type != NULL ? response->content_type : "application/octet-stream");
    for (size_t i = 0; i < response->headers_count && i < CIOT_HTTP_SERVER_MAX_RESPONSE_HEADERS; i++)
    {
        if (response->headers[i].name != NULL && response->headers[i].value != NULL)
        {
            mg_printf(self->conn_tx, "%s: %s\r\n", response->headers[i].name, response->headers[i].value);
        }
    }
    mg_printf(self->conn_tx, "Content-Length: %d\r\n\r\n", (int)response->body_size);
    if (response->body != NULL && response->body_size > 0) mg_send(self->conn_tx, response->body, response->body_size);
}

static const char *ciot_http_server_status_text(uint16_t status_code)
{
    switch (status_code)
    {
    case 200: return "OK";
    case 201: return "Created";
    case 204: return "No Content";
    case 400: return "Bad Request";
    case 401: return "Unauthorized";
    case 403: return "Forbidden";
    case 404: return "Not Found";
    case 409: return "Conflict";
    case 428: return "Precondition Required";
    case 429: return "Too Many Requests";
    default: return "Internal Server Error";
    }
}

static bool ciot_http_server_get_header(const ciot_http_server_request_t *request, const char *name, char *value, size_t value_size)
{
    if (request == NULL || name == NULL || value == NULL || value_size == 0) return false;
    struct mg_http_message *hm = request->context;
    size_t name_len = strlen(name);
    const struct mg_str *header = NULL;
    for (size_t i = 0; i < MG_MAX_HTTP_HEADERS && hm->headers[i].name.len > 0; i++)
    {
        if (hm->headers[i].name.len != name_len) continue;
        bool equal = true;
        for (size_t j = 0; j < name_len; j++)
        {
            if (tolower((unsigned char)hm->headers[i].name.buf[j]) != tolower((unsigned char)name[j]))
            {
                equal = false;
                break;
            }
        }
        if (equal)
        {
            header = &hm->headers[i].value;
            break;
        }
    }
    if (header == NULL) return false;
    size_t size = header->len < value_size - 1 ? header->len : value_size - 1;
    memcpy(value, header->buf, size);
    value[size] = '\0';
    return true;
}

static bool check_method(struct mg_http_message *hm, const char *method)
{
    size_t len = strlen(method);
    return hm->method.len == len && strncmp(hm->method.buf, method, len) == 0;
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
        ciot_http_server_request_t request = {
            .method = hm->method.buf,
            .method_len = hm->method.len,
            .uri = hm->uri.buf,
            .uri_len = hm->uri.len,
            .get_header = ciot_http_server_get_header,
            .context = hm,
        };
        if (!c->loc.is_ip6)
        {
            snprintf(request.local_address, sizeof(request.local_address), "%u.%u.%u.%u", c->loc.ip[0], c->loc.ip[1], c->loc.ip[2], c->loc.ip[3]);
        }
        if (!c->rem.is_ip6)
        {
            snprintf(request.remote_address, sizeof(request.remote_address), "%u.%u.%u.%u", c->rem.ip[0], c->rem.ip[1], c->rem.ip[2], c->rem.ip[3]);
        }
        if (base->request_interceptor != NULL)
        {
            ciot_http_server_response_t response = {0};
            if (base->request_interceptor(self, &request, &response, base->request_interceptor_args))
            {
                ciot_http_server_write_response(self, &response);
                break;
            }
        }
        if (mg_match(hm->uri, mg_str(base->cfg.route), NULL) && check_method(hm, "POST"))
        {
            ciot_iface_send_event_data(&base->iface, CIOT_EVENT_TYPE_MSG, (uint8_t *)hm->body.buf, hm->body.len);
        }
        else if(self->base.custom_api.enabled && mg_match(hm->uri, mg_str(self->base.custom_api.uri), NULL))
        {
            self->base.custom_api.handler(self, hm->uri.buf, hm->uri.len, hm->method.buf, (uint8_t *)hm->body.buf, hm->body.len, self->base.custom_api.args);
        }
        else if (self->base.upload_api.enabled && mg_match(hm->uri, mg_str(self->base.upload_api.uri), NULL) && check_method(hm, self->base.upload_api.method))
        {
            if (hm->body.len == 0 || hm->body.len > self->base.upload_api.max_size)
            {
                CIOT_LOGW(TAG, "Upload rejected: body.len %d, max %d", (int)hm->body.len, (int)self->base.upload_api.max_size);
                mg_printf(self->conn_tx, "HTTP/1.0 400 Bad Request\r\nContent-Length: 0\r\n\r\n");
            }
            else
            {
                ciot_err_t err = self->base.upload_api.handler(self->base.upload_api.args, (uint8_t *)hm->body.buf, hm->body.len);
                if (err == CIOT_ERR_OK)
                {
                    mg_printf(self->conn_tx, "HTTP/1.0 200 OK\r\nContent-Length: 0\r\n\r\n");
                }
                else
                {
                    const char *msg = ciot_err_to_message(err);
                    mg_printf(self->conn_tx, "HTTP/1.0 400 Bad Request\r\nContent-Length: %d\r\n\r\n%s", (int)strlen(msg), msg);
                }
            }
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
            ciot_http_server_event_data_t evt_data = {0};
            size_t uri_size = hm->uri.len;
            size_t body_size = hm->body.len;
            if (uri_size > sizeof(evt_data.uri)) uri_size = sizeof(evt_data.uri);
            if (body_size > sizeof(evt_data.body)) body_size = sizeof(evt_data.body);
            memcpy(evt_data.uri, hm->uri.buf, uri_size);
            memcpy(evt_data.body, hm->body.buf, body_size);
            ciot_iface_send_event_data(&base->iface, CIOT_EVENT_TYPE_DATA, (uint8_t *)&evt_data, sizeof(evt_data));
        }
        break;
    }
    default:
        return;
    }
}

#endif // CIOT_CONFIG_FEATURE_HTTP_SERVER == 1
