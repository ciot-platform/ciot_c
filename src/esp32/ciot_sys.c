/**
 * @file ciot_sys.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-10-22
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "ciot_sys.h"

#if CIOT_CONFIG_FEATURE_SYSTEM && defined(CIOT_TARGET_ESP32)

#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"

#include "esp_system.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_timer.h"

#include "ciot_config.h"
#include "ciot_timer.h"

struct ciot_sys
{
    ciot_iface_t iface;
    ciot_sys_cfg_t cfg;
    ciot_sys_status_t status;
    time_t init_time;
    uint64_t reset_scheduled;
    EventGroupHandle_t event_group;
};

// static ciot_sys_t sys;

RTC_NOINIT_ATTR int rst_count;

static void ciot_sys_init(ciot_sys_t self);

ciot_sys_t ciot_sys_new(void *handle)
{
    ciot_sys_t self = calloc(1, sizeof(struct ciot_sys));
    self->iface.base.ptr = self;
    self->iface.base.start = (ciot_iface_start_fn *)ciot_sys_start;
    self->iface.base.stop = (ciot_iface_stop_fn *)ciot_sys_stop;
    self->iface.base.process_req = (ciot_iface_process_req_fn *)ciot_sys_process_req;
    self->iface.base.send_data = (ciot_iface_send_data_fn *)ciot_sys_send_data;
    self->iface.base.cfg.ptr = &self->cfg;
    self->iface.base.cfg.size = sizeof(self->cfg);
    self->iface.base.status.ptr = &self->status;
    self->iface.base.status.size = sizeof(ciot_sys_status_t);
    self->iface.info.type = CIOT_IFACE_TYPE_SYSTEM;
    self->event_group = xEventGroupCreate();
    ciot_sys_init(self);
    return self;
}

ciot_err_t ciot_sys_start(ciot_sys_t self, ciot_sys_cfg_t *cfg)
{
    return CIOT_OK;
}

ciot_err_t ciot_sys_stop(ciot_sys_t self)
{
    return CIOT_ERR_NOT_SUPPORTED;
}

ciot_err_t ciot_sys_process_req(ciot_sys_t self, ciot_sys_req_t *req)
{
    switch (req->type)
    {
    case CIOT_SYS_REQ_UNKNONW:
        return CIOT_ERR_INVALID_TYPE;
    case CIOT_SYS_REQ_RESTART:
        self->iface.base.req.status = CIOT_IFACE_REQ_STATUS_IDLE;
        self->iface.base.req.response_size++;
#ifdef CIOT_CONFIG_FEATURE_TIMER
        self->reset_scheduled = ciot_timer_get() + 5;
#else
        ciot_sys_rst(self);
#endif
        return CIOT_OK;
    }

    return CIOT_ERR_INVALID_TYPE;
}

ciot_err_t ciot_sys_send_data(ciot_sys_t self, uint8_t *data, int size)
{
    return CIOT_ERR_NOT_SUPPORTED;
}

ciot_err_t ciot_sys_rst(ciot_sys_t self)
{
    esp_restart();
    return CIOT_OK;
}

ciot_err_t ciot_sys_task(ciot_sys_t self)
{
    CIOT_NULL_CHECK(self);
    self->status.free_memory = esp_get_free_heap_size();
    self->status.lifetime = time(NULL) - self->init_time;
    xEventGroupWaitBits(self->event_group, CIOT_SYS_EVT_BIT_POOLING, pdTRUE, pdTRUE, 1000 / portTICK_PERIOD_MS);
#if CIOT_CONFIG_FEATURE_TIMER
    if(self->reset_scheduled > 0 && ciot_timer_compare(&self->reset_scheduled, 0))
    {
        ciot_sys_rst(self);
    }
    return CIOT_OK;
#endif
}

ciot_err_t ciot_sys_set_event_bits(ciot_sys_t self, int event_bits)
{
    CIOT_NULL_CHECK(self);
    xEventGroupSetBits(self->event_group, event_bits);
    return CIOT_OK;
}

void ciot_sys_sleep(long ms)
{
    vTaskDelay(pdMS_TO_TICKS(ms));
}

static void ciot_sys_init(ciot_sys_t self)
{
    char hw_name[] = CIOT_CONFIG_HARDWARE_NAME;
    uint8_t app_ver[] = {CIOT_CONFIG_APP_VER};

    ciot_sys_update_features(&self->status.info.features);

    if (esp_reset_reason() == ESP_RST_POWERON || esp_reset_reason() == ESP_RST_SW)
    {
        rst_count = 0;
    }

    rst_count++;
    self->status.rst_reason = esp_reset_reason();
    self->status.rst_count = rst_count;
    self->status.info.hardware = ciot_sys_get_hw();

    memcpy(self->status.info.hw_name, hw_name, sizeof(hw_name));
    memcpy(self->status.info.app_ver, app_ver, sizeof(app_ver));
}

#endif
