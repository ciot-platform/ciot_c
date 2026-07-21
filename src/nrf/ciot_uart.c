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

#include "app_uart.h"
#include "ciot_uart.h"
#include "ciot_err.h"

 #ifndef CIOT_CONFIG_UART_RX_BUF_SIZE
 #define CIOT_CONFIG_UART_RX_BUF_SIZE 256
 #endif
 #ifndef CIOT_CONFIG_UART_TX_BUF_SIZE
 #define CIOT_CONFIG_UART_TX_BUF_SIZE 256
 #endif

struct ciot_uart
{
    ciot_uart_base_t base;
};

static const char *TAG = "ciot_uart";

static ciot_uart_t _self = NULL;

static void ciot_uart_event_handler(app_uart_evt_t * p_event);

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

    if(_self != NULL)
    {
        return CIOT_ERR_INVALID_STATE;
    }

    _self = self;
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

    const app_uart_comm_params_t comm_params =
    {
        .rx_pin_no = base->cfg.gpio.rx,
        .tx_pin_no = base->cfg.gpio.tx,
        .rts_pin_no = base->cfg.gpio.rts,
        .cts_pin_no = base->cfg.gpio.cts,
        .flow_control = base->cfg.flow_control ? APP_UART_FLOW_CONTROL_ENABLED : APP_UART_FLOW_CONTROL_DISABLED,
        .use_parity = base->cfg.parity ? true : false,
        .baud_rate = base->cfg.baud_rate
    };

    uint32_t err_code;

    APP_UART_FIFO_INIT(&comm_params,
                         CIOT_CONFIG_UART_RX_BUF_SIZE,
                         CIOT_CONFIG_UART_TX_BUF_SIZE,
                         ciot_uart_event_handler,
                         APP_IRQ_PRIORITY_LOWEST,
                         err_code);

    APP_ERROR_CHECK(err_code);

    ciot_iface_send_event_type(&base->iface, CIOT_EVENT_TYPE_STARTED);

    base->status.state = CIOT_UART_STATE_STARTED;

    return CIOT_ERR_OK;
}

ciot_err_t ciot_uart_stop(ciot_uart_t self)
{
    CIOT_ERR_NULL_CHECK(self);
    app_uart_close();
    self->base.status.state = CIOT_UART_STATE_CLOSED;
    ciot_iface_send_event_type(&self->base.iface, CIOT_EVENT_TYPE_STOPPED);
    _self = NULL;
    return CIOT_ERR_OK;
}

ciot_err_t ciot_uart_send_bytes(ciot_uart_t self, uint8_t *bytes, int size)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(bytes);

    for(int i = 0; i < size; i++)
    {
        uint32_t err_code = app_uart_put(bytes[i]);
        if(err_code != NRF_SUCCESS)
        {
            return CIOT_ERR_FAIL;
        }
    }

    return CIOT_ERR_OK;
}

ciot_err_t ciot_uart_read_bytes(ciot_uart_t self, uint8_t *bytes, int size)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(bytes);
    
    for (int i = 0; i < size; i++)
    {
        uint8_t byte;
        uint32_t err_code = app_uart_get(&byte);
        if(err_code != NRF_SUCCESS)
        {
            return CIOT_ERR_FAIL;
        }
        bytes[i] = byte;
    }

    return CIOT_ERR_OK;
}

size_t ciot_uart_available(ciot_uart_t self)
{
    return 0;
}

ciot_err_t ciot_uart_task(ciot_uart_t self)
{
    return CIOT_ERR_OK;
}

static void ciot_uart_event_handler(app_uart_evt_t * p_event)
{
    ciot_uart_t self = _self;
    ciot_uart_base_t *base = &self->base;
    uint8_t byte;

    switch (p_event->evt_type)
    {
        case APP_UART_DATA_READY:
            if(app_uart_get(&byte) == NRF_SUCCESS)
            {
                ciot_iface_process_data(&base->iface, &byte, 1, CIOT_EVENT_TYPE_MSG);
            }
            break;
        case APP_UART_COMMUNICATION_ERROR:
            CIOT_LOGE(TAG, "UART communication error: %d", p_event->data.error_communication);
            base->status.error = CIOT_ERR_FAIL;
            break;
        case APP_UART_FIFO_ERROR:
            CIOT_LOGE(TAG, "UART FIFO error: %d", p_event->data.error_code);
            base->status.error = CIOT_ERR_OVERFLOW;
            break;
        default:
            break;
    }
}

#endif // CIOT_CONFIG_FEATURE_UART == 1
