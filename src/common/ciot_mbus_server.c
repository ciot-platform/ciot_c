/**
 * @file ciot_mbus_server.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-12-11
 *
 * @copyright Copyright (c) 2024
 *
 */
 
#include "ciot_config.h"

#if CIOT_CONFIG_FEATURE_MBUS_SERVER == 1

#include "ciot_mbus_server.h"
#include "ciot_timer.h"
#include "ciot_uart.h"
#include "ciot_socket.h"
#include <stdlib.h>

struct ciot_mbus_server
{
    ciot_mbus_server_base_t base;
    nmbs_t nmbs;
    bool nmbs_initialized;
};

static const char *TAG = "ciot_mbus_server";

static int32_t ciot_mbus_server_read(uint8_t *buf, uint16_t count, int32_t byte_timeout_ms, void *arg);
static int32_t ciot_mbus_server_write(const uint8_t *buf, uint16_t count, int32_t byte_timeout_ms, void *arg);
static nmbs_error ciot_mbus_server_read_coils(uint16_t address, uint16_t quantity, nmbs_bitfield coils_out, uint8_t unit_id, void *arg);
static nmbs_error ciot_mbus_server_write_multiple_coils(uint16_t address, uint16_t quantity, const nmbs_bitfield coils, uint8_t unit_id, void *arg);
static nmbs_error ciot_mbus_server_read_holding_registers(uint16_t address, uint16_t quantity, uint16_t *registers_out, uint8_t unit_id, void *arg);
static nmbs_error ciot_mbus_server_write_multiple_registers(uint16_t address, uint16_t quantity, const uint16_t *registers, uint8_t unit_id, void *arg);

ciot_mbus_server_t ciot_mbus_server_new(void *handle, ciot_mbus_data_t *data, ciot_iface_t *conn)
{
    if(data == NULL) {
        CIOT_LOGE(TAG, "data is null");
        return NULL;
    }
    if(conn == NULL) {
        CIOT_LOGE(TAG, "conn is null");
        return NULL;
    }
    ciot_mbus_server_t self = calloc(1, sizeof(struct ciot_mbus_server));
    ciot_mbus_server_init(self);
    self->base.data = *data;
    self->base.conn = conn;
    self->base.rtu_conn = conn;
    return self;
}

ciot_err_t ciot_mbus_server_set_tcp_conn(ciot_mbus_server_t self, ciot_iface_t *tcp_conn)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(tcp_conn);
    self->base.tcp_conn = tcp_conn;
    return CIOT_ERR_OK;
}

ciot_err_t ciot_mbus_server_start(ciot_mbus_server_t self, ciot_mbus_server_cfg_t *cfg)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(cfg);

    /*
     * Reusing the same iface for both transports (e.g. hg_tcp, where RTU and TCP are
     * mutually exclusive modes of a single MBUS_SERVER iface) means switching
     * which_type at runtime must tear down whichever transport was active before
     * bringing up the new one - otherwise both could end up started at once, or the
     * old one's resources (UART peripheral, listening socket) would leak.
     * Same-transport reconfigures (e.g. a new baud rate or a new TCP port) keep the
     * existing restart-in-place behavior below.
     */
    if (self->base.status.state == CIOT_MBUS_SERVER_STATE_STARTED &&
        self->base.cfg.which_type != cfg->which_type)
    {
        ciot_mbus_server_stop(self);
    }

    self->base.cfg = *cfg;

    nmbs_platform_conf platform_conf;
    nmbs_platform_conf_create(&platform_conf);
    platform_conf.read = ciot_mbus_server_read;
    platform_conf.write = ciot_mbus_server_write;
    platform_conf.arg = self;

    nmbs_callbacks callbacks;
    nmbs_callbacks_create(&callbacks);
    callbacks.arg = self;
    callbacks.read_coils = ciot_mbus_server_read_coils;
    callbacks.write_multiple_coils = ciot_mbus_server_write_multiple_coils;
    callbacks.read_holding_registers = ciot_mbus_server_read_holding_registers;
    callbacks.write_multiple_registers = ciot_mbus_server_write_multiple_registers;

    nmbs_set_read_timeout(&self->nmbs, CIOT_MBUS_SERVER_READ_TIMEOUT_MS);
    nmbs_set_byte_timeout(&self->nmbs, CIOT_MBUS_SERVER_BYTE_TIMEOUT_MS);

    nmbs_error err = 0;
    switch (cfg->which_type)
    {
    case CIOT_MBUS_SERVER_CFG_RTU_TAG:
        self->base.conn = self->base.rtu_conn;
        CIOT_ERR_NULL_CHECK(self->base.conn);
        platform_conf.transport = NMBS_TRANSPORT_RTU;
        if(cfg->rtu.has_uart) {
            CIOT_ERR_RETURN(ciot_uart_start((ciot_uart_t)self->base.conn, &cfg->rtu.uart));
        }
        break;
    case CIOT_MBUS_SERVER_CFG_TCP_TAG:
        self->base.conn = self->base.tcp_conn;
        CIOT_ERR_NULL_CHECK(self->base.conn);
        platform_conf.transport = NMBS_TRANSPORT_TCP;
        if (cfg->tcp.max_connections > 1)
        {
            CIOT_LOGW(TAG, "max_connections=%lu not supported yet, serving 1 connection at a time", (long unsigned int)cfg->tcp.max_connections);
        }
        CIOT_ERR_RETURN(ciot_socket_start_server((ciot_socket_t)self->base.conn, cfg->tcp.port, CIOT_MBUS_SERVER_BYTE_TIMEOUT_MS));
        break;
    default:
        return CIOT_ERR_INVALID_ARG;
    }

    err = nmbs_server_create(&self->nmbs, cfg->rtu.server_id, &platform_conf, &callbacks);
    if (err == NMBS_ERROR_NONE)
    {
        self->nmbs_initialized = true;
        self->base.status.state = CIOT_MBUS_SERVER_STATE_STARTED;
        ciot_iface_send_event_type(&self->base.iface, CIOT_EVENT_TYPE_STARTED);
    }
    else
    {
        self->base.status.state = CIOT_MBUS_SERVER_STATE_ERROR;
        ciot_iface_send_event_type(&self->base.iface, CIOT_EVENT_TYPE_ERROR);
    }

    return ciot_mbus_get_error(err);
}

ciot_err_t ciot_mbus_server_stop(ciot_mbus_server_t self)
{
     CIOT_ERR_NULL_CHECK(self);
     ciot_err_t conn_err = CIOT_ERR_OK;
     if (self->nmbs_initialized &&
         self->base.cfg.which_type == CIOT_MBUS_SERVER_CFG_RTU_TAG &&
         self->base.cfg.rtu.has_uart)
     {
         conn_err = ciot_uart_stop((ciot_uart_t)self->base.conn);
     }
     else if (self->base.cfg.which_type == CIOT_MBUS_SERVER_CFG_TCP_TAG)
     {
         conn_err = ciot_socket_stop((ciot_socket_t)self->base.conn);
     }
     memset(&self->nmbs, 0, sizeof(self->nmbs));
     self->nmbs_initialized = false;
     self->base.status.state = CIOT_MBUS_SERVER_STATE_STOPPED;
     self->base.status.error = conn_err;
     ciot_iface_send_event_type(&self->base.iface, CIOT_EVENT_TYPE_STOPPED);
     return conn_err;
}

ciot_err_t ciot_mbus_server_task(ciot_mbus_server_t self)
{
    CIOT_ERR_NULL_CHECK(self);
    if (self->base.cfg.which_type == CIOT_MBUS_SERVER_CFG_TCP_TAG)
    {
        ciot_socket_task((ciot_socket_t)self->base.conn);
    }
    if (self->base.status.state == CIOT_MBUS_SERVER_STATE_STARTED && self->base.conn->state == CIOT_IFACE_STATE_STARTED)
    {
        if(self->base.cfg.which_type == CIOT_MBUS_SERVER_CFG_RTU_TAG && ciot_uart_available((ciot_uart_t)self->base.conn) == 0) {
            return CIOT_ERR_OK;
        }
        /*
         * Same reasoning as the RTU guard above: nmbs_server_poll() blocks (via the
         * platform read callback's byte/read timeout) waiting for the first byte of a
         * new request when nothing is pending. Without this check, every task() tick
         * on an idle-but-connected TCP client would stall the caller's loop for up to
         * CIOT_MBUS_SERVER_READ_TIMEOUT_MS - e.g. starving other work like a BLE
         * advertisement queue drained from the same loop.
         */
        if(self->base.cfg.which_type == CIOT_MBUS_SERVER_CFG_TCP_TAG && ciot_socket_available((ciot_socket_t)self->base.conn) == 0) {
            return CIOT_ERR_OK;
        }
        nmbs_error err = nmbs_server_poll(&self->nmbs);
        self->base.status.error = ciot_mbus_get_error(err);
        self->base.status.last_poll = ciot_timer_now();
        if(err != NMBS_ERROR_NONE) {
            self->base.status.error_count++;
        }
        return self->base.status.error;
    }
    return CIOT_ERR_OK;
}

ciot_err_t ciot_mbus_server_send_bytes(ciot_mbus_server_t self, uint8_t *data, int size)
{
    return CIOT_ERR_NOT_IMPLEMENTED;
}

ciot_err_t ciot_mbus_server_set_reg(ciot_mbus_server_t self, uint16_t addr, void *data, uint16_t size)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_UINDEX_CHECK(addr, self->base.data.regs.count - size);
    self->base.status.last_update = ciot_timer_now();
    memcpy(&self->base.data.regs.values[addr], data, size);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_mbus_server_get_reg(ciot_mbus_server_t self, uint16_t addr, void *data, uint16_t size)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_UINDEX_CHECK(addr, self->base.data.regs.count - size);
    memcpy(data, &self->base.data.regs.values[addr], size);
    return CIOT_ERR_OK;
}

static int32_t ciot_mbus_server_read(uint8_t *buf, uint16_t count, int32_t byte_timeout_ms, void *arg)
{
    ciot_mbus_server_t self = (ciot_mbus_server_t)arg;
    return ciot_iface_read_bytes(self->base.conn, buf, count);
}

static int32_t ciot_mbus_server_write(const uint8_t *buf, uint16_t count, int32_t byte_timeout_ms, void *arg)
{
    ciot_mbus_server_t self = (ciot_mbus_server_t)arg;
    return ciot_iface_send_bytes(self->base.conn, (uint8_t*)buf, count);
}

static nmbs_error ciot_mbus_server_read_coils(uint16_t address, uint16_t quantity, nmbs_bitfield coils_out, uint8_t unit_id, void *arg)
{
    ciot_mbus_server_t self = (ciot_mbus_server_t)arg;

    if (address + quantity > self->base.data.coils.count)
    {
        return NMBS_EXCEPTION_ILLEGAL_DATA_ADDRESS;
    }

    for (size_t i = 0; i < quantity; i++)
    {
        bool value = nmbs_bitfield_read(self->base.data.coils.values, address + i);
        nmbs_bitfield_write(coils_out, i, value);
    }

    self->base.status.last_request = ciot_timer_now();
    self->base.status.request_count++;

    return NMBS_ERROR_NONE;
}

static nmbs_error ciot_mbus_server_write_multiple_coils(uint16_t address, uint16_t quantity, const nmbs_bitfield coils, uint8_t unit_id, void *arg)
{
    ciot_mbus_server_t self = (ciot_mbus_server_t)arg;

    if (address + quantity > self->base.data.coils.count)
    {
        return NMBS_EXCEPTION_ILLEGAL_DATA_ADDRESS;
    }

    for (int i = 0; i < quantity; i++)
    {
        nmbs_bitfield_write(self->base.data.coils.values, address + i, nmbs_bitfield_read(coils, i));
    }

    self->base.status.last_request = ciot_timer_now();
    self->base.status.request_count++;

    return NMBS_ERROR_NONE;
}

static nmbs_error ciot_mbus_server_read_holding_registers(uint16_t address, uint16_t quantity, uint16_t *registers_out, uint8_t unit_id, void *arg)
{
    ciot_mbus_server_t self = (ciot_mbus_server_t)arg;

    if (address + quantity > self->base.data.regs.count)
    {
        return NMBS_EXCEPTION_ILLEGAL_DATA_ADDRESS;
    }

    for (size_t i = 0; i < quantity; i++)
    {
        registers_out[i] = self->base.data.regs.values[address + i];
    }

    self->base.status.last_request = ciot_timer_now();
    self->base.status.request_count++;

    return NMBS_ERROR_NONE;
}

static nmbs_error ciot_mbus_server_write_multiple_registers(uint16_t address, uint16_t quantity, const uint16_t *registers, uint8_t unit_id, void *arg)
{
    ciot_mbus_server_t self = (ciot_mbus_server_t)arg;

    if (address + quantity > self->base.data.regs.count)
    {
        return NMBS_EXCEPTION_ILLEGAL_DATA_ADDRESS;
    }

    for (size_t i = 0; i < quantity; i++)
    {
        self->base.data.regs.values[address + i] = registers[i];
    }

    self->base.status.last_request = ciot_timer_now();
    self->base.status.request_count++;

    return NMBS_ERROR_NONE;
}

#endif // CIOT_CONFIG_FEATURE_MBUS_SERVER == 1