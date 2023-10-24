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

typedef struct ciot_sys_handle
{
    ciot_sys_cfg_t cfg;
    ciot_sys_status_t status;
    time_t init_time;
} ciot_sys_handle_t;

static ciot_sys_handle_t sys;

RTC_NOINIT_ATTR int rst_count;

static ciot_sys_status_t *ciot_sys_get_status();

void ciot_sys_init(void)
{
    char hw_name[] = CIOT_CONFIG_HARDWARE_NAME;
    uint8_t fw_ver[] = CIOT_CONFIG_FIRMWARE_VER;

    sys.status.info.features.hw.uart = CIOT_CONFIG_FEATURE_UART;
    sys.status.info.features.hw.ethernet = CIOT_CONFIG_FEATURE_ETHERNET;
    sys.status.info.features.hw.wifi = CIOT_CONFIG_FEATURE_WIFI;
    sys.status.info.features.hw.bluetooth = CIOT_CONFIG_FEATURE_BLE;
    
    sys.status.info.features.sw.ntp = CIOT_CONFIG_FEATURE_NTP;
    sys.status.info.features.sw.ntp = CIOT_CONFIG_FEATURE_OTA;
    sys.status.info.features.sw.http_client = CIOT_CONFIG_FEATURE_HTTPC;
    sys.status.info.features.sw.http_server = CIOT_CONFIG_FEATURE_HTTPS;
    sys.status.info.features.sw.mqtt_client = CIOT_CONFIG_FEATURE_MQTTC;

    if (esp_reset_reason() == ESP_RST_POWERON || esp_reset_reason() == ESP_RST_SW)
    {
        rst_count = 0;
    }

    rst_count++;
    sys.status.rst_reason = esp_reset_reason();
    sys.status.rst_count = rst_count;

    memcpy(sys.status.info.hw_name, hw_name, sizeof(hw_name));
    memcpy(sys.status.info.fw_ver, fw_ver, sizeof(fw_ver));
}

ciot_sys_t ciot_sys_new(void *handle)
{
    ciot_sys_t this = calloc(1, sizeof(ciot_iface_t));
    this->base.ptr = this;
    this->base.start = (ciot_iface_start_fn *)ciot_sys_start;
    this->base.stop = (ciot_iface_stop_fn *)ciot_sys_stop;
    this->base.process_req = (ciot_iface_process_req_fn *)ciot_sys_process_req;
    this->base.send_data = (ciot_iface_send_data_fn *)ciot_sys_send_data;
    this->base.cfg.ptr = &sys.cfg;
    this->base.cfg.size = sizeof(sys.cfg);
    this->base.status.ptr = ciot_sys_get_status;
    this->base.status.size = sizeof(ciot_sys_status_t);
    this->info.type = CIOT_IFACE_TYPE_SYSTEM;

    return this;
}

ciot_err_t ciot_sys_start(ciot_sys_t this, ciot_sys_cfg_t *cfg)
{
    return CIOT_ERR_NOT_SUPPORTED;
}

ciot_err_t ciot_sys_stop(ciot_sys_t this)
{
    return CIOT_ERR_NOT_SUPPORTED;
}

ciot_err_t ciot_sys_process_req(ciot_sys_t this, ciot_sys_req_t *req)
{
    return CIOT_ERR_NOT_IMPLEMENTED;
}

ciot_err_t ciot_sys_send_data(ciot_sys_t this, uint8_t *data, int size)
{
    return CIOT_ERR_NOT_SUPPORTED;
}

static ciot_sys_status_t *ciot_sys_get_status()
{
    sys.status.free_memory = esp_get_free_heap_size();
    sys.status.lifetime = time(NULL) - sys.init_time;
    return &sys.status;
}