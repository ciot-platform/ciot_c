/**
 * @file ciot_ntp.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "ciot_config.h"

#if CIOT_CONFIG_FEATURE_NTP == 1 && defined(CIOT_PLATFORM_ESP32)

#include <stdlib.h>
#include "ciot_ntp.h"
#include "ciot_timer.h"
#include "esp_sntp.h"

static const char *TAG = "ciot_ntp";

struct ciot_ntp
{
    ciot_ntp_base_t base;
};

void *notif_args;

static void ciot_ntp_sync_notification_cb(struct timeval *tv);

ciot_ntp_t ciot_ntp_new(void *handle)
{
    ciot_ntp_t self = calloc(1, sizeof(struct ciot_ntp));
    ciot_ntp_init(self);
    return self;
}

ciot_err_t ciot_ntp_start(ciot_ntp_t self, ciot_ntp_cfg_t *cfg)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(cfg);

    ciot_ntp_base_t *base = &self->base;

    CIOT_LOGI(TAG, "Starting NTP");
    CIOT_LOGI(TAG, "operating_mode: %d", (int)base->cfg.op_mode);
    CIOT_LOGI(TAG, "sync_interval: %d", (int)base->cfg.sync_interval);
    CIOT_LOGI(TAG, "server1: %s", base->cfg.server1);
    CIOT_LOGI(TAG, "server2: %s", base->cfg.server2);
    CIOT_LOGI(TAG, "server3: %s", base->cfg.server3);

    if(base->status.init)
    {
        esp_sntp_stop();
    }

    base->cfg = *cfg;

    if(base->cfg.server1[0] != '\0') esp_sntp_setservername(0, base->cfg.server1);
    if(base->cfg.server2[0] != '\0') esp_sntp_setservername(1, base->cfg.server2);
    if(base->cfg.server3[0] != '\0') esp_sntp_setservername(2, base->cfg.server3);
    if(base->cfg.sync_interval == 0) base->cfg.sync_interval = 3600 * 1000;

    esp_sntp_setoperatingmode(base->cfg.op_mode);
    esp_sntp_set_sync_interval(base->cfg.sync_interval);
    esp_sntp_set_sync_mode(base->cfg.sync_mode);
    esp_sntp_set_time_sync_notification_cb(ciot_ntp_sync_notification_cb);

    notif_args = self;
    esp_sntp_init();
    base->status.init = true;

    if(self->base.cfg.timezone[0] != '\0')
    {
        setenv("TZ", self->base.cfg.timezone, 1);
        tzset();
    }

    if (!esp_sntp_enabled()) {
        CIOT_LOGE(TAG, "Failed to initialize SNTP");
        return CIOT_ERR_FAIL;
    }

    return CIOT_ERR_OK;
}

ciot_err_t ciot_ntp_stop(ciot_ntp_t self)
{
    CIOT_ERR_NULL_CHECK(self);
    esp_sntp_stop();
    ciot_iface_send_event_type(&self->base.iface, CIOT_EVENT_TYPE_STOPPED);
    return CIOT_ERR_OK;
}

static void ciot_ntp_sync_notification_cb(struct timeval *tv)
{
    CIOT_LOGI(TAG, "Started");
    if(notif_args == NULL) return;
    ciot_ntp_t self = (ciot_ntp_t)notif_args;
    ciot_ntp_base_t *base = &self->base;
    base->status.sync = 1;
    base->status.sync_count++;
    base->status.last_sync = ciot_timer_now();
    ciot_iface_send_event_type(&base->iface, CIOT_EVENT_TYPE_STARTED);
}

#endif  //!CIOT_CONFIG_FEATURE_NTP == 1