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

#include "esp_system.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_timer.h"

#include "ciot_config.h"

struct ciot_sys
{
    ciot_iface_t iface;
    ciot_sys_cfg_t cfg;
    ciot_sys_status_t status;
    time_t init_time;
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
    ciot_sys_init(self);
    return self;
}

ciot_err_t ciot_sys_start(ciot_sys_t self, ciot_sys_cfg_t *cfg)
{
    return CIOT_ERR_NOT_SUPPORTED;
}

ciot_err_t ciot_sys_stop(ciot_sys_t self)
{
    return CIOT_ERR_NOT_SUPPORTED;
}

ciot_err_t ciot_sys_process_req(ciot_sys_t self, ciot_sys_req_t *req)
{
    return CIOT_ERR_NOT_IMPLEMENTED;
}

ciot_err_t ciot_sys_send_data(ciot_sys_t self, uint8_t *data, int size)
{
    return CIOT_ERR_NOT_SUPPORTED;
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

    memcpy(self->status.info.hw_name, hw_name, sizeof(hw_name));
    memcpy(self->status.info.app_ver, app_ver, sizeof(app_ver));
}

ciot_err_t ciot_sys_task(ciot_sys_t self)
{
    CIOT_NULL_CHECK(self);
    self->status.free_memory = esp_get_free_heap_size();
    self->status.lifetime = time(NULL) - self->init_time;
    return CIOT_OK;
}

#endif