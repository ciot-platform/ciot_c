/**
 * @file main.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-05-16
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "main.h"
#include "parser.h"

ciot_msg_data_t http_server_cfg = {
    .which_type = CIOT_MSG_DATA_HTTP_SERVER_TAG,
    .http_server = {
        .which_type = CIOT_HTTP_SERVER_DATA_CONFIG_TAG,
        .config = {
            .route = "/v1/ota",
            .port = 5151,
        },
    },
};

ciot_uart_cfg_t uart_cfg = {};

ciot_http_client_cfg_t http_client_cfg = {
    .transport = CIOT_HTTP_TRANSPORT_TCP,
    .method = CIOT_HTTP_METHOD_POST,
    .timeout = 20000,
};

static const char *TAG = "ciota";

ciot_err_t iface_event_handler(ciot_iface_t *sender, ciot_event_t *event, void *data);
ciot_err_t ciota_get_status_req(ciota_t *self);
ciot_err_t ciota_start_sender(ciota_t *self);
ciot_err_t ciota_start_req(ciota_t *self);
ciot_err_t ciota_send_req(ciota_t *self, ciot_msg_t *req);

ciot_err_t ciota_start(ciota_t *self)
{
    self->ifaces.ciot = ciot_new();
    self->ifaces.list[IFACE_ID_CIOT] = (ciot_iface_t *)self->ifaces.ciot;
    self->ifaces.cfgs[IFACE_ID_CIOT] = NULL;

    self->ifaces.sys = ciot_sys_new(CIOT_HANDLE);
    self->ifaces.list[IFACE_ID_CIOT_SYS] = (ciot_iface_t *)self->ifaces.sys;
    self->ifaces.cfgs[IFACE_ID_CIOT_SYS] = NULL;

    self->ifaces.http_server = ciot_http_server_new(CIOT_HANDLE);
    self->ifaces.list[IFACE_ID_CIOT_HTTP_SERVER] = (ciot_iface_t *)self->ifaces.http_server;
    self->ifaces.cfgs[IFACE_ID_CIOT_HTTP_SERVER] = &http_server_cfg;

    self->ifaces.http_client = ciot_http_client_new(CIOT_HANDLE);
    self->ifaces.list[IFACE_ID_CIOT_HTTP_CLIENT] = (ciot_iface_t *)self->ifaces.http_client;
    self->ifaces.cfgs[IFACE_ID_CIOT_HTTP_CLIENT] = NULL;

    ciot_iface_set_event_handler((ciot_iface_t *)self->ifaces.ciot, iface_event_handler, self);

    ciot_cfg_t ciot_cfg = {
        .ifaces.list = self->ifaces.list,
        .ifaces.cfgs = self->ifaces.cfgs,
        .ifaces.count = IFACE_ID_MAX,
    };
    return ciot_start(self->ifaces.ciot, &ciot_cfg);
}

ciot_err_t ciota_task(ciota_t *self)
{
    ciot_err_t err = ciot_task(self->ifaces.ciot);
    if (err == CIOT_ERR_OK)
    {
        switch (self->state)
        {
        case CIOTA_STATE_GET_OTA_STATUS:
            err = ciota_get_status_req(self);
            if (err == CIOT_ERR_OK)
            {
                self->state = CIOTA_STATE_WAIT_OTA_STATUS;
            }
            break;
        case CIOTA_STATE_START:
            err = ciota_start_req(self);
            if (err == CIOT_ERR_OK)
            {
                self->state = CIOTA_STATE_IN_PROGRESS;
            }
            break;
        case CIOTA_STATE_CHECK_PROGRESS:
            if(ciot_timer_compare(&self->timer_check_progress, 1000))
            {
                err = ciota_get_status_req(self);
            }
            break;
        case CIOTA_STATE_ERROR:
            err = CIOT_ERR_FAIL;
            break;
        default:
            break;
        }
    }
    return err;
}

int main(int argc, char **argv)
{
    ciota_t self = {0};

    if (argc == 1)
    {
        static char *argv_test[] = {
            "ciota",
            "--host",
            "http://192.168.4.2",
            "--dir",
            "./",
            "--filename",
            "test.bin",
            "--sender",
            "http",
            "--iface-id",
            "8",
            "--url",
            "http://192.168.4.1:80/v3/hg",
        };

        argc = sizeof(argv_test) / sizeof(argv_test[0]);
        argv = argv_test;
    }

    ciot_err_t err = parse_args(argc, argv, &self.cfg);
    if (err != CIOT_ERR_OK)
    {
        CIOT_LOGE(TAG, "Error parsing arguments: %s", ciot_err_to_message(err));
    }

    strcpy(http_server_cfg.http_server.config.root, self.cfg.dir);
    strcpy(http_server_cfg.http_server.config.address, self.cfg.host);

    err = ciota_start(&self);
    if (err != CIOT_ERR_OK)
    {
        CIOT_LOGE(TAG, "Error starting OTA server: %s", ciot_err_to_message(err));
        return err;
    }

    while (1)
    {
        err = ciota_task(&self);
        if (err != CIOT_ERR_OK)
        {
            CIOT_LOGE(TAG, "Error in OTA server task: %s", ciot_err_to_message(err));
            return err;
        }
    }
}

ciot_err_t iface_event_handler(ciot_iface_t *sender, ciot_event_t *event, void *data)
{
    ciota_t *self = (ciota_t *)data;

    if (sender->info.type == CIOT_IFACE_TYPE_CIOT && event->type == CIOT_EVENT_TYPE_STARTED)
    {
        ciot_err_t err = ciota_start_sender(self);
        if(err == CIOT_ERR_OK)
        {
            self->state = CIOTA_STATE_GET_OTA_STATUS;
        }
        else
        {
            CIOT_LOGE(TAG, "Error starting sender: %s", ciot_err_to_message(err));
            self->state = CIOTA_STATE_ERROR;
            return err;
        }
    }

    if (sender->info.type == CIOT_IFACE_TYPE_HTTP_SERVER && event->type == CIOT_EVENT_TYPE_ERROR)
    {
        CIOT_LOGE(TAG, "Error in HTTP server: %s", ciot_err_to_message(event->msg.error));
        self->state = CIOTA_STATE_ERROR;
        return event->msg.error;
    }

    if (sender->info.type == CIOT_IFACE_TYPE_HTTP_CLIENT && event->type == CIOT_EVENT_TYPE_ERROR)
    {
        CIOT_LOGE(TAG, "Error in HTTP client: %s", ciot_err_to_message(event->msg.error));
        self->state = CIOTA_STATE_ERROR;
        return event->msg.error;
    }

    if (sender->info.type == CIOT_IFACE_TYPE_HTTP_CLIENT && event->type == CIOT_EVENT_TYPE_DONE)
    {
        if(event->msg.error != CIOT_ERR_OK)
        {
            CIOT_LOGE(TAG, "Error in HTTP client request: %s", ciot_err_to_message(event->msg.error));
            self->state = CIOTA_STATE_ERROR;
            return event->msg.error;
        }
        else if(event->msg.data.which_type == CIOT_MSG_DATA_OTA_TAG)
        {
            if(event->msg.data.ota.which_type == CIOT_OTA_DATA_STATUS_TAG)
            {
                if(self->state == CIOTA_STATE_WAIT_OTA_STATUS)
                {
                    self->state = CIOTA_STATE_START;
                }
                if(self->state == CIOTA_STATE_IN_PROGRESS)
                {
                    if(event->msg.data.ota.status.state == CIOT_OTA_STATE_START) {
                        CIOT_LOGI(TAG, "OTA started");
                        ciota_get_status_req(self);
                        self->state = CIOTA_STATE_CHECK_PROGRESS;
                    }
                    if(event->msg.data.ota.status.state == CIOT_OTA_STATE_FLASHING) {
                        CIOT_LOGI(TAG, "OTA in progress");
                    }
                    if(event->msg.data.ota.status.state == CIOT_OTA_STATE_ERROR) {
                        if(event->msg.data.ota.status.error >= 0x7000 && event->msg.data.ota.status.error <= 0x700A) {
                            CIOT_LOGE(TAG, "OTA HTTP error: 0x%x", event->msg.data.ota.status.error);
                        } 
                        if(event->msg.data.ota.status.error == -1) {
                            CIOT_LOGE(TAG, "OTA Failed");
                        }
                        self->state = CIOTA_STATE_ERROR;
                    }
                }
            }
        }
    }

    return CIOT_ERR_OK;
}

ciot_err_t ciota_start_sender(ciota_t *self)
{
    switch (self->cfg.sender_type)
    {
    case SENDER_TYPE_UART:
        uart_cfg.baud_rate = self->cfg.baudrate;
        uart_cfg.num = self->cfg.port;
        return ciot_uart_start(self->ifaces.uart, &uart_cfg);
    case SENDER_TYPE_HTTP_CLIENT:
        strcpy(http_client_cfg.url, self->cfg.url);
        return ciot_http_client_start(self->ifaces.http_client, &http_client_cfg);
    default:
        return CIOT_ERR_INVALID_TYPE;
    }
}

ciot_err_t ciota_get_status_req(ciota_t *self)
{
    ciot_msg_t msg = {
        .has_iface = true,
        .iface.id = self->cfg.iface_id,
        .iface.type = CIOT_IFACE_TYPE_OTA,
        .has_data = true,
        .data.which_type = CIOT_MSG_DATA_GET_DATA_TAG,
        .data.get_data.type = CIOT_DATA_TYPE_STATUS,
    };
    return ciota_send_req(self, &msg);
}

ciot_err_t ciota_start_req(ciota_t *self)
{
    ciot_msg_t msg = {
        .has_iface = true,
        .iface.id = self->cfg.iface_id,
        .iface.type = CIOT_IFACE_TYPE_OTA,
        .has_data = true,
        .data.which_type = CIOT_MSG_DATA_OTA_TAG,
        .data.ota.which_type = CIOT_OTA_DATA_CONFIG_TAG,
        .data.ota.config = {
            .force = self->cfg.force,
            .type = self->cfg.type,
            .restart = self->cfg.restart,
        },
    };
    sprintf(msg.data.ota.config.url, "%s:%d/%s", http_server_cfg.http_server.config.address, http_server_cfg.http_server.config.port, self->cfg.filename);
    return ciota_send_req(self, &msg);
}

ciot_err_t ciota_send_req(ciota_t *self, ciot_msg_t *req)
{
    switch (self->cfg.sender_type)
    {
    case SENDER_TYPE_UART:
        return ciot_iface_send_req(self->ifaces.list[IFACE_ID_CIOT_UART], req);
    case SENDER_TYPE_HTTP_CLIENT:
        return ciot_iface_send_req(self->ifaces.list[IFACE_ID_CIOT_HTTP_CLIENT], req);
    default:
        return CIOT_ERR_INVALID_TYPE;
    }
}
