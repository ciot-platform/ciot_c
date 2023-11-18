/**
 * @file ciot_uart.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-10-30
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "ciot_uart.h"

#if CIOT_CONFIG_FEATURE_UART && defined(CIOT_TARGET_ESP)

#include <string.h>

#include "esp_log.h"
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

struct ciot_uart
{
    ciot_uart_base_t uart;
    QueueHandle_t queue;
    TaskHandle_t task;
    uart_event_t event;
};

static void ciot_uart_event_handler(void *args);

ciot_err_t ciot_uart_on_message(ciot_iface_t *iface, uint8_t *data, int size);

static const char *TAG = "ciot_uart";

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
    CIOT_NULL_CHECK(cfg);
    self->uart.cfg = *cfg;
    int num = self->uart.cfg.num;
    const uart_config_t uart_cfg = {
        .flow_ctrl = self->uart.cfg.flow_control,
        .parity = self->uart.cfg.parity,
        .baud_rate = self->uart.cfg.baud_rate,
        .source_clk = UART_SCLK_DEFAULT,
    };
    ciot_s_set_bridge_mode(self->uart.s, self->uart.cfg.bridge_mode);
    ESP_ERROR_CHECK(uart_param_config(num, &uart_cfg));
    ESP_ERROR_CHECK(uart_set_pin(num, self->uart.cfg.tx_pin, self->uart.cfg.rx_pin, self->uart.cfg.rts_pin, self->uart.cfg.cts_pin));
    ESP_ERROR_CHECK(uart_driver_install(num, CIOT_CONFIG_UART_RX_BUF_SIZE, CIOT_CONFIG_UART_TX_BUF_SIZE, CIOT_CONFIG_UART_QUEUE_SIZE, &self->queue, 0));
    xTaskCreatePinnedToCore(ciot_uart_event_handler, "ciot_uart_task", CIOT_CONFIG_UART_TASK_SIZE, self, CIOT_CONFIG_UART_TASK_PRIO, &self->task, CIOT_CONFIG_UART_TASK_CORE);
    return CIOT_OK;
}

ciot_err_t ciot_uart_stop(ciot_uart_t self)
{
    CIOT_NULL_CHECK(self);
    uart_driver_delete(self->uart.cfg.num);
    vTaskDelete(self->task);
    return CIOT_OK;
}

ciot_err_t ciot_uart_send_data(ciot_uart_t self, uint8_t *data, int size)
{
    CIOT_NULL_CHECK(self);
    CIOT_NULL_CHECK(data);
    return ciot_s_send(self->uart.s, data, size);
}

ciot_err_t ciot_uart_send_bytes(ciot_iface_t *iface, uint8_t *bytes, int size)
{
    ciot_uart_t self = (ciot_uart_t)iface;
    CIOT_NULL_CHECK(self);
    CIOT_NULL_CHECK(bytes);
    uart_write_bytes(self->uart.cfg.num, bytes, size);
    return CIOT_OK;
}

ciot_err_t ciot_uart_set_bridge_mode(ciot_uart_t self, bool mode)
{
    CIOT_NULL_CHECK(self);
    return ciot_s_set_bridge_mode(self->uart.s, mode);
}

ciot_err_t ciot_uart_task(ciot_uart_t self)
{
    return CIOT_ERR_NOT_IMPLEMENTED;
}

static void ciot_uart_event_handler(void *args)
{
    ciot_uart_t self = (ciot_uart_t)args;

    if (self == NULL) return;

    ciot_iface_event_t iface_event = {0};
    ciot_uart_status_t iface_status = self->uart.status;
    iface_event.iface = self->uart.iface.info;

    while (true)
    {
        if(xQueueReceive(self->queue, (void*)&self->event, portMAX_DELAY))
        {
            switch (self->event.type)
            {
            case UART_DATA:
                while (self->event.size)
                {
                    uint8_t byte;
                    uart_read_bytes(self->uart.cfg.num, &byte, 1, portMAX_DELAY);
                    self->event.size--;              
                    ciot_err_t err = ciot_s_process_byte(self->uart.s, byte);
                    if(err != CIOT_OK) {
                        ESP_LOGE(TAG, "Process byte error: %d", err);
                    }
                }
                return;
            case UART_FIFO_OVF:
                ESP_LOGE(TAG, "UART_FIFO_OVF");
                iface_status.error = CIOT_UART_ERR_FIFO_OVERFLOW;
                iface_status.state = CIOT_UART_STATE_INTERNAL_ERROR;
                iface_event.id = CIOT_IFACE_EVENT_ERROR;
                iface_event.data = (ciot_iface_event_data_u*)&iface_status;
                iface_event.size = sizeof(iface_status);
                uart_flush_input(self->uart.cfg.num);
                xQueueReset(self->queue);
                break;
            case UART_BUFFER_FULL:
                ESP_LOGE(TAG, "UART_BUFFER_FULL: %d", self->event.size);
                iface_status.error = CIOT_UART_ERR_BUFFER_FULL;
                iface_status.state = CIOT_UART_STATE_INTERNAL_ERROR;
                iface_event.id = CIOT_IFACE_EVENT_ERROR;
                iface_event.data = (ciot_iface_event_data_u*)&iface_status;
                iface_event.size = sizeof(iface_status);
                uart_flush_input(self->uart.cfg.num);
                xQueueReset(self->queue);
                break;
            case UART_BREAK:
                ESP_LOGE(TAG, "UART_BREAK");
                iface_status.error = CIOT_UART_ERR_BREAK;
                iface_status.state = CIOT_UART_STATE_INTERNAL_ERROR;
                iface_event.id = CIOT_IFACE_EVENT_ERROR;
                iface_event.data = (ciot_iface_event_data_u*)&iface_status;
                iface_event.size = sizeof(iface_status);
                break;
            case UART_FRAME_ERR:
                ESP_LOGE(TAG, "UART_FRAME_ERR");
                iface_status.error = CIOT_UART_ERR_FRAME;
                iface_status.state = CIOT_UART_STATE_INTERNAL_ERROR;
                iface_event.id = CIOT_IFACE_EVENT_ERROR;
                iface_event.data = (ciot_iface_event_data_u*)&iface_status;
                iface_event.size = sizeof(iface_status);
                break;
            default:
                ESP_LOGW(TAG, "unhandled event: %d", self->event.type);
                return;
            }

            self->uart.status = iface_status;

            if(self->uart.iface.event_handler)
            {
                self->uart.iface.event_handler(&self->uart.iface, &iface_event, self->uart.iface.event_args);
            }
        }
    }
}

#endif
