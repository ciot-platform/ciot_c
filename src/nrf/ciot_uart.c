/**
 * @file ciot_uart.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-11-08
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "ciot_uart.h"

#if (CIOT_CONFIG_FEATURE_UART) && defined(CIOT_TARGET_NRF)

#include "sdk_common.h"
#include "nrf_drv_uart.h"
#include "app_fifo.h"
#include "app_util_platform.h"

#include "ciot_s.h"

#define UART_PIN_DISCONNECTED 0xFFFFFFFF

typedef struct ciot_uart_fifo
{
    app_fifo_t tx;
    uint8_t tx_buf[CIOT_CONFIG_UART_TX_BUF_SIZE];
} ciot_uart_fifo_t;

struct ciot_uart
{
    ciot_uart_base_t uart;
    nrf_drv_uart_t handle;
    ciot_uart_fifo_t fifo;
    uint8_t rx_byte[1];
    uint8_t tx_byte[1];
};

ciot_err_t ciot_uart_on_message(ciot_iface_t *iface, uint8_t *data, int size);
ciot_err_t ciot_uart_task_internal(ciot_iface_t *iface, ciot_s_t ciot_s);

static void ciot_uart_event_handler(nrf_drv_uart_event_t *event, void *context);

ciot_uart_t ciot_uart_new(void *handle)
{
    ciot_uart_t self = calloc(1, sizeof(struct ciot_uart));
    self->uart.iface.base.ptr = self;
    self->uart.iface.base.start = (ciot_iface_start_fn *)ciot_uart_start;
    self->uart.iface.base.stop = (ciot_iface_stop_fn *)ciot_uart_stop;
    self->uart.iface.base.process_req = (ciot_iface_process_req_fn *)ciot_uart_process_req;
    self->uart.iface.base.send_data = (ciot_iface_send_data_fn *)ciot_uart_send_data;
    self->uart.iface.base.cfg.ptr = &self->uart.cfg;
    self->uart.iface.base.cfg.size = sizeof(self->uart.cfg);
    self->uart.iface.base.status.ptr = &self->uart.status;
    self->uart.iface.base.status.size = sizeof(self->uart.status);
    self->uart.iface.info.type = CIOT_IFACE_TYPE_UART;

    ciot_s_cfg_t s_cfg = {
        .on_message_cb = ciot_uart_on_message,
        .send_bytes = ciot_uart_send_bytes,
        .iface = &self->uart.iface
    };
    self->uart.s = ciot_s_new(&s_cfg);
    return self;
}

ciot_err_t ciot_uart_start(ciot_uart_t self, ciot_uart_cfg_t *cfg)
{
    CIOT_NULL_CHECK(self);
    uint32_t err_code;
    
    self->uart.cfg = *cfg;

    // err_code = app_fifo_init(&self->fifo.rx, self->fifo.rx_buf, CIOT_CONFIG_UART_RX_BUF_SIZE);
    // VERIFY_SUCCESS(err_code);

    err_code = app_fifo_init(&self->fifo.tx, self->fifo.tx_buf, CIOT_CONFIG_UART_TX_BUF_SIZE);
    VERIFY_SUCCESS(err_code);

    nrf_drv_uart_config_t config = NRF_DRV_UART_DEFAULT_CONFIG;
    config.baudrate = (nrf_uart_baudrate_t)self->uart.cfg.baud_rate;
    config.hwfc = self->uart.cfg.flow_control;
    config.interrupt_priority = APP_IRQ_PRIORITY_LOWEST;
    config.parity = self->uart.cfg.parity;
    config.pselcts = self->uart.cfg.cts_pin;
    config.pselrts = self->uart.cfg.rts_pin;
    config.pselrxd = self->uart.cfg.rx_pin;
    config.pseltxd = self->uart.cfg.tx_pin;
    config.p_context = self;
    ciot_s_set_bridge_mode(self->uart.s, self->uart.cfg.bridge_mode);

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

    if (self->uart.cfg.rx_pin != UART_PIN_DISCONNECTED)
    {
        nrf_drv_uart_rx(&self->handle, self->rx_byte, 1);
    }
       
    return CIOT_OK;
}

ciot_err_t ciot_uart_stop(ciot_uart_t self)
{
    nrf_drv_uart_uninit(&self->handle);
    return NRF_SUCCESS;
}

// ciot_err_t ciot_uart_process_req(ciot_uart_t self, ciot_uart_req_t *req)
// {
//     CIOT_NULL_CHECK(self);
//     CIOT_NULL_CHECK(req);

//     switch (req->type)
//     {
//     case CIOT_UART_REQ_SEND_DATA:
//         return ciot_uart_send_bytes(&self->uart.iface, req->data.send_data.data, req->data.send_data.size);
//     case CIOT_UART_REQ_ENABLE_BRIDGE_MODE:
//         return ciot_s_set_bridge_mode(self->uart.s, true);
//     default:
//         return CIOT_ERR_INVALID_ID;
//     }

//     return CIOT_ERR_INVALID_ID;
// }

ciot_err_t ciot_uart_send_data(ciot_uart_t self, uint8_t *data, int size)
{
    CIOT_NULL_CHECK(self);
    CIOT_NULL_CHECK(data);
    return ciot_s_send(self->uart.s, data, size);
}

ciot_err_t ciot_uart_send_bytes(ciot_iface_t *iface, uint8_t *bytes, int size)
{
    CIOT_NULL_CHECK(iface);
    CIOT_NULL_CHECK(bytes);
    
    uint32_t err_code;
    uint32_t len = 0;
    ciot_uart_t self = (ciot_uart_t)iface;

    err_code = app_fifo_write(&self->fifo.tx, NULL, &len);
    err_code = len < size ? CIOT_ERR_OVERFLOW : CIOT_OK;
    if(err_code == CIOT_OK)
    {
        len = size;
        err_code = app_fifo_write(&self->fifo.tx, bytes, &len);
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

ciot_err_t ciot_uart_set_bridge_mode(ciot_uart_t self, bool mode)
{
    CIOT_NULL_CHECK(self);
    return ciot_s_set_bridge_mode(self->uart.s, mode);
}

ciot_err_t ciot_uart_task(ciot_uart_t self)
{
    CIOT_NULL_CHECK(self);
    return ciot_uart_task_internal(&self->uart.iface, self->uart.s);
}

static void ciot_uart_event_handler(nrf_drv_uart_event_t *event, void *args)
{
    ciot_uart_t self = (ciot_uart_t)args;

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
            ciot_s_process_byte(self->uart.s, event->data.rxtx.p_data[0]);
            nrf_drv_uart_rx(&self->handle, self->rx_byte, 1);
            break;
        case NRF_DRV_UART_EVT_ERROR:
            nrf_drv_uart_rx(&self->handle, self->rx_byte, 1);
            break;
        break;
    }
}

#endif
