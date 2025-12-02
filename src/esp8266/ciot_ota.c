/**
 * @file ciot_ota.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-06-07
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "ciot_config.h"

#if CIOT_CONFIG_FEATURE_OTA == 1 && defined(CIOT_PLATFORM_ESP8266)

#include <stdlib.h>

#include "esp_https_ota.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_ota_ops.h"

#include "ciot_ota.h"
#include "ciot_err.h"

struct ciot_ota
{
    ciot_ota_base_t base;
};

static const char *TAG = "ciot_ota";

static esp_err_t ciot_ota_event_handler(esp_http_client_event_t *evt);

ciot_ota_t ciot_ota_new(void *handle)
{
    ciot_ota_t self = calloc(1, sizeof(struct ciot_ota));
    ciot_ota_init(self);
    return self;
}

ciot_err_t ciot_ota_start(ciot_ota_t self, ciot_ota_cfg_t *cfg)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(cfg);

    ciot_ota_base_t *base = &self->base;

    if (base->status.state != CIOT_OTA_STATE_IDLE)
        return CIOT_ERR_INVALID_STATE;

    base->cfg = *cfg;
    base->status.state = CIOT_OTA_STATE_INIT;
    base->status.error = CIOT_ERR_OK;

    esp_http_client_config_t config = {
        .url = cfg->url,
        .cert_pem = NULL,
        .event_handler = ciot_ota_event_handler,
        .user_data = self,
    };

    esp_err_t err = esp_https_ota(&config);
    if (err == ESP_OK)
    {
        base->status.state = CIOT_OTA_STATE_STATE_DONE;
        if (cfg->restart)
        {
            esp_restart();
        }
    }
    else
    {
        base->status.state = CIOT_OTA_STATE_ERROR;
        base->status.error = CIOT_ERR_FAIL;
    }

    return base->status.error;
}

ciot_err_t ciot_ota_stop(ciot_ota_t self)
{
    CIOT_ERR_NULL_CHECK(self);
    return CIOT_ERR_NOT_IMPLEMENTED;
}

ciot_err_t ciot_ota_rollback(ciot_ota_t self)
{
    return CIOT_ERR_NOT_IMPLEMENTED;
}

static esp_err_t ciot_ota_event_handler(esp_http_client_event_t *evt)
{
    ciot_ota_t self = (ciot_ota_t)evt->user_data;
    ciot_ota_base_t *base = &self->base;
    ciot_ota_status_t *status = &base->status;

    switch (evt->event_id)
    {
    case HTTP_EVENT_ON_CONNECTED:
        status->state = CIOT_OTA_STATE_CONNECTED;
        ciot_iface_send_event_type(&base->iface, CIOT_EVENT_TYPE_INTERNAL);
        break;
    case HTTP_EVENT_HEADER_SENT:
        status->state = CIOT_OTA_STATE_CHECKING_DATA;
        ciot_iface_send_event_type(&base->iface, CIOT_EVENT_TYPE_INTERNAL);
        break;
    case HTTP_EVENT_ON_DATA:
        status->state = CIOT_OTA_STATE_FLASHING;
        status->image_written += evt->data_len;
        ciot_iface_send_event_type(&base->iface, CIOT_EVENT_TYPE_INTERNAL);
        break;
    case HTTP_EVENT_ON_FINISH:
        status->state = CIOT_OTA_STATE_STATE_DONE;
        ciot_iface_send_event_type(&base->iface, CIOT_EVENT_TYPE_DONE);
        break;
    case HTTP_EVENT_DISCONNECTED:
    case HTTP_EVENT_ERROR:
        status->state = CIOT_OTA_STATE_ERROR;
        ciot_iface_send_event_type(&base->iface, CIOT_EVENT_TYPE_ERROR);
        break;
    default:
        break;
    }

    return ESP_OK;
}

#endif // CIOT_CONFIG_FEATURE_OTA == 1
