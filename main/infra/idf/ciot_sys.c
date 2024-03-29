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

#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "esp_system.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_timer.h"

#include "ciot_config.h"
#include "ciot_sys.h"

struct ciot_sys
{
    ciot_iface_t iface;
    ciot_sys_cfg_t cfg;
    ciot_sys_status_t status;
    time_t init_time;
};

static ciot_sys_t sys;

RTC_NOINIT_ATTR int rst_count;

static void ciot_sys_init(ciot_sys_t self);
static ciot_sys_status_t *ciot_sys_get_status();

ciot_sys_t ciot_sys_new(void *handle)
{
    ciot_sys_t self = calloc(1, sizeof(struct ciot_sys));
    self->base.ptr = self;
    self->base.start = (ciot_iface_start_fn *)ciot_sys_start;
    self->base.stop = (ciot_iface_stop_fn *)ciot_sys_stop;
    self->base.process_req = (ciot_iface_process_req_fn *)ciot_sys_process_req;
    self->base.send_data = (ciot_iface_send_data_fn *)ciot_sys_send_data;
    self->base.cfg.ptr = &sys.cfg;
    self->base.cfg.size = sizeof(sys.cfg);
    self->base.status.ptr = &sys.status;
    self->base.status.size = sizeof(ciot_sys_status_t);
    self->info.type = CIOT_IFACE_TYPE_SYSTEM;
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
    uint8_t fw_ver[] = CIOT_CONFIG_FIRMWARE_VER;

    self->status.info.features.hw.storage = CIOT_CONFIG_FEATURE_STORAGE;
    self->status.info.features.hw.system = CIOT_CONFIG_FEATURE_SYSTEM;
    self->status.info.features.hw.uart = CIOT_CONFIG_FEATURE_UART;
    self->status.info.features.hw.usb = CIOT_CONFIG_FEATURE_USB;
    self->status.info.features.hw.ethernet = CIOT_CONFIG_FEATURE_ETHERNET;
    self->status.info.features.hw.wifi = CIOT_CONFIG_FEATURE_WIFI;
    self->status.info.features.hw.bluetooth = CIOT_CONFIG_FEATURE_BLE;
    
    self->status.info.features.sw.ntp = CIOT_CONFIG_FEATURE_NTP;
    self->status.info.features.sw.ota = CIOT_CONFIG_FEATURE_OTA;
    self->status.info.features.sw.http_client = CIOT_CONFIG_FEATURE_HTTPC;
    self->status.info.features.sw.http_server = CIOT_CONFIG_FEATURE_HTTPS;
    self->status.info.features.sw.mqtt_client = CIOT_CONFIG_FEATURE_MQTTC;

    if (esp_reset_reason() == ESP_RST_POWERON || esp_reset_reason() == ESP_RST_SW)
    {
        rst_count = 0;
    }

    rst_count++;
    self->status.rst_reason = esp_reset_reason();
    self->status.rst_count = rst_count;

    memcpy(self->status.info.hw_name, hw_name, sizeof(hw_name));
    memcpy(self->status.info.fw_ver, fw_ver, sizeof(fw_ver));
}

static ciot_sys_status_t *ciot_sys_get_status()
{
    sys.status.free_memory = esp_get_free_heap_size();
    sys.status.lifetime = time(NULL) - sys.init_time;
    return &sys.status;
}
