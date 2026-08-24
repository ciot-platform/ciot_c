/**
 * @file ciot_socket.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2026-08-24
 *
 * @copyright Copyright (c) 2026
 *
 */

#include "ciot_config.h"

#if CIOT_CONFIG_FEATURE_SOCKET == 1 && defined(CIOT_PLATFORM_ESP32)

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#include "ciot_socket.h"
#include "ciot_err.h"

#include "lwip/sockets.h"

static const char *TAG = "ciot_socket";

struct ciot_socket
{
    ciot_socket_base_t base;
    int listen_fd;
    int conn_fd;
    uint16_t port;
    int32_t timeout_ms;
};

static void ciot_socket_apply_timeout(int fd, int32_t timeout_ms);
static void ciot_socket_accept(ciot_socket_t self);
static void ciot_socket_on_disconnect(ciot_socket_t self);

ciot_socket_t ciot_socket_new(void *handle)
{
    ciot_socket_t self = calloc(1, sizeof(struct ciot_socket));
    ciot_socket_init(self);
    self->listen_fd = -1;
    self->conn_fd = -1;
    return self;
}

ciot_err_t ciot_socket_start_server(ciot_socket_t self, uint16_t port, int32_t timeout_ms)
{
    CIOT_ERR_NULL_CHECK(self);

    if (self->base.status.state == CIOT_SOCKET_STATE_LISTENING ||
        self->base.status.state == CIOT_SOCKET_STATE_CONNECTED)
    {
        if (self->port == port)
        {
            CIOT_LOGI(TAG, "Socket server already listening on port %d", (int)port);
            return CIOT_ERR_OK;
        }
        ciot_socket_stop(self);
    }

    self->port = port;
    self->timeout_ms = timeout_ms;
    self->base.status.role = CIOT_SOCKET_ROLE_SERVER;

    self->listen_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (self->listen_fd < 0)
    {
        CIOT_LOGE(TAG, "socket() failed: errno %d", errno);
        self->base.status.state = CIOT_SOCKET_STATE_ERROR;
        return CIOT_ERR_FAIL;
    }

    int reuse = 1;
    setsockopt(self->listen_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = htonl(INADDR_ANY),
        .sin_port = htons(port),
    };

    if (bind(self->listen_fd, (struct sockaddr *)&addr, sizeof(addr)) != 0)
    {
        CIOT_LOGE(TAG, "bind() on port %d failed: errno %d", (int)port, errno);
        close(self->listen_fd);
        self->listen_fd = -1;
        self->base.status.state = CIOT_SOCKET_STATE_ERROR;
        return CIOT_ERR_FAIL;
    }

    if (listen(self->listen_fd, 1) != 0)
    {
        CIOT_LOGE(TAG, "listen() failed: errno %d", errno);
        close(self->listen_fd);
        self->listen_fd = -1;
        self->base.status.state = CIOT_SOCKET_STATE_ERROR;
        return CIOT_ERR_FAIL;
    }

    fcntl(self->listen_fd, F_SETFL, O_NONBLOCK);

    self->base.status.state = CIOT_SOCKET_STATE_LISTENING;
    self->base.status.error = CIOT_ERR_OK;
    CIOT_LOGI(TAG, "Listening for Modbus TCP connections on port %d", (int)port);

    return CIOT_ERR_OK;
}

ciot_err_t ciot_socket_start_client(ciot_socket_t self, const uint8_t ip[4], uint16_t port, int32_t timeout_ms)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(ip);

    if (self->base.status.state == CIOT_SOCKET_STATE_CONNECTED)
    {
        return CIOT_ERR_OK;
    }

    self->port = port;
    self->timeout_ms = timeout_ms;
    self->base.status.role = CIOT_SOCKET_ROLE_CLIENT;

    self->conn_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (self->conn_fd < 0)
    {
        CIOT_LOGE(TAG, "socket() failed: errno %d", errno);
        self->base.status.state = CIOT_SOCKET_STATE_ERROR;
        ciot_iface_send_event_type(&self->base.iface, CIOT_EVENT_TYPE_ERROR);
        return CIOT_ERR_FAIL;
    }

    ciot_socket_apply_timeout(self->conn_fd, timeout_ms);

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
    };
    memcpy(&addr.sin_addr.s_addr, ip, sizeof(addr.sin_addr.s_addr));

    if (connect(self->conn_fd, (struct sockaddr *)&addr, sizeof(addr)) != 0)
    {
        CIOT_LOGW(TAG, "connect() to %d.%d.%d.%d:%d failed: errno %d", ip[0], ip[1], ip[2], ip[3], (int)port, errno);
        close(self->conn_fd);
        self->conn_fd = -1;
        self->base.status.state = CIOT_SOCKET_STATE_ERROR;
        self->base.status.error = CIOT_ERR_FAIL;
        ciot_iface_send_event_type(&self->base.iface, CIOT_EVENT_TYPE_ERROR);
        return CIOT_ERR_FAIL;
    }

    self->base.status.state = CIOT_SOCKET_STATE_CONNECTED;
    self->base.status.error = CIOT_ERR_OK;
    CIOT_LOGI(TAG, "Connected to Modbus TCP server %d.%d.%d.%d:%d", ip[0], ip[1], ip[2], ip[3], (int)port);
    ciot_iface_send_event_type(&self->base.iface, CIOT_EVENT_TYPE_STARTED);

    return CIOT_ERR_OK;
}

ciot_err_t ciot_socket_stop(ciot_socket_t self)
{
    CIOT_ERR_NULL_CHECK(self);

    if (self->conn_fd >= 0)
    {
        close(self->conn_fd);
        self->conn_fd = -1;
    }
    if (self->listen_fd >= 0)
    {
        close(self->listen_fd);
        self->listen_fd = -1;
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

    if (self->base.status.role == CIOT_SOCKET_ROLE_SERVER &&
        self->base.status.state == CIOT_SOCKET_STATE_LISTENING)
    {
        ciot_socket_accept(self);
    }

    return CIOT_ERR_OK;
}

ciot_err_t ciot_socket_send_bytes(ciot_socket_t self, uint8_t *data, int size)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(data);

    if (self->base.status.state != CIOT_SOCKET_STATE_CONNECTED)
    {
        return CIOT_ERR_INVALID_STATE;
    }

    int sent = 0;
    while (sent < size)
    {
        int n = send(self->conn_fd, data + sent, size - sent, 0);
        if (n <= 0)
        {
            if (n < 0 && (errno == EWOULDBLOCK || errno == EAGAIN))
            {
                break; // byte timeout expired; nanomodbus treats a partial transfer as a timeout
            }
            CIOT_LOGW(TAG, "send() failed: errno %d", errno);
            ciot_socket_on_disconnect(self);
            return CIOT_ERR_FAIL;
        }
        sent += n;
    }

    return sent == size ? CIOT_ERR_OK : CIOT_ERR_TIMEOUT;
}

ciot_err_t ciot_socket_read_bytes(ciot_socket_t self, uint8_t *data, int size)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(data);

    if (self->base.status.state != CIOT_SOCKET_STATE_CONNECTED)
    {
        return CIOT_ERR_INVALID_STATE;
    }

    int received = 0;
    while (received < size)
    {
        int n = recv(self->conn_fd, data + received, size - received, 0);
        if (n == 0)
        {
            CIOT_LOGI(TAG, "Peer closed the TCP connection");
            ciot_socket_on_disconnect(self);
            return CIOT_ERR_FAIL;
        }
        if (n < 0)
        {
            if (errno == EWOULDBLOCK || errno == EAGAIN)
            {
                break; // byte timeout expired; nanomodbus treats a partial transfer as a timeout
            }
            CIOT_LOGW(TAG, "recv() failed: errno %d", errno);
            ciot_socket_on_disconnect(self);
            return CIOT_ERR_FAIL;
        }
        received += n;
    }

    return received == size ? CIOT_ERR_OK : CIOT_ERR_TIMEOUT;
}

static void ciot_socket_apply_timeout(int fd, int32_t timeout_ms)
{
    struct timeval tv = {
        .tv_sec = timeout_ms / 1000,
        .tv_usec = (timeout_ms % 1000) * 1000,
    };
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
}

static void ciot_socket_accept(ciot_socket_t self)
{
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    int fd = accept(self->listen_fd, (struct sockaddr *)&client_addr, &addr_len);
    if (fd < 0)
    {
        if (errno != EWOULDBLOCK && errno != EAGAIN)
        {
            CIOT_LOGW(TAG, "accept() failed: errno %d", errno);
        }
        return;
    }

    ciot_socket_apply_timeout(fd, self->timeout_ms);
    self->conn_fd = fd;
    self->base.status.state = CIOT_SOCKET_STATE_CONNECTED;
    self->base.status.error = CIOT_ERR_OK;
    CIOT_LOGI(TAG, "Modbus TCP client connected");
    ciot_iface_send_event_type(&self->base.iface, CIOT_EVENT_TYPE_STARTED);
}

static void ciot_socket_on_disconnect(ciot_socket_t self)
{
    if (self->conn_fd >= 0)
    {
        close(self->conn_fd);
        self->conn_fd = -1;
    }

    self->base.status.state = self->base.status.role == CIOT_SOCKET_ROLE_SERVER
                                   ? CIOT_SOCKET_STATE_LISTENING
                                   : CIOT_SOCKET_STATE_STOPPED;

    ciot_iface_send_event_type(&self->base.iface, CIOT_EVENT_TYPE_STOPPED);
}

#endif //! CIOT_CONFIG_FEATURE_SOCKET == 1 && defined(CIOT_PLATFORM_ESP32)
