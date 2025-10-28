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
#include "ciot_uart.h"
#include "ciot_err.h"

#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#ifndef CIOT_CONFIG_UART_RX_BUF_SIZE
#define CIOT_CONFIG_UART_RX_BUF_SIZE 256
#endif

#ifndef CIOT_CONFIG_UART_TX_BUF_SIZE
#define CIOT_CONFIG_UART_TX_BUF_SIZE 256
#endif

#ifndef CIOT_CONFIG_UART_QUEUE_SIZE
#define CIOT_CONFIG_UART_QUEUE_SIZE 10
#endif

#ifndef CIOT_CONFIG_UART_TASK_SIZE
#define CIOT_CONFIG_UART_TASK_SIZE 4096
#endif

#ifndef CIOT_CONFIG_UART_TASK_PRIO
#define CIOT_CONFIG_UART_TASK_PRIO (tskIDLE_PRIORITY + 1)
#endif

#ifndef CIOT_CONFIG_UART_TASK_CORE
#define CIOT_CONFIG_UART_TASK_CORE 0
#endif

static const char *TAG = "ciot_uart";

struct ciot_uart
{
    ciot_uart_base_t base;
    QueueHandle_t queue;
    TaskHandle_t task;
    uart_event_t event;
};

static void ciot_uart0_task(void *args);
static void ciot_uart1_task(void *args);
static void ciot_uart2_task(void *args);
static void ciot_uart_event_handler(ciot_uart_t self, uart_event_t *event);

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
        CIOT_LOGW(TAG, "UART %d already started", (int)base->cfg.num);
        return CIOT_ERR_OK;
    }

    CIOT_LOGI(TAG, "num: %d", (int)cfg->num);

    if(base->status.state == CIOT_UART_STATE_STARTED)
    {
        CIOT_LOGW(TAG, "UART already started");
        return CIOT_ERR_OK;
    }

    if(cfg->has_gpio == false)
    {
        cfg->has_gpio = base->cfg.has_gpio;
        cfg->gpio = base->cfg.gpio;
    }
    base->cfg = *cfg;


    int num = base->cfg.num;
    const uart_config_t uart_cfg = {
        .flow_ctrl = cfg->flow_control,
        .parity = cfg->parity,
        .baud_rate = cfg->baud_rate,
        .data_bits = UART_DATA_8_BITS,
        .stop_bits = UART_STOP_BITS_1,
        .source_clk = UART_SCLK_DEFAULT,
    };

    ESP_ERROR_CHECK(uart_param_config(num, &uart_cfg));
    ESP_ERROR_CHECK(uart_set_pin(num, base->cfg.gpio.tx, base->cfg.gpio.rx, base->cfg.gpio.rts, base->cfg.gpio.cts));
    ESP_ERROR_CHECK(uart_driver_install(num, CIOT_CONFIG_UART_RX_BUF_SIZE, CIOT_CONFIG_UART_TX_BUF_SIZE, CIOT_CONFIG_UART_QUEUE_SIZE, &self->queue, 0));
    ESP_ERROR_CHECK(uart_set_mode(num, base->cfg.mode));

    if(base->iface.decoder)
    {
        if (num == 0)
            xTaskCreatePinnedToCore(ciot_uart0_task, "ciot_uart0_task", CIOT_CONFIG_UART_TASK_SIZE, self, CIOT_CONFIG_UART_TASK_PRIO, &self->task, CIOT_CONFIG_UART_TASK_CORE);
        if (num == 1)
            xTaskCreatePinnedToCore(ciot_uart1_task, "ciot_uart1_task", CIOT_CONFIG_UART_TASK_SIZE, self, CIOT_CONFIG_UART_TASK_PRIO, &self->task, CIOT_CONFIG_UART_TASK_CORE);
        if (num == 2)
            xTaskCreatePinnedToCore(ciot_uart2_task, "ciot_uart2_task", CIOT_CONFIG_UART_TASK_SIZE, self, CIOT_CONFIG_UART_TASK_PRIO, &self->task, CIOT_CONFIG_UART_TASK_CORE);
    }

    base->status.state = CIOT_UART_STATE_STARTED;
    ciot_iface_send_event_type(&base->iface, CIOT_EVENT_TYPE_STARTED);

    return CIOT_ERR_OK;
}

ciot_err_t ciot_uart_stop(ciot_uart_t self)
{
    CIOT_ERR_NULL_CHECK(self);
    return CIOT_ERR_NOT_IMPLEMENTED;
}

ciot_err_t ciot_uart_task(ciot_uart_t self)
{
    CIOT_ERR_NULL_CHECK(self);
    return CIOT_ERR_NOT_IMPLEMENTED;
}

ciot_err_t ciot_uart_send_bytes(ciot_uart_t self, uint8_t *bytes, int size)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(bytes);
    uart_write_bytes(self->base.cfg.num, bytes, size);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_uart_read_bytes(ciot_uart_t self, uint8_t *bytes, int size)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(bytes);
    int read = uart_read_bytes(self->base.cfg.num, bytes, size, pdMS_TO_TICKS(self->base.cfg.read_timeout));
    return read == size ? CIOT_ERR_OK : CIOT_ERR_TIMEOUT;
}

static void ciot_uart0_task(void *args)
{
    ciot_uart_t self = (ciot_uart_t)args;
    CIOT_LOGI(TAG, "uart0 task started");

    while (true)
    {
        if (xQueueReceive(self->queue, (void *)&self->event, portMAX_DELAY))
        {
            ciot_uart_event_handler(self, &self->event);
        }
    }
}

static void ciot_uart1_task(void *args)
{
    ciot_uart_t self = (ciot_uart_t)args;
    CIOT_LOGI(TAG, "uart1 task started");

    while (true)
    {
        if (xQueueReceive(self->queue, (void *)&self->event, portMAX_DELAY))
        {
            ciot_uart_event_handler(self, &self->event);
        }
    }
}

static void ciot_uart2_task(void *args)
{
    ciot_uart_t self = (ciot_uart_t)args;
    CIOT_LOGI(TAG, "uart2 task started");

    while (true)
    {
        if (xQueueReceive(self->queue, (void *)&self->event, portMAX_DELAY))
        {
            ciot_uart_event_handler(self, &self->event);
        }
    }
}

static void ciot_uart_event_handler(ciot_uart_t self, uart_event_t *event)
{
    ciot_uart_base_t *base = &self->base;

    switch (event->type)
    {
    case UART_DATA:
        while (event->size)
        {
            uint8_t byte;
            uart_read_bytes(base->cfg.num, &byte, 1, portMAX_DELAY);
            event->size--;
            ciot_iface_process_data(&base->iface, &byte, 1, CIOT_EVENT_TYPE_REQUEST);
        }
        break;
    case UART_FIFO_OVF:
        ESP_LOGE(TAG, "UART_FIFO_OVF[%ld]", base->cfg.num);
        base->status.error = CIOT_UART_ERROR_FIFO_OVERFLOW;
        uart_flush_input(base->cfg.num);
        xQueueReset(self->queue);
        break;
    case UART_BUFFER_FULL:
        ESP_LOGE(TAG, "UART_BUFFER_FULL[%ld]: %d", base->cfg.num, event->size);
        base->status.error = CIOT_UART_ERROR_BUFFER_FULL;
        uart_flush_input(base->cfg.num);
        xQueueReset(self->queue);
        break;
    case UART_BREAK:
        ESP_LOGE(TAG, "UART_BREAK[%ld]", base->cfg.num);
        base->status.error = CIOT_UART_ERROR_BREAK;
        uart_flush_input(base->cfg.num);
        xQueueReset(self->queue);
        break;
    case UART_FRAME_ERR:
        ESP_LOGE(TAG, "UART_FRAME_ERR[%ld]", base->cfg.num);
        base->status.error = CIOT_UART_ERROR_FRAME;
        uart_flush_input(base->cfg.num);
        xQueueReset(self->queue);
        break;
    default:
        ESP_LOGW(TAG, "unhandled event: %d", event->type);
        break;
    }
}

#endif //! CIOT_CONFIG_FEATURE_UART == 1
