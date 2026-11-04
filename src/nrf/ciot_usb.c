/**
 * @file ciot_usb.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "ciot_config.h"

#if CIOT_CONFIG_FEATURE_USB && defined(CIOT_PLATFORM_NRF)

#include <stdlib.h>
#include "app_usbd_cdc_acm.h"
#include "app_fifo.h"
#include "app_usbd_serial_num.h"
#include "nrf_drv_clock.h"
#include "app_util_platform.h"
#include "ciot_usb.h"
#include "ciot_uart.h"
#include "ciot_err.h"
#include "ciot_timer.h"

static const char *TAG = "ciot_usb";

#define CDC_ACM_COMM_INTERFACE 0
#define CDC_ACM_COMM_EPIN NRF_DRV_USBD_EPIN2

#define CDC_ACM_DATA_INTERFACE 1
#define CDC_ACM_DATA_EPIN NRF_DRV_USBD_EPIN1
#define CDC_ACM_DATA_EPOUT NRF_DRV_USBD_EPOUT1

// Bytes handed to the CDC class per transfer (one full-speed bulk packet).
#ifndef CIOT_CONFIG_USB_TX_CHUNK_SIZE
#define CIOT_CONFIG_USB_TX_CHUNK_SIZE NRF_DRV_USBD_EPSIZE
#endif

static void ciot_usbd_event_handler(app_usbd_event_type_t event);
static void ciot_cdc_acm_event_handler(app_usbd_class_inst_t const *p_inst, app_usbd_cdc_acm_user_event_t event);
static void ciot_usb_tx_next(ciot_usb_t self);

APP_USBD_CDC_ACM_GLOBAL_DEF(m_app_cdc_acm,
                            ciot_cdc_acm_event_handler,
                            CDC_ACM_COMM_INTERFACE,
                            CDC_ACM_DATA_INTERFACE,
                            CDC_ACM_COMM_EPIN,
                            CDC_ACM_DATA_EPIN,
                            CDC_ACM_DATA_EPOUT,
                            APP_USBD_CDC_COMM_PROTOCOL_AT_V250);

typedef struct ciot_usb_fifo
{
    app_fifo_t tx;
    uint8_t tx_buf[CIOT_CONFIG_UART_TX_BUF_SIZE];
} ciot_usb_fifo_t;

struct ciot_usb
{
    ciot_usb_base_t base;
    ciot_usb_fifo_t fifo;
    uint8_t rx_byte[1];
    bool tx_in_progress;
    const app_usbd_class_inst_t *usb;
};

static ciot_usb_t usb;

// Static: there is a single CDC instance, and the buffer stays off the heap.
static uint8_t m_tx_chunk[CIOT_CONFIG_USB_TX_CHUNK_SIZE];

ciot_usb_t ciot_usb_new(void *handle)
{
    ciot_usb_t self = calloc(1, sizeof(struct ciot_usb));
    ciot_usb_init(self);
    usb = self;
    return self;
}

ciot_err_t ciot_usb_start(ciot_usb_t self, ciot_usb_cfg_t *cfg)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(cfg);

    ret_code_t ret;

    self->base.cfg = *cfg;

    ret = app_fifo_init(&self->fifo.tx, self->fifo.tx_buf, CIOT_CONFIG_UART_TX_BUF_SIZE);
    VERIFY_SUCCESS(ret);

    static const app_usbd_config_t usbd_config = {
        .ev_state_proc = ciot_usbd_event_handler,
    };

    ret = nrf_drv_clock_init();
    if(ret != NRF_ERROR_MODULE_ALREADY_INITIALIZED)
    {
        APP_ERROR_CHECK(ret);
    }

    nrf_drv_clock_lfclk_request(NULL);

    while (!nrf_drv_clock_lfclk_is_running())
    {
        // wait...
    }

    app_usbd_serial_num_generate();

    ret = app_usbd_init(&usbd_config);
    APP_ERROR_CHECK(ret);

    self->usb = app_usbd_cdc_acm_class_inst_get(&m_app_cdc_acm);
    ret = app_usbd_class_append(self->usb);
    APP_ERROR_CHECK(ret);

    #if APP_USBD_CONFIG_POWER_EVENTS_PROCESS
    ret = app_usbd_power_events_enable();
    APP_ERROR_CHECK(ret);
    #else
    app_usbd_enable();
    app_usbd_start();
    #endif  //APP_USBD_CONFIG_POWER_EVENTS_PROCESS

    self->tx_in_progress = false;

    // ciot_event_t event = {0};
    // event.type = CIOT_EVENT_TYPE_STARTED;
    // event.msg = ciot_msg_get(CIOT__MSG_TYPE__MSG_TYPE_STATUS, &self->base.iface);
    // ciot_iface_send_event(&self->base.iface, &event);
    ciot_iface_send_event_type(&self->base.iface, CIOT_EVENT_TYPE_STARTED);

    return CIOT_ERR_OK;
}

ciot_err_t ciot_usb_stop(ciot_usb_t self)
{
    CIOT_ERR_NULL_CHECK(self);
    return CIOT_ERR_NOT_IMPLEMENTED;
}

ciot_err_t ciot_usb_task(ciot_usb_t self)
{
    CIOT_ERR_NULL_CHECK(self);
    app_usbd_event_queue_process();
    return CIOT_ERR_OK;
}

// All or nothing: the bytes are queued only if all of them fit in the TX FIFO. Otherwise nothing
// is queued and CIOT_ERR_OVERFLOW is returned, so a frame never goes out partially.
ciot_err_t ciot_usb_send_bytes(ciot_usb_t self, uint8_t *bytes, int size)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(bytes);

    if(self->base.status.state != CIOT_USB_STATE_STARTED)
    {
        return CIOT_ERR_INVALID_STATE;
    }

    if(size <= 0)
    {
        return CIOT_ERR_OK;
    }

    ciot_err_t err = CIOT_ERR_OK;

    // Keeps the free space check, the write and the TX start together when senders run at different priorities.
    CRITICAL_REGION_ENTER();
    uint32_t len = 0;
    app_fifo_write(&self->fifo.tx, NULL, &len);
    if(len < (uint32_t)size)
    {
        err = CIOT_ERR_OVERFLOW;
    }
    else
    {
        len = size;
        app_fifo_write(&self->fifo.tx, bytes, &len);
        if(!self->tx_in_progress)
        {
            ciot_usb_tx_next(self);
        }
    }
    CRITICAL_REGION_EXIT();

    return err;
}

// Starts the transfer of the next chunk of the TX FIFO. The bytes leave the FIFO only after the
// CDC class accepts them, so a refused write (e.g. port closed) keeps them for the next try.
static void ciot_usb_tx_next(ciot_usb_t self)
{
    uint32_t len = 0;
    app_fifo_read(&self->fifo.tx, NULL, &len);
    if(len == 0)
    {
        self->tx_in_progress = false;
        return;
    }
    if(len > sizeof(m_tx_chunk))
    {
        len = sizeof(m_tx_chunk);
    }
    for(uint32_t i = 0; i < len; i++)
    {
        app_fifo_peek(&self->fifo.tx, i, &m_tx_chunk[i]);
    }
    if(app_usbd_cdc_acm_write(&m_app_cdc_acm, m_tx_chunk, len) == NRF_SUCCESS)
    {
        for(uint32_t i = 0; i < len; i++)
        {
            uint8_t byte;
            app_fifo_get(&self->fifo.tx, &byte);
        }
        self->tx_in_progress = true;
    }
    else
    {
        self->tx_in_progress = false;
    }
}

static void ciot_usbd_event_handler(app_usbd_event_type_t event)
{
    ciot_usb_t self = usb;
    ciot_usb_base_t *base = &usb->base;

    if (self == NULL) return;

    ciot_event_t ciot_event = {0};

    switch (event)
    {
    case APP_USBD_EVT_DRV_RESET:
        CIOT_LOGI(TAG, "APP_USBD_EVT_DRV_RESET");
        break;
    case APP_USBD_EVT_DRV_SUSPEND:
        CIOT_LOGI(TAG, "APP_USBD_EVT_DRV_SUSPEND");
        break;
    case APP_USBD_EVT_DRV_RESUME:
        CIOT_LOGI(TAG, "APP_USBD_EVT_DRV_RESUME");
        break;
    case APP_USBD_EVT_POWER_DETECTED:
        CIOT_LOGI(TAG, "APP_USBD_EVT_POWER_DETECTED");
        if (!nrf_drv_usbd_is_enabled())
        {
            self->tx_in_progress = false;
            app_usbd_enable();
        }
        ciot_event.type = CIOT_EVENT_TYPE_INTERNAL;
        break;
    case APP_USBD_EVT_POWER_REMOVED:
        CIOT_LOGI(TAG, "APP_USBD_EVT_POWER_REMOVED");
        app_usbd_stop();
        self->tx_in_progress = false;
        ciot_event.type = CIOT_EVENT_TYPE_INTERNAL;
        break;
    case APP_USBD_EVT_POWER_READY:
        CIOT_LOGI(TAG, "APP_USBD_EVT_POWER_READY");
        app_usbd_start();
        self->tx_in_progress = false;
        ciot_event.type = CIOT_EVENT_TYPE_STARTED;
        break;
    case APP_USBD_EVT_STARTED:
        CIOT_LOGI(TAG, "APP_USBD_EVT_STARTED");
        base->status.state = CIOT_USB_STATE_STARTED;
        break;
    case APP_USBD_EVT_STOPPED:
        CIOT_LOGI(TAG, "APP_USBD_EVT_STOPPED");
        app_usbd_disable();
        base->status.state = CIOT_USB_STATE_STOPPED;
        self->tx_in_progress = false;
        ciot_event.type = CIOT_EVENT_TYPE_STOPPED;
        break;
    case APP_USBD_EVT_STATE_CHANGED:
        CIOT_LOGI(TAG, "APP_USBD_EVT_STATE_CHANGED");
        break;
    default:
        CIOT_LOGI(TAG, "APP_USB_EVT_UNKNOWN %d", event);
        return;
    }

    // ciot_event.msg = ciot_msg_get(CIOT__MSG_TYPE__MSG_TYPE_STATUS, &base->iface);
    ciot_usb_get_status(self, &ciot_event.msg.data.usb.status);
    ciot_iface_send_event(&base->iface, &ciot_event);
}

static void ciot_cdc_acm_event_handler(app_usbd_class_inst_t const *p_inst, app_usbd_cdc_acm_user_event_t event)
{
    ciot_usb_t self = usb;

    switch (event)
    {
    case APP_USBD_CDC_ACM_USER_EVT_RX_DONE:
    {
        ret_code_t ret;
        do
        {
            ciot_iface_process_data(&self->base.iface, self->rx_byte, 1, CIOT_EVENT_TYPE_MSG);
            ret = app_usbd_cdc_acm_read(&m_app_cdc_acm,
                                        self->rx_byte,
                                        1);
        } while (ret == NRF_SUCCESS);
        break;
    }
    case APP_USBD_CDC_ACM_USER_EVT_TX_DONE:
    {
        CRITICAL_REGION_ENTER();
        ciot_usb_tx_next(self);
        CRITICAL_REGION_EXIT();
        break;
    }
    case APP_USBD_CDC_ACM_USER_EVT_PORT_OPEN:
    {
        app_usbd_cdc_acm_read(&m_app_cdc_acm, &self->rx_byte, 1);
        // Sends what was queued while the port was closed.
        CRITICAL_REGION_ENTER();
        ciot_usb_tx_next(self);
        CRITICAL_REGION_EXIT();
        break;
    }
    default:
        break;
    }
}

#endif  //!CIOT_CONFIG_FEATURE_USB
