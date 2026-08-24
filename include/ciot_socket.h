/**
 * @file ciot_socket.h
 * @author your name (you@domain.com)
 * @brief Generic raw TCP byte-stream transport (ciot_iface_t over BSD/lwIP sockets).
 * @version 0.1
 * @date 2026-08-24
 *
 * @copyright Copyright (c) 2026
 *
 */

#ifndef __CIOT_SOCKET__H__
#define __CIOT_SOCKET__H__

#ifdef __cplusplus
extern "C" {
#endif

#include "ciot_types.h"
#include "ciot_iface.h"

typedef struct ciot_socket *ciot_socket_t;

typedef enum ciot_socket_role
{
    CIOT_SOCKET_ROLE_NONE = 0,
    CIOT_SOCKET_ROLE_CLIENT,
    CIOT_SOCKET_ROLE_SERVER,
} ciot_socket_role_t;

typedef enum ciot_socket_state
{
    CIOT_SOCKET_STATE_STOPPED = 0,
    CIOT_SOCKET_STATE_LISTENING,
    CIOT_SOCKET_STATE_CONNECTED,
    CIOT_SOCKET_STATE_ERROR,
} ciot_socket_state_t;

typedef struct ciot_socket_status
{
    ciot_socket_role_t role;
    ciot_socket_state_t state;
    ciot_err_t error;
} ciot_socket_status_t;

typedef struct ciot_socket_base
{
    ciot_iface_t iface;
    ciot_socket_status_t status;
} ciot_socket_base_t;

ciot_socket_t ciot_socket_new(void *handle);
ciot_err_t ciot_socket_init(ciot_socket_t self);
ciot_err_t ciot_socket_start_server(ciot_socket_t self, uint16_t port, int32_t timeout_ms);
ciot_err_t ciot_socket_start_client(ciot_socket_t self, const uint8_t ip[4], uint16_t port, int32_t timeout_ms);
ciot_err_t ciot_socket_stop(ciot_socket_t self);
ciot_err_t ciot_socket_task(ciot_socket_t self);
ciot_err_t ciot_socket_get_status(ciot_socket_t self, ciot_socket_status_t *status);
ciot_err_t ciot_socket_send_bytes(ciot_socket_t self, uint8_t *data, int size);
ciot_err_t ciot_socket_read_bytes(ciot_socket_t self, uint8_t *data, int size);

#ifdef __cplusplus
}
#endif

#endif  //!__CIOT_SOCKET__H__
