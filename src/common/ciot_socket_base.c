/**
 * @file ciot_socket_base.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2026-08-24
 *
 * @copyright Copyright (c) 2026
 *
 */

#include "ciot_config.h"

#if CIOT_CONFIG_FEATURE_SOCKET == 1

#include "ciot_socket.h"
#include "ciot_types.h"

static ciot_err_t ciot_socket_process_data(ciot_iface_t *iface, ciot_msg_data_t *data);
static ciot_err_t ciot_socket_get_data(ciot_iface_t *iface, ciot_msg_data_t *data);
static ciot_err_t ciot_socket_send_data(ciot_iface_t *iface, uint8_t *data, int size);
static ciot_err_t ciot_socket_read_data(ciot_iface_t *iface, uint8_t *data, int size);

ciot_err_t ciot_socket_init(ciot_socket_t self)
{
    ciot_socket_base_t *base = (ciot_socket_base_t *)self;

    base->iface.ptr = self;
    base->iface.process_data = ciot_socket_process_data;
    base->iface.get_data = ciot_socket_get_data;
    base->iface.send_data = ciot_socket_send_data;
    base->iface.read_data = ciot_socket_read_data;
    base->iface.info.type = CIOT_IFACE_TYPE_SOCKET;

    return CIOT_ERR_OK;
}

ciot_err_t ciot_socket_get_status(ciot_socket_t self, ciot_socket_status_t *status)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(status);
    ciot_socket_base_t *base = (ciot_socket_base_t *)self;
    *status = base->status;
    return CIOT_ERR_OK;
}

/**
 * ciot_socket has no protobuf message of its own: it is only ever driven internally
 * (today, by ciot_mbus_client/server's TCP config), never addressed directly from the
 * wire. process_data only supports the generic "common/stop" request so it can still
 * be stopped remotely if it is ever exposed as a top-level device iface.
 */
static ciot_err_t ciot_socket_process_data(ciot_iface_t *iface, ciot_msg_data_t *data)
{
    ciot_socket_t self = iface->ptr;

    if (data->which_type == CIOT_MSG_DATA_COMMON_TAG &&
        data->common.which_type == CIOT_COMMON_STOP_TAG)
    {
        return ciot_socket_stop(self);
    }

    return CIOT_ERR_NOT_SUPPORTED;
}

/**
 * ciot_iface_send_event_type() requires get_data() to succeed for CIOT_DATA_TYPE_STATUS
 * before it will deliver the STARTED/STOPPED event, so this must return CIOT_ERR_OK even
 * though there is no socket-specific status message defined in the protobuf schema.
 */
static ciot_err_t ciot_socket_get_data(ciot_iface_t *iface, ciot_msg_data_t *data)
{
    CIOT_ERR_TYPE_CHECK(data->which_type, CIOT_MSG_DATA_GET_DATA_TAG);

    data->which_type = CIOT_MSG_DATA_COMMON_TAG;
    data->common.which_type = 0;

    return CIOT_ERR_OK;
}

static ciot_err_t ciot_socket_send_data(ciot_iface_t *iface, uint8_t *data, int size)
{
    CIOT_ERR_NULL_CHECK(iface);
    CIOT_ERR_NULL_CHECK(data);
    return ciot_socket_send_bytes((ciot_socket_t)iface, data, size);
}

static ciot_err_t ciot_socket_read_data(ciot_iface_t *iface, uint8_t *data, int size)
{
    CIOT_ERR_NULL_CHECK(iface);
    CIOT_ERR_NULL_CHECK(data);
    return ciot_socket_read_bytes((ciot_socket_t)iface, data, size);
}

#endif // CIOT_CONFIG_FEATURE_SOCKET == 1
