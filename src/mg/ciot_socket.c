/**
 * @file ciot_socket.c
 * @author your name (you@domain.com)
 * @brief Mongoose driver for ciot_socket: a generic raw TCP byte-stream
 *        transport. Not specific to any protocol carried over it (e.g.
 *        Modbus TCP) - it only moves bytes, framing/parsing is the caller's
 *        job. Covers every platform that already links Mongoose in this
 *        codebase (Windows, Linux, and any future Mongoose-capable MCU port
 *        that wires src/mg into its build).
 * @version 0.1
 * @date 2026-08-26
 *
 * @copyright Copyright (c) 2026
 *
 */

#include "ciot_config.h"

#if CIOT_CONFIG_FEATURE_SOCKET == 1 && defined(CIOT_PLATFORM_MONGOOSE)

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "ciot_socket.h"
#include "ciot_err.h"
#include "mongoose.h"

#ifndef CIOT_CONFIG_SOCKET_DEFAULT_TIMEOUT_MS
#define CIOT_CONFIG_SOCKET_DEFAULT_TIMEOUT_MS 1000
#endif

static const char *TAG = "ciot_socket";

struct ciot_socket
{
    ciot_socket_base_t base;
    struct mg_mgr *mgr;
    struct mg_connection *listener;
    struct mg_connection *conn;
    uint16_t port;
    int32_t timeout_ms;
};

static void ciot_socket_event_handler(struct mg_connection *c, int ev, void *ev_data);
static uint64_t ciot_socket_deadline(ciot_socket_t self);

ciot_socket_t ciot_socket_new(void *handle)
{
    ciot_socket_t self = calloc(1, sizeof(struct ciot_socket));
    ciot_socket_init(self);
    self->mgr = handle;
    return self;
}

ciot_err_t ciot_socket_start_server(ciot_socket_t self, uint16_t port, int32_t timeout_ms)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(self->mgr);

    if ((self->base.status.state == CIOT_SOCKET_STATE_LISTENING ||
         self->base.status.state == CIOT_SOCKET_STATE_CONNECTED) &&
        self->port == port)
    {
        CIOT_LOGI(TAG, "Socket server already listening on port %d", (int)port);
        return CIOT_ERR_OK;
    }

    if (self->listener != NULL || self->conn != NULL)
    {
        ciot_socket_stop(self);
    }

    self->port = port;
    self->timeout_ms = timeout_ms;
    self->base.status.role = CIOT_SOCKET_ROLE_SERVER;

    char url[24];
    snprintf(url, sizeof(url), "tcp://0.0.0.0:%d", (int)port);

    self->listener = mg_listen(self->mgr, url, ciot_socket_event_handler, self);
    if (self->listener == NULL)
    {
        CIOT_LOGE(TAG, "mg_listen(%s) failed", url);
        self->base.status.state = CIOT_SOCKET_STATE_ERROR;
        return CIOT_ERR_FAIL;
    }

    self->base.status.state = CIOT_SOCKET_STATE_LISTENING;
    self->base.status.error = CIOT_ERR_OK;
    CIOT_LOGI(TAG, "Listening for TCP connections on port %d", (int)port);

    return CIOT_ERR_OK;
}

ciot_err_t ciot_socket_start_client(ciot_socket_t self, const uint8_t ip[4], uint16_t port, int32_t timeout_ms)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(ip);
    CIOT_ERR_NULL_CHECK(self->mgr);

    if (self->base.status.state == CIOT_SOCKET_STATE_CONNECTED)
    {
        return CIOT_ERR_OK;
    }

    self->port = port;
    self->timeout_ms = timeout_ms;
    self->base.status.role = CIOT_SOCKET_ROLE_CLIENT;
    /* Neutral placeholder distinct from CONNECTED/ERROR while the async
     * connect below is in flight - avoids the wait loop mistaking a
     * leftover ERROR/STOPPED state from a previous attempt as "done". */
    self->base.status.state = CIOT_SOCKET_STATE_STOPPED;

    char url[32];
    snprintf(url, sizeof(url), "tcp://%d.%d.%d.%d:%d", ip[0], ip[1], ip[2], ip[3], (int)port);

    self->conn = mg_connect(self->mgr, url, ciot_socket_event_handler, self);
    if (self->conn == NULL)
    {
        CIOT_LOGW(TAG, "mg_connect(%s) failed", url);
        self->base.status.state = CIOT_SOCKET_STATE_ERROR;
        ciot_iface_send_event_type(&self->base.iface, CIOT_EVENT_TYPE_ERROR);
        return CIOT_ERR_FAIL;
    }

    /* mg_connect() only kicks off an async connection; block here (pumping
     * the shared mg_mgr) until it resolves, so callers keep the same
     * "returns already connected or failed" contract as the other backend. */
    uint64_t deadline = ciot_socket_deadline(self);
    while (self->base.status.state != CIOT_SOCKET_STATE_CONNECTED &&
           self->base.status.state != CIOT_SOCKET_STATE_ERROR &&
           mg_millis() < deadline)
    {
        mg_mgr_poll(self->mgr, 10);
    }

    if (self->base.status.state != CIOT_SOCKET_STATE_CONNECTED)
    {
        CIOT_LOGW(TAG, "connect() to %s timed out", url);
        if (self->conn != NULL)
        {
            self->conn->is_closing = 1;
            self->conn->fn_data = NULL;
            self->conn = NULL;
        }
        self->base.status.state = CIOT_SOCKET_STATE_ERROR;
        return CIOT_ERR_TIMEOUT;
    }

    return CIOT_ERR_OK;
}

ciot_err_t ciot_socket_stop(ciot_socket_t self)
{
    CIOT_ERR_NULL_CHECK(self);

    if (self->conn != NULL)
    {
        /* Detach first so our own MG_EV_CLOSE handling below doesn't also
         * fire a redundant STOPPED event for this same stop() call. */
        self->conn->fn_data = NULL;
        mg_close_conn(self->conn);
        self->conn = NULL;
    }
    if (self->listener != NULL)
    {
        self->listener->fn_data = NULL;
        mg_close_conn(self->listener);
        self->listener = NULL;
    }

    if (self->base.status.state == CIOT_SOCKET_STATE_STOPPED)
    {
        return CIOT_ERR_OK;
    }

    self->base.status.state = CIOT_SOCKET_STATE_STOPPED;
    ciot_iface_send_event_type(&self->base.iface, CIOT_EVENT_TYPE_STOPPED);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_socket_task(ciot_socket_t self)
{
    CIOT_ERR_NULL_CHECK(self);
    /* No-op: mg_mgr_poll() is already driven centrally by ciot_task() every
     * tick, and accept/connect/close are all handled asynchronously through
     * that shared event loop and our event handler below. */
    return CIOT_ERR_OK;
}

ciot_err_t ciot_socket_send_bytes(ciot_socket_t self, uint8_t *data, int size)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(data);

    if (self->conn == NULL || self->base.status.state != CIOT_SOCKET_STATE_CONNECTED)
    {
        return CIOT_ERR_INVALID_STATE;
    }

    if (!mg_send(self->conn, data, (size_t)size))
    {
        return CIOT_ERR_FAIL;
    }

    /* mg_send() only queues into c->send; block (pumping the shared mg_mgr)
     * until the queue actually drains, so this keeps the same all-or-nothing
     * contract as ciot_uart_send_bytes()/ciot_iface_send_bytes() expect from
     * any ciot_iface_t transport - not just Modbus. */
    uint64_t deadline = ciot_socket_deadline(self);
    while (self->conn != NULL && self->conn->send.len > 0 && mg_millis() < deadline)
    {
        mg_mgr_poll(self->mgr, 10);
    }

    if (self->conn == NULL)
    {
        return CIOT_ERR_FAIL; // disconnected while flushing
    }

    return self->conn->send.len == 0 ? CIOT_ERR_OK : CIOT_ERR_TIMEOUT;
}

ciot_err_t ciot_socket_read_bytes(ciot_socket_t self, uint8_t *data, int size)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(data);

    if (self->conn == NULL || self->base.status.state != CIOT_SOCKET_STATE_CONNECTED)
    {
        return CIOT_ERR_INVALID_STATE;
    }

    /* Same all-or-nothing contract as ciot_uart_read_bytes()/ciot_iface_read_bytes():
     * only returns CIOT_ERR_OK once all `size` bytes have arrived, blocking
     * (by pumping the shared mg_mgr) until then or until the timeout expires. */
    uint64_t deadline = ciot_socket_deadline(self);
    while (self->conn != NULL && self->conn->recv.len < (size_t)size && mg_millis() < deadline)
    {
        mg_mgr_poll(self->mgr, 10);
    }

    if (self->conn == NULL || self->conn->recv.len < (size_t)size)
    {
        return CIOT_ERR_TIMEOUT;
    }

    memcpy(data, self->conn->recv.buf, (size_t)size);
    mg_iobuf_del(&self->conn->recv, 0, (size_t)size);

    return CIOT_ERR_OK;
}

static uint64_t ciot_socket_deadline(ciot_socket_t self)
{
    int32_t timeout_ms = self->timeout_ms > 0 ? self->timeout_ms : CIOT_CONFIG_SOCKET_DEFAULT_TIMEOUT_MS;
    return mg_millis() + (uint64_t)timeout_ms;
}

/**
 * Non-blocking peek at how many bytes are already buffered for this connection,
 * mirroring ciot_uart_available(). Callers (ciot_mbus_server_task()) use this to
 * avoid entering ciot_socket_read_bytes()'s polling loop - and its timeout - when
 * there is nothing to read yet. Mongoose already accumulates arrived bytes into
 * c->recv asynchronously via the shared mg_mgr_poll(), so this is a plain field read,
 * no extra polling needed.
 */
size_t ciot_socket_available(ciot_socket_t self)
{
    if (self == NULL || self->conn == NULL || self->base.status.state != CIOT_SOCKET_STATE_CONNECTED)
    {
        return 0;
    }

    return self->conn->recv.len;
}

static void ciot_socket_event_handler(struct mg_connection *c, int ev, void *ev_data)
{
    ciot_socket_t self = (ciot_socket_t)c->fn_data;
    if (self == NULL)
    {
        return; // detached by ciot_socket_stop(); ignore any events still in flight for it
    }

    mg_event_t mg_ev = ev;

    switch (mg_ev)
    {
    case MG_EV_ACCEPT:
    {
        if (self->conn != NULL)
        {
            CIOT_LOGW(TAG, "rejecting extra connection, only one at a time is supported");
            c->is_closing = 1;
            break;
        }
        self->conn = c;
        self->base.status.state = CIOT_SOCKET_STATE_CONNECTED;
        self->base.status.error = CIOT_ERR_OK;
        CIOT_LOGI(TAG, "TCP client connected");
        ciot_iface_send_event_type(&self->base.iface, CIOT_EVENT_TYPE_STARTED);
        break;
    }
    case MG_EV_CONNECT:
    {
        if (c != self->conn)
        {
            break;
        }
        self->base.status.state = CIOT_SOCKET_STATE_CONNECTED;
        self->base.status.error = CIOT_ERR_OK;
        CIOT_LOGI(TAG, "Connected to TCP server");
        ciot_iface_send_event_type(&self->base.iface, CIOT_EVENT_TYPE_STARTED);
        break;
    }
    case MG_EV_ERROR:
    {
        CIOT_LOGW(TAG, "MG_EV_ERROR: %s", ev_data ? (char *)ev_data : "");
        if (c == self->conn || c == self->listener)
        {
            self->base.status.state = CIOT_SOCKET_STATE_ERROR;
            self->base.status.error = CIOT_ERR_FAIL;
        }
        break;
    }
    case MG_EV_CLOSE:
    {
        if (c == self->conn)
        {
            self->conn = NULL;
            self->base.status.state = self->base.status.role == CIOT_SOCKET_ROLE_SERVER
                                           ? CIOT_SOCKET_STATE_LISTENING
                                           : CIOT_SOCKET_STATE_STOPPED;
            ciot_iface_send_event_type(&self->base.iface, CIOT_EVENT_TYPE_STOPPED);
        }
        else if (c == self->listener)
        {
            self->listener = NULL;
        }
        break;
    }
    default:
        break;
    }
}

#endif // CIOT_CONFIG_FEATURE_SOCKET == 1 && defined(CIOT_PLATFORM_MONGOOSE)
