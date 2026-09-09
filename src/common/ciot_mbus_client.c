/**
 * @file ciot_mbus_client.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-12-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "ciot_config.h"
 
#if CIOT_CONFIG_FEATURE_MBUS_CLIENT == 1 

#include "ciot_mbus_client.h"
#include "ciot_socket.h"
#include <stdlib.h>

struct ciot_mbus_client
{
    ciot_mbus_client_base_t base;
    ciot_iface_t *iface;
    nmbs_t nmbs;
};

static const char *TAG = "ciot_mbus_client";

static int32_t ciot_mbus_client_read(uint8_t *buf, uint16_t count, int32_t byte_timeout_ms, void *arg);
static int32_t ciot_mbus_client_write(const uint8_t *buf, uint16_t count, int32_t byte_timeout_ms, void *arg);
static void ciot_mbus_client_ensure_connected(ciot_mbus_client_t self);

ciot_mbus_client_t ciot_mbus_client_new(void *handle, ciot_iface_t *iface)
{
    ciot_mbus_client_t self = calloc(1, sizeof(struct ciot_mbus_client));
    ciot_mbus_client_init(self);
    self->iface = iface;
    return self;
}

ciot_err_t ciot_mbus_client_start(ciot_mbus_client_t self, ciot_mbus_client_cfg_t *cfg)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(cfg);

    self->base.cfg = *cfg;

    nmbs_platform_conf platform_conf;
    nmbs_platform_conf_create(&platform_conf);
    platform_conf.read = ciot_mbus_client_read;
    platform_conf.write = ciot_mbus_client_write;
    platform_conf.arg = self;

    nmbs_error err = 0;
    switch (cfg->which_type)
    {
    case CIOT_MBUS_CLIENT_CFG_RTU_TAG:
        platform_conf.transport = NMBS_TRANSPORT_RTU;
        err = nmbs_client_create(&self->nmbs, &platform_conf);
        nmbs_set_destination_rtu_address(&self->nmbs, self->base.cfg.rtu.server_id);
        break;
    case CIOT_MBUS_CLIENT_CFG_TCP_TAG:
        platform_conf.transport = NMBS_TRANSPORT_TCP;
        /*
         * cfg->tcp.ip is a nanopb CALLBACK bytes field (the .proto has no max_size
         * hint for it), so it is only ever populated when the app builds this cfg
         * as a static C literal and points .arg directly at a 4-byte buffer.
         * Decoding this field from a real wire/storage protobuf message is not
         * supported yet: pb_default_field_callback() no-ops unless a custom
         * ip.funcs.decode is installed, which nothing in this codebase does.
         */
        if (cfg->tcp.ip.arg == NULL)
        {
            CIOT_LOGE(TAG, "TCP client config missing server IP (tcp.ip.arg); only statically configured IPs are supported today");
            return CIOT_ERR_INVALID_ARG;
        }
        CIOT_ERR_RETURN(ciot_socket_start_client((ciot_socket_t)self->iface, (const uint8_t *)cfg->tcp.ip.arg, (uint16_t)cfg->tcp.port, (int32_t)cfg->timeout));
        err = nmbs_client_create(&self->nmbs, &platform_conf);
        break;
    default:
        return CIOT_ERR_INVALID_ARG;
    }

    if (err == NMBS_ERROR_NONE)
    {
        self->base.status.state = CIOT_MBUS_SERVER_STATE_STARTED;
        ciot_iface_send_event_type(&self->base.iface, CIOT_EVENT_TYPE_STARTED);
    }
    else
    {
        self->base.status.state = CIOT_MBUS_SERVER_STATE_ERROR;
        ciot_iface_send_event_type(&self->base.iface, CIOT_EVENT_TYPE_ERROR);
    }

    nmbs_set_read_timeout(&self->nmbs, self->base.cfg.timeout);
    nmbs_set_byte_timeout(&self->nmbs, self->base.cfg.timeout);

    return ciot_mbus_get_error(err);
}

ciot_err_t ciot_mbus_client_stop(ciot_mbus_client_t self)
{
    CIOT_ERR_NULL_CHECK(self);
    return CIOT_ERR_NOT_IMPLEMENTED;
}

ciot_err_t ciot_mbus_client_send_bytes(ciot_mbus_client_t self, uint8_t *data, int size)
{
    return CIOT_ERR_NOT_IMPLEMENTED;
}

static ciot_err_t ciot_mbus_client_process_request_result(ciot_mbus_client_t self, nmbs_error error)
{
    ciot_err_t err = ciot_mbus_get_error(error);
    if(err == CIOT_ERR_OK)
    {
        self->base.status.requests_success++;
    } else {
        self->base.status.requests_error++;
        self->base.status.error = err;
    }
    return err;
}

ciot_err_t ciot_mbus_client_read_coils(ciot_mbus_client_t self, uint16_t address, uint16_t quantity, nmbs_bitfield coils_out)
{
    CIOT_ERR_STATE_CHECK(self->base.status.state, CIOT_MBUS_CLIENT_STATE_STARTED);
    ciot_mbus_client_ensure_connected(self);
    CIOT_ERR_STATE_CHECK(self->iface->state, CIOT_IFACE_STATE_STARTED);
    nmbs_error error = nmbs_read_coils(&self->nmbs, address, quantity, coils_out);
    return ciot_mbus_client_process_request_result(self, error);
}

ciot_err_t ciot_mbus_client_read_discrete_inputs(ciot_mbus_client_t self, uint16_t address, uint16_t quantity, nmbs_bitfield inputs_out)
{
    CIOT_ERR_STATE_CHECK(self->base.status.state, CIOT_MBUS_CLIENT_STATE_STARTED);
    ciot_mbus_client_ensure_connected(self);
    CIOT_ERR_STATE_CHECK(self->iface->state, CIOT_IFACE_STATE_STARTED);
    nmbs_error error = nmbs_read_discrete_inputs(&self->nmbs, address, quantity, inputs_out);
    return ciot_mbus_client_process_request_result(self, error);
}

ciot_err_t ciot_mbus_client_read_holding_registers(ciot_mbus_client_t self, uint16_t address, uint16_t quantity, uint16_t* registers_out)
{
    CIOT_ERR_STATE_CHECK(self->base.status.state, CIOT_MBUS_CLIENT_STATE_STARTED);
    ciot_mbus_client_ensure_connected(self);
    CIOT_ERR_STATE_CHECK(self->iface->state, CIOT_IFACE_STATE_STARTED);
    nmbs_error error = nmbs_read_holding_registers(&self->nmbs, address, quantity, registers_out);
    return ciot_mbus_client_process_request_result(self, error);
}

ciot_err_t ciot_mbus_client_read_input_registers(ciot_mbus_client_t self, uint16_t address, uint16_t quantity, uint16_t* registers_out)
{
    CIOT_ERR_STATE_CHECK(self->base.status.state, CIOT_MBUS_CLIENT_STATE_STARTED);
    ciot_mbus_client_ensure_connected(self);
    CIOT_ERR_STATE_CHECK(self->iface->state, CIOT_IFACE_STATE_STARTED);
    nmbs_error error = nmbs_read_input_registers(&self->nmbs, address, quantity, registers_out);
    return ciot_mbus_client_process_request_result(self, error);
}

ciot_err_t ciot_mbus_client_write_single_coil(ciot_mbus_client_t self, uint16_t address, bool value)
{
    CIOT_ERR_STATE_CHECK(self->base.status.state, CIOT_MBUS_CLIENT_STATE_STARTED);
    ciot_mbus_client_ensure_connected(self);
    CIOT_ERR_STATE_CHECK(self->iface->state, CIOT_IFACE_STATE_STARTED);
    nmbs_error error = nmbs_write_single_coil(&self->nmbs, address, value);
    return ciot_mbus_client_process_request_result(self, error);
}

ciot_err_t ciot_mbus_client_write_single_register(ciot_mbus_client_t self, uint16_t address, uint16_t value)
{
    CIOT_ERR_STATE_CHECK(self->base.status.state, CIOT_MBUS_CLIENT_STATE_STARTED);
    ciot_mbus_client_ensure_connected(self);
    CIOT_ERR_STATE_CHECK(self->iface->state, CIOT_IFACE_STATE_STARTED);
    nmbs_error error = nmbs_write_single_register(&self->nmbs, address, value);
    return ciot_mbus_client_process_request_result(self, error);
}

ciot_err_t ciot_mbus_client_write_multiple_coils(ciot_mbus_client_t self, uint16_t address, uint16_t quantity, const nmbs_bitfield coils)
{
    CIOT_ERR_STATE_CHECK(self->base.status.state, CIOT_MBUS_CLIENT_STATE_STARTED);
    ciot_mbus_client_ensure_connected(self);
    CIOT_ERR_STATE_CHECK(self->iface->state, CIOT_IFACE_STATE_STARTED);
    nmbs_error error = nmbs_write_multiple_coils(&self->nmbs, address, quantity, coils);
    return ciot_mbus_client_process_request_result(self, error);
}

ciot_err_t ciot_mbus_client_write_multiple_registers(ciot_mbus_client_t self, uint16_t address, uint16_t quantity, const uint16_t* registers)
{
    CIOT_ERR_STATE_CHECK(self->base.status.state, CIOT_MBUS_CLIENT_STATE_STARTED);
    ciot_mbus_client_ensure_connected(self);
    CIOT_ERR_STATE_CHECK(self->iface->state, CIOT_IFACE_STATE_STARTED);
    nmbs_error error = nmbs_write_multiple_registers(&self->nmbs, address, quantity, registers);
    return ciot_mbus_client_process_request_result(self, error);
}

/**
 * TCP is not always-on like RS-485: the socket can drop (peer closed it, network
 * blip, etc), which flips self->iface->state to CIOT_IFACE_STATE_STOPPED. Client
 * requests are synchronous (no _task() to run reconnection in the background), so
 * every public request function calls this first to transparently reconnect before
 * the state check below would otherwise reject the request.
 */
static void ciot_mbus_client_ensure_connected(ciot_mbus_client_t self)
{
    if (self->base.cfg.which_type != CIOT_MBUS_CLIENT_CFG_TCP_TAG)
    {
        return;
    }
    if (self->iface->state == CIOT_IFACE_STATE_STARTED)
    {
        return;
    }
    if (self->base.cfg.tcp.ip.arg == NULL)
    {
        return;
    }
    ciot_socket_start_client((ciot_socket_t)self->iface, (const uint8_t *)self->base.cfg.tcp.ip.arg,
                              (uint16_t)self->base.cfg.tcp.port, (int32_t)self->base.cfg.timeout);
}

static int32_t ciot_mbus_client_read(uint8_t *buf, uint16_t count, int32_t byte_timeout_ms, void *arg)
{
    ciot_mbus_client_t self = (ciot_mbus_client_t)arg;
    return ciot_iface_read_bytes(self->iface, buf, count);
}

static int32_t ciot_mbus_client_write(const uint8_t *buf, uint16_t count, int32_t byte_timeout_ms, void *arg)
{
    ciot_mbus_client_t self = (ciot_mbus_client_t)arg;
    return ciot_iface_send_bytes(self->iface, (uint8_t*)buf, count);
}

#endif // CIOT_CONFIG_FEATURE_MBUS_CLIENT == 1
