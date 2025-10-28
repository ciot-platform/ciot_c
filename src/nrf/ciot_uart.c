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

#if CIOT_CONFIG_FEATURE_UART == 1

#include <stdlib.h>
#include "app_fifo.h"
#include "app_util_platform.h"
#include "nrf_drv_uart.h"
#include "sdk_config.h"
#include "sdk_macros.h"
#include "ciot_uart.h"
#include "ciot_err.h"

#define UART_PIN_DISCONNECTED 0xFFFFFFFF

typedef struct ciot_uart_fifo
{
    app_fifo_t tx;
    uint8_t tx_buf[CIOT_CONFIG_UART_TX_BUF_SIZE];
} ciot_uart_fifo_t;

struct ciot_uart
{
    ciot_uart_base_t base;
    nrf_drv_uart_t handle;
    ciot_uart_fifo_t fifo;
    uint8_t rx_byte[1];
    uint8_t tx_byte[1];
};

static void ciot_uart_event_handler(nrf_drv_uart_event_t *event, void *context);

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

    ciot_uart_base_t *base = &self->base;

    if(base->status.state == CIOT_UART_STATE_STARTED &&
       base->cfg.num == cfg->num)
    {
        return CIOT_ERR_OK;
    }

    if(cfg->has_gpio == false)
    {
        cfg->gpio = base->cfg.gpio;
    }
    base->cfg = *cfg;

    uint32_t err_code = app_fifo_init(&self->fifo.tx, self->fifo.tx_buf, CIOT_CONFIG_UART_TX_BUF_SIZE);
    VERIFY_SUCCESS(err_code);

    nrf_drv_uart_config_t config = NRF_DRV_UART_DEFAULT_CONFIG;
    config.baudrate = (nrf_uart_baudrate_t)base->cfg.baud_rate;
    config.hwfc = base->cfg.flow_control;
    config.interrupt_priority = APP_IRQ_PRIORITY_LOWEST;
    config.parity = base->cfg.parity;
    config.pselcts = base->cfg.gpio.cts;
    config.pselrts = base->cfg.gpio.rts;
    config.pselrxd = base->cfg.gpio.rx;
    config.pseltxd = base->cfg.gpio.tx;
    config.p_context = self;

    switch (cfg->num)
    {
#if UART0_ENABLED
    case 0:
        {
            static nrf_drv_uart_t uart_inst0 = NRF_DRV_UART_INSTANCE(0);
            self->handle = uart_inst0;
            break;
        }
#endif
#if UART1_ENABLED
    case 1:
        {
            static nrf_drv_uart_t uart_inst1 = NRF_DRV_UART_INSTANCE(1);
            self->handle = uart_inst1;
            break;
        }
#endif
    default:
        return CIOT_ERR_INVALID_ARG;
    }

    err_code = nrf_drv_uart_init(&self->handle, &config, ciot_uart_event_handler);
    VERIFY_SUCCESS(err_code);

    if (base->cfg.gpio.rx != UART_PIN_DISCONNECTED)
    {
        nrf_drv_uart_rx(&self->handle, self->rx_byte, 1);
    }

    // ciot_event_t event = {0};
    // event.type = CIOT_IFACE_EVENT_STARTED;
    // event.msg = ciot_msg_get(CIOT__MSG_TYPE__MSG_TYPE_STATUS, &base->iface);
    // ciot_iface_send_event(&base->iface, &event);
    ciot_iface_send_event_type(&base->iface, CIOT_EVENT_TYPE_STARTED);

    base->status.state = CIOT_UART_STATE_STARTED;

    return CIOT_ERR_OK;
}

ciot_err_t ciot_uart_stop(ciot_uart_t self)
{
    CIOT_ERR_NULL_CHECK(self);
    nrf_drv_uart_uninit(&self->handle);
    self->base.status.state = CIOT_UART_STATE_CLOSED;
    ciot_iface_send_event_type(&self->base.iface, CIOT_EVENT_TYPE_STOPPED);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_uart_send_bytes(ciot_uart_t self, uint8_t *bytes, int size)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(bytes);
    
    uint32_t err_code;
    uint32_t len = 0;

    app_fifo_write(&self->fifo.tx, NULL, &len);
    err_code = len < size ? CIOT_UART_ERROR_FIFO_OVERFLOW : CIOT_ERR_OK;
    if(err_code == CIOT_ERR_OK)
    {
        len = size;
        err_code = app_fifo_write(&self->fifo.tx, bytes, &len);
    }
    else if(self->base.status.state == CIOT_UART_STATE_STARTED)
    {
        self->base.status.error = self->base.status.error;
    }
    if(!nrf_drv_uart_tx_in_progress(&self->handle))
    {
        if(app_fifo_get(&self->fifo.tx, self->tx_byte) == NRF_SUCCESS) 
        {
            err_code = nrf_drv_uart_tx(&self->handle, self->tx_byte, 1);
        }
    }

    return err_code;
}

ciot_err_t ciot_uart_read_bytes(ciot_uart_t self, uint8_t *bytes, int size)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(bytes);
    ret_code_t code = nrf_drv_uart_rx(&self->handle, bytes,  size);
    return code == NRF_SUCCESS ? size : CIOT_ERR_FAIL;
}

ciot_err_t ciot_uart_task(ciot_uart_t self)
{
    return CIOT_ERR_NOT_IMPLEMENTED;
}

static void ciot_uart_event_handler(nrf_drv_uart_event_t *event, void *args)
{
    ciot_uart_t self = (ciot_uart_t)args;
    ciot_uart_base_t *base = &self->base;

    switch (event->type)
    {
        case NRF_DRV_UART_EVT_TX_DONE:
            if (app_fifo_get(&self->fifo.tx, self->tx_byte) == NRF_SUCCESS)
            {
                nrf_drv_uart_tx(&self->handle, self->tx_byte, 1);
            }
            break;
        case NRF_DRV_UART_EVT_RX_DONE:
            if(event->data.rxtx.bytes == 0)
            {
                nrf_drv_uart_rx(&self->handle, self->rx_byte, 1);
                break;
            }
            ciot_err_t err = ciot_iface_process_data(&self->base.iface, event->data.rxtx.p_data, event->data.rxtx.bytes, CIOT_EVENT_TYPE_REQUEST);
            if(err != CIOT_ERR_OK)
            {
                base->status.error = err;
            }
            nrf_drv_uart_rx(&self->handle, self->rx_byte, 1);
            break;
        case NRF_DRV_UART_EVT_ERROR:
            nrf_drv_uart_rx(&self->handle, self->rx_byte, 1);
            break;
        break;
    }
}

#endif // CIOT_CONFIG_FEATURE_UART == 1
