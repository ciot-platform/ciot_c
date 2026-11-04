/**
 * @file ciot_uart.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-06-07
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "ciot_config.h"

#if CIOT_CONFIG_FEATURE_UART == 1 && defined(CIOT_PLATFORM_NRF)

#include "nrf_drv_uart.h"
#include "app_fifo.h"
#include "app_util_platform.h"
#include "ciot_uart.h"
#include "ciot_err.h"

#ifndef CIOT_CONFIG_UART_TX_BUF_SIZE
#define CIOT_CONFIG_UART_TX_BUF_SIZE 256
#endif

// Bytes handed to the driver per transfer (nrf_drv_uart_tx takes a uint8_t length).
#ifndef CIOT_CONFIG_UART_TX_CHUNK_SIZE
#define CIOT_CONFIG_UART_TX_CHUNK_SIZE 32
#endif

#define CIOT_UART_DRIVER_INSTANCE 0

struct ciot_uart
{
    ciot_uart_base_t base;
    app_fifo_t tx_fifo;
    bool rx_enabled;
};

static const char *TAG = "ciot_uart";

static const nrf_drv_uart_t m_uart = NRF_DRV_UART_INSTANCE(CIOT_UART_DRIVER_INSTANCE);

static ciot_uart_t _self = NULL;

// Static, as in app_uart_fifo: there is a single UART instance, and the buffers stay off the heap.
static uint8_t m_tx_fifo_buf[CIOT_CONFIG_UART_TX_BUF_SIZE];
static uint8_t m_tx_chunk[CIOT_CONFIG_UART_TX_CHUNK_SIZE];
static uint8_t m_rx_byte[1];

static void ciot_uart_event_handler(nrf_drv_uart_event_t *p_event, void *p_context);
static void ciot_uart_tx_next(ciot_uart_t self);

ciot_uart_t ciot_uart_new(void *handle)
{
    ciot_uart_t self = calloc(1, sizeof(struct ciot_uart));
    ciot_uart_init(self);
    return self;
}

ciot_err_t ciot_uart_start(ciot_uart_t self, ciot_uart_cfg_t *cfg)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(cfg);
    ciot_uart_cfg_apply_limits(cfg);

    if (cfg->num != 0)
    {
        return CIOT_ERR_INVALID_ARG;
    }

    if (_self != NULL && _self != self)
    {
        return CIOT_ERR_INVALID_STATE;
    }

    _self = self;
    ciot_uart_base_t *base = &self->base;

    if (base->status.state == CIOT_UART_STATE_STARTED &&
        base->cfg.num == cfg->num)
    {
        return CIOT_ERR_OK;
    }

    if (cfg->has_gpio == false)
    {
        cfg->gpio = base->cfg.gpio;
    }
    base->cfg = *cfg;

    uint32_t err_code = app_fifo_init(&self->tx_fifo, m_tx_fifo_buf, sizeof(m_tx_fifo_buf));
    APP_ERROR_CHECK(err_code);

    nrf_drv_uart_config_t config = NRF_DRV_UART_DEFAULT_CONFIG;
    config.baudrate = (nrf_uart_baudrate_t)base->cfg.baud_rate;
    config.hwfc = base->cfg.flow_control ? NRF_UART_HWFC_ENABLED : NRF_UART_HWFC_DISABLED;
    config.interrupt_priority = APP_IRQ_PRIORITY_LOWEST;
    config.parity = base->cfg.parity ? NRF_UART_PARITY_INCLUDED : NRF_UART_PARITY_EXCLUDED;
    config.pselcts = base->cfg.gpio.cts;
    config.pselrts = base->cfg.gpio.rts;
    config.pselrxd = base->cfg.gpio.rx;
    config.pseltxd = base->cfg.gpio.tx;
    config.p_context = self;

    err_code = nrf_drv_uart_init(&m_uart, &config, ciot_uart_event_handler);
    APP_ERROR_CHECK(err_code);

    self->rx_enabled = config.pselrxd != NRF_UART_PSEL_DISCONNECTED;
    if (self->rx_enabled)
    {
        err_code = nrf_drv_uart_rx(&m_uart, m_rx_byte, 1);
        APP_ERROR_CHECK(err_code);
    }

    ciot_iface_send_event_type(&base->iface, CIOT_EVENT_TYPE_STARTED);

    base->status.state = CIOT_UART_STATE_STARTED;

    return CIOT_ERR_OK;
}

ciot_err_t ciot_uart_stop(ciot_uart_t self)
{
    CIOT_ERR_NULL_CHECK(self);
    nrf_drv_uart_uninit(&m_uart);
    app_fifo_flush(&self->tx_fifo);
    self->base.status.state = CIOT_UART_STATE_CLOSED;
    ciot_iface_send_event_type(&self->base.iface, CIOT_EVENT_TYPE_STOPPED);
    _self = NULL;
    return CIOT_ERR_OK;
}

// All or nothing: the bytes are queued only if all of them fit in the TX FIFO. Otherwise nothing
// is queued and CIOT_ERR_OVERFLOW is returned, so a frame never goes out partially.
ciot_err_t ciot_uart_send_bytes(ciot_uart_t self, uint8_t *bytes, int size)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(bytes);

    if (self->base.status.state != CIOT_UART_STATE_STARTED)
    {
        return CIOT_ERR_INVALID_STATE;
    }

    if (size <= 0)
    {
        return CIOT_ERR_OK;
    }

    ciot_err_t err = CIOT_ERR_OK;

    // Keeps the free space check, the write and the TX start together when senders run at different priorities.
    CRITICAL_REGION_ENTER();
    uint32_t len = 0;
    app_fifo_write(&self->tx_fifo, NULL, &len);
    if (len < (uint32_t)size)
    {
        err = CIOT_ERR_OVERFLOW;
    }
    else
    {
        len = size;
        app_fifo_write(&self->tx_fifo, bytes, &len);
        if (!nrf_drv_uart_tx_in_progress(&m_uart))
        {
            ciot_uart_tx_next(self);
        }
    }
    CRITICAL_REGION_EXIT();

    return err;
}

// Received bytes go to the iface (ciot_iface_process_data) from the driver event, so there is nothing to read here.
ciot_err_t ciot_uart_read_bytes(ciot_uart_t self, uint8_t *bytes, int size)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(bytes);
    return CIOT_ERR_NOT_IMPLEMENTED;
}

size_t ciot_uart_available(ciot_uart_t self)
{
    return 0;
}

ciot_err_t ciot_uart_task(ciot_uart_t self)
{
    return CIOT_ERR_OK;
}

// Starts the transfer of the next chunk of the TX FIFO. The bytes leave the FIFO only after the driver accepts them.
static void ciot_uart_tx_next(ciot_uart_t self)
{
    uint32_t len = 0;
    app_fifo_read(&self->tx_fifo, NULL, &len);
    if (len == 0)
    {
        return;
    }
    if (len > sizeof(m_tx_chunk))
    {
        len = sizeof(m_tx_chunk);
    }
    for (uint32_t i = 0; i < len; i++)
    {
        app_fifo_peek(&self->tx_fifo, i, &m_tx_chunk[i]);
    }
    if (nrf_drv_uart_tx(&m_uart, m_tx_chunk, (uint8_t)len) == NRF_SUCCESS)
    {
        for (uint32_t i = 0; i < len; i++)
        {
            uint8_t byte;
            app_fifo_get(&self->tx_fifo, &byte);
        }
    }
}

static void ciot_uart_event_handler(nrf_drv_uart_event_t *p_event, void *p_context)
{
    ciot_uart_t self = (ciot_uart_t)p_context;

    if (self == NULL || self != _self)
    {
        return;
    }

    ciot_uart_base_t *base = &self->base;

    switch (p_event->type)
    {
    case NRF_DRV_UART_EVT_RX_DONE:
        // 0 bytes is an RX timeout with no new byte.
        if (p_event->data.rxtx.bytes != 0)
        {
            ciot_iface_process_data(&base->iface, p_event->data.rxtx.p_data, 1, CIOT_EVENT_TYPE_MSG);
        }
        (void)nrf_drv_uart_rx(&m_uart, m_rx_byte, 1);
        break;
    case NRF_DRV_UART_EVT_ERROR:
        CIOT_LOGE(TAG, "UART communication error: %lu", (unsigned long)p_event->data.error.error_mask);
        base->status.error = CIOT_ERR_FAIL;
        if (self->rx_enabled)
        {
            (void)nrf_drv_uart_rx(&m_uart, m_rx_byte, 1);
        }
        break;
    case NRF_DRV_UART_EVT_TX_DONE:
        ciot_uart_tx_next(self);
        break;
    default:
        break;
    }
}

#endif // CIOT_CONFIG_FEATURE_UART == 1
