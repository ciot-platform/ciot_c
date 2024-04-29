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

#if CIOT_CONFIG_FEATURE_UART && defined(CIOT_TARGET_WIN)

#include <stdbool.h>
#include "stdio.h"
#include "windows.h"

#include "ciot_log.h"

struct ciot_uart
{
    ciot_uart_base_t uart;
    HANDLE handle;
    DCB params;
    COMMTIMEOUTS timeouts;
    DWORD bytes_written;
    DWORD bytes_read;
    char port_name[10];
};

static ciot_err_t ciot_uart_init(ciot_uart_t self);
static void ciot_uart_process_error(ciot_uart_t self, DWORD error);
static ciot_err_t ciot_uart_process_status(ciot_uart_t self, COMSTAT *status);
ciot_err_t ciot_uart_task_internal(ciot_iface_t *iface, ciot_s_t ciot_s);
ciot_err_t ciot_uart_event(ciot_uart_base_t *base, ciot_iface_event_type_t event);

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
        .iface = (ciot_iface_t*)self,
    };
    self->uart.s = ciot_s_new(&s_cfg);

    return self;
}

ciot_err_t ciot_uart_start(ciot_uart_t self, ciot_uart_cfg_t *cfg)
{
    CIOT_NULL_CHECK(self);
    CIOT_NULL_CHECK(cfg);

    self->uart.cfg = *cfg;
    sprintf(self->port_name, "\\\\.\\COM%d", self->uart.cfg.num);

    self->handle = CreateFile(self->port_name, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if(self->handle == INVALID_HANDLE_VALUE)
    {
        CIOT_LOGE(TAG, "Unable to open serial port %s", self->port_name);
        self->uart.status.state = CIOT_UART_STATE_INTERNAL_ERROR;
        self->uart.status.error = CIOT_UART_ERR_OPEN;
        ciot_uart_event(&self->uart, CIOT_IFACE_EVENT_ERROR);
        return CIOT_FAIL;
    }

    self->params.DCBlength = sizeof(self->params);
    if(!GetCommState(self->handle, &self->params))
    {
        CIOT_LOGE(TAG, "GetCommState failed at %s", self->port_name);
        CloseHandle(self->handle);
        self->uart.status.state = CIOT_UART_STATE_INTERNAL_ERROR;
        return CIOT_FAIL;
    }
    
    ciot_s_set_bridge_mode(self->uart.s, self->uart.cfg.bridge_mode);
    ciot_err_t err = ciot_uart_init(self);
    ciot_uart_process_error(self, err);
    return err;
}

ciot_err_t ciot_uart_stop(ciot_uart_t self)
{
    CIOT_LOGI(TAG, "UART_CLOSE port: %s", self->port_name);
    CloseHandle(self->handle);
    return CIOT_OK;
}

ciot_err_t ciot_uart_send_data(ciot_uart_t self, uint8_t *data, int size)
{
    CIOT_NULL_CHECK(self);
    CIOT_NULL_CHECK(data);
    return ciot_s_send(self->uart.s, (char*)data, size);
}

ciot_err_t ciot_uart_send_bytes(ciot_iface_t *iface, uint8_t *bytes, int size)
{
    ciot_uart_t self = (ciot_uart_t)iface;
    CIOT_NULL_CHECK(self);
    if(self->uart.status.state == CIOT_UART_STATE_STARTED)
    {
        CIOT_NULL_CHECK(bytes);
        WriteFile(self->handle, bytes, size, &self->bytes_written, NULL);
        return CIOT_OK;
    }
    else
    {
        CIOT_LOGE(TAG, "Port %s is closed", self->port_name);
        return CIOT_ERR_INVALID_STATE;
    }
}

ciot_err_t ciot_uart_set_bridge_mode(ciot_uart_t self, bool mode)
{
    CIOT_NULL_CHECK(self);
    self->uart.cfg.bridge_mode = mode;
    return ciot_s_set_bridge_mode(self->uart.s, mode);
}

static ciot_err_t ciot_uart_init(ciot_uart_t self)
{
    CIOT_NULL_CHECK(self);
    self->params.BaudRate = self->uart.cfg.baud_rate;
    self->params.ByteSize = 8;
    self->params.StopBits = ONESTOPBIT;
    self->params.Parity = self->uart.cfg.parity;
    self->params.fDtrControl = self->uart.cfg.dtr;
    if(SetCommState(self->handle, &self->params) == FALSE)
    {
        CIOT_LOGE(TAG, "SetCommState error at %s", self->port_name);
        return CIOT_FAIL;
    }

    self->timeouts.ReadIntervalTimeout = 50;
    self->timeouts.ReadTotalTimeoutConstant = 50;
    self->timeouts.ReadTotalTimeoutMultiplier = 20;
    self->timeouts.WriteTotalTimeoutConstant = 50;
    self->timeouts.WriteTotalTimeoutMultiplier = 20;
    if(SetCommTimeouts(self->handle, &self->timeouts) == FALSE)
    {
        CIOT_LOGE(TAG, "SetCommTimeouts error at %s", self->port_name);
        return CIOT_FAIL;
    }

    return CIOT_OK;
}

ciot_err_t ciot_uart_task(ciot_uart_t self)
{
    CIOT_NULL_CHECK(self);
    CIOT_NULL_CHECK(self->handle);

    DWORD error;
    COMSTAT status;
    ClearCommError(self->handle, &error, &status);

    ciot_uart_process_error(self, error);
    ciot_uart_process_status(self, &status);
    return ciot_uart_task_internal(&self->uart.iface, self->uart.s);
}

static void ciot_uart_process_error(ciot_uart_t self, DWORD error)
{
    if(self->uart.status.state == CIOT_UART_STATE_CLOSED && error == 0)
    {
        ciot_iface_event_t iface_event = { 0 };
        ciot_uart_status_msg_t status_msg = { 0 };

        status_msg.header.iface = self->uart.iface.info;
        iface_event.data = (ciot_iface_event_data_u*)&status_msg;
        iface_event.size = sizeof(status_msg);

        self->uart.status.state = CIOT_UART_STATE_STARTED;
        status_msg.header.type = CIOT_MSG_TYPE_START;
        status_msg.status = self->uart.status;
        iface_event.type = CIOT_IFACE_EVENT_STARTED;

        if(self->uart.iface.event_handler != NULL)
        {
            self->uart.iface.event_handler(&self->uart.iface, &iface_event, self->uart.iface.event_args);
        }
        CIOT_LOGI(TAG, "UART_OPEN port:%s", self->port_name);
    }

    if(self->uart.status.state == CIOT_UART_STATE_STARTED && error == CE_FRAME)
    {
        ciot_iface_event_t iface_event = { 0 };
        ciot_uart_status_msg_t status_msg = { 0 };

        status_msg.header.iface = self->uart.iface.info;
        iface_event.data = (ciot_iface_event_data_u*)&status_msg;
        iface_event.size = sizeof(status_msg);

        self->uart.status.state = CIOT_UART_STATE_CLOSED;
        status_msg.header.type = CIOT_MSG_TYPE_STOP;
        status_msg.status = self->uart.status;
        iface_event.type = CIOT_IFACE_EVENT_STOPPED;

        if(self->uart.iface.event_handler != NULL)
        {
            self->uart.iface.event_handler(&self->uart.iface, &iface_event, self->uart.iface.event_args);
        }
        CIOT_LOGI(TAG, "UART_CLOSED port:%s", self->port_name);
    }
}

static ciot_err_t ciot_uart_process_status(ciot_uart_t self, COMSTAT *status)
{
    if(self->uart.status.state != CIOT_UART_STATE_STARTED)
    {
        return CIOT_ERR_INVALID_STATE;
    }

    if(status->cbInQue > 0)
    {
        uint8_t byte;
        while (status->cbInQue > 0)
        {
            if(ReadFile(self->handle, &byte, 1, &self->bytes_read, NULL))
            {
                ciot_err_t err = ciot_s_process_byte(self->uart.s, byte);
                if(err != CIOT_OK)
                {
                    CIOT_LOGE(TAG, "Error %s processing byte %d", ciot_err_to_message(err), byte);
                }
            }
            status->cbInQue--;
        }
    }
}

#endif
