/**
 * @file ciot_sys.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-11-06
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "ciot_sys.h"

#if CIOT_CONFIG_FEATURE_SYSTEM && defined(CIOT_TARGET_NRF)

#if defined CIOT_CONFIG_FEATURE_TIMER
#if (NRF_SD_BLE_API_VERSION == 2 || NRF_SD_BLE_API_VERSION == 3)
#include "softdevice_handler.h"
#define BLE_GAP_ADV_SET_DATA_SIZE_MAX BLE_GAP_ADV_MAX_SIZE
#else
#include "nrf_sdh.h"
#include "nrf_sdh_soc.h"
#include "nrf_sdh_ble.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_log_default_backends.h"
#include "nrf_bootloader_info.h"
#endif
#endif

#include "nrf_nvic.h"
#include "ciot_timer.h"

#include <string.h>
#include <stdlib.h>

struct ciot_sys
{
    ciot_iface_t iface;
    ciot_sys_status_t status;
    uint64_t reset_scheduled;
};

static void ciot_sys_init(ciot_sys_t self);
static ciot_err_t ciot_sys_req_restart(ciot_sys_t self);
static ciot_err_t ciot_sys_req_init_dfu(ciot_sys_t self);

static const char *TAG = "ciot_sys";

ciot_sys_t ciot_sys_new(void *handle)
{
    ciot_sys_t self = calloc(1, sizeof(struct ciot_sys));
    self->iface.base.ptr = self;
    self->iface.base.start = (ciot_iface_start_fn *)ciot_sys_start;
    self->iface.base.stop = (ciot_iface_stop_fn *)ciot_sys_stop;
    self->iface.base.process_req = (ciot_iface_process_req_fn *)ciot_sys_process_req;
    self->iface.base.send_data = (ciot_iface_send_data_fn *)ciot_sys_send_data;
    self->iface.base.cfg.size = 0;
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
    switch (req->type)
    {
    case CIOT_SYS_REQ_UNKNONW:
        return CIOT_ERR_INVALID_TYPE;
    case CIOT_SYS_REQ_RESTART:
        return ciot_sys_req_restart(self);
    case CIOT_SYS_REQ_INIT_DFU:
        return ciot_sys_req_init_dfu(self);
    }

    return CIOT_ERR_INVALID_TYPE;
}

ciot_err_t ciot_sys_send_data(ciot_sys_t self, uint8_t *data, int size)
{
    return CIOT_ERR_NOT_SUPPORTED;
}

ciot_err_t ciot_sys_rst(ciot_sys_t self)
{
    sd_nvic_SystemReset();
    return CIOT_OK;
}

static void ciot_sys_init(ciot_sys_t self)
{
    CIOT_LOGI(TAG, "System init");

    char hw_name[] = CIOT_CONFIG_HARDWARE_NAME;
    uint8_t app_ver[] = {CIOT_CONFIG_APP_VER};

    ciot_sys_update_features(&self->status.info.features);

    self->status.rst_reason = 0;
    self->status.rst_count = 0;
    self->status.info.hardware = ciot_sys_get_hw();

    memcpy(self->status.info.hw_name, hw_name, sizeof(hw_name));
    memcpy(self->status.info.app_ver, app_ver, sizeof(app_ver));
}

static ciot_err_t ciot_sys_req_restart(ciot_sys_t self)
{
    ciot_sys_rst(self);
    self->iface.base.req.status = CIOT_IFACE_REQ_STATUS_IDLE;
    self->iface.base.req.response_size++;
#ifdef SOFTDEVICE_PRESENT
#ifdef CIOT_CONFIG_FEATURE_TIMER
    self->reset_scheduled = self->status.lifetime + 5;
#else
    ciot_sys_rst(self);
#endif
    return CIOT_OK;
#else
    return CIOT_ERR_NOT_SUPPORTED;
#endif
}

static ciot_err_t ciot_sys_req_init_dfu(ciot_sys_t self)
{
#if CIOT_CONFIG_FEATURE_DFU
    CIOT_ERROR_RETURN(sd_power_gpregret_clr(0, 0xffffffff));
    CIOT_ERROR_RETURN(sd_power_gpregret_set(0, BOOTLOADER_DFU_START));
    nrf_pwr_mgmt_shutdown(NRF_PWR_MGMT_SHUTDOWN_GOTO_DFU);
    return CIOT_OK;
#else
    return CIOT_ERR_NOT_SUPPORTED;
#endif
}

ciot_err_t ciot_sys_task(ciot_sys_t self)
{
    CIOT_NULL_CHECK(self);
    self->status.rst_reason = 0;
    self->status.rst_count = 0;
    self->status.free_memory = 0;
#if CIOT_CONFIG_FEATURE_TIMER
    self->status.lifetime = ciot_timer_get();
    if (self->reset_scheduled)
    {
        ciot_sys_rst(self);
    }
#endif
    return CIOT_OK;
}

ciot_err_t ciot_sys_set_event_bits(ciot_sys_t self, int event_bits)
{
    return CIOT_ERR_NOT_SUPPORTED;
}

#endif
