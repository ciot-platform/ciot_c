/**
 * @file ciot_sys.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <stdlib.h>
#include "esp_system.h"
#include "ciot_sys.h"
#include "ciot_timer.h"
#include "ciot_err.h"
#include "ciot_msg.h"

// static const char *TAG = "ciot_sys";

struct ciot_sys
{
    ciot_sys_base_t base;
    uint64_t init_time;
    EventGroupHandle_t event_group;
};

RTC_NOINIT_ATTR int rst_count;

ciot_sys_t ciot_sys_new(void *handle)
{
    ciot_sys_t self = calloc(1, sizeof(struct ciot_sys));
    self->event_group = xEventGroupCreate();
    ciot_sys_init(self);
    return self;
}

ciot_err_t ciot_sys_start(ciot_sys_t self, ciot_sys_cfg_t *cfg)
{
    CIOT_ERR_NULL_CHECK(self);

    ciot_sys_base_t *base = &self->base;

    if (esp_reset_reason() == ESP_RST_POWERON || esp_reset_reason() == ESP_RST_SW)
    {
        rst_count = 0;
    }

    rst_count++;
    base->status.reset_reason = esp_reset_reason();
    base->status.reset_count = rst_count;

    ciot_iface_event_t event = { 0 };
    event.type = CIOT_IFACE_EVENT_STARTED;
    event.msg = ciot_msg_get(CIOT__MSG_TYPE__MSG_TYPE_STATUS, &self->base.iface);
    ciot_iface_send_event(&base->iface, &event);

    return CIOT__ERR__OK;
}

ciot_err_t ciot_sys_stop(ciot_sys_t self)
{
    CIOT_ERR_NULL_CHECK(self);
    return CIOT__ERR__NOT_SUPPORTED;
}

ciot_err_t ciot_sys_task(ciot_sys_t self)
{
    ciot_sys_base_t *base = &self->base;
    base->status.free_memory = esp_get_free_heap_size();
    base->status.lifetime = ciot_timer_now() - self->init_time;
    xEventGroupWaitBits(self->event_group, CIOT_SYS_EVT_BIT_POOLING, pdTRUE, pdTRUE, pdMS_TO_TICKS(100));
    return CIOT__ERR__OK;
}

ciot_err_t ciot_sys_set_event_bits(ciot_sys_t self, int event_bits)
{
    CIOT_ERR_NULL_CHECK(self);
    xEventGroupSetBits(self->event_group, event_bits);
    return CIOT__ERR__OK;
}

ciot_err_t ciot_sys_sleep(long ms)
{
    vTaskDelay(pdMS_TO_TICKS(ms));
    return CIOT__ERR__OK;
}

ciot_err_t ciot_sys_restart(void)
{
    esp_restart();
    return CIOT__ERR__OK;
}

ciot_err_t ciot_sys_init_dfu(void)
{
    return CIOT__ERR__NOT_SUPPORTED;
}
