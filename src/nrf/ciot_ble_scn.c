/**
 * @file ciot_ble_scn.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-06-07
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "ciot_config.h"

#if CIOT_CONFIG_FEATURE_BLE_SCN == 1 && defined(CIOT_PLATFORM_NRF)

#include <stdlib.h>
#if (NRF_SD_BLE_API_VERSION == 2 || NRF_SD_BLE_API_VERSION == 3)
#include "softdevice_handler.h"
#define BLE_GAP_ADV_SET_DATA_SIZE_MAX BLE_GAP_ADV_MAX_SIZE
#else
#include "nrf_sdh.h"
#include "nrf_sdh_soc.h"
#include "nrf_sdh_ble.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_log_default_backends.h"
#endif
#include "ble_gap.h"
#include "ciot_ble_scn.h"
#include "ciot_err.h"
#include "ciot_config.h"

struct ciot_ble_scn
{
    ciot_ble_scn_base_t base;
    ble_gap_scan_params_t scan_params;
#if NRF_SD_BLE_API_VERSION != 2 && NRF_SD_BLE_API_VERSION != 3
    uint8_t buffer[BLE_GAP_SCAN_BUFFER_MIN];
    ble_data_t scan_buffer;
#endif
};

static ciot_err_t ciot_ble_scn_get_error(uint32_t nrf_error);

// static const char *TAG = "hg_ble_scn";

ciot_ble_scn_t ciot_ble_scn_new(void *handle)
{
    ciot_ble_scn_t self = calloc(1, sizeof(struct ciot_ble_scn));
    ciot_ble_scn_init(self);
    return self;
}

ciot_err_t ciot_ble_scn_start(ciot_ble_scn_t self, ciot_ble_scn_cfg_t *cfg)
{
    CIOT_ERR_NULL_CHECK(self);

    ciot_ble_scn_base_t *base = &self->base;

    if (cfg != NULL)
    {
        base->cfg = *cfg;
    }

    if (base->cfg.active && base->status.state == CIOT_BLE_SCN_STATE_ACTIVE)
    {
        return CIOT_ERR_OK;
    }

    if (!base->cfg.active && base->status.state == CIOT_BLE_SCN_STATE_PASSIVE)
    {
        return CIOT_ERR_OK;
    }

    uint32_t err = sd_ble_gap_scan_stop();

    self->scan_params.interval = base->cfg.interval;
    self->scan_params.window = base->cfg.window;
    self->scan_params.active = base->cfg.active;
    self->scan_params.timeout = base->cfg.timeout;

#if NRF_SD_BLE_API_VERSION == 2 || NRF_SD_BLE_API_VERSION == 3
    err = sd_ble_gap_scan_start(&self->scan_params);
#else
    self->scan_buffer.p_data = self->buffer;
    self->scan_buffer.len = BLE_GAP_SCAN_BUFFER_MIN;
    err = sd_ble_gap_scan_start(&self->scan_params, &self->scan_buffer);
#endif

    if (err == NRF_SUCCESS)
    {
        base->status.state = cfg->active
                                 ? CIOT_BLE_SCN_STATE_ACTIVE
                                 : CIOT_BLE_SCN_STATE_PASSIVE;
        ciot_iface_send_event_type(&base->iface, CIOT_EVENT_TYPE_STARTED);
    }

    base->status.err_code = ciot_ble_scn_get_error(err);

    return err;
}

ciot_err_t ciot_ble_scn_start_scan(ciot_ble_scn_t self)
{
    uint32_t err = sd_ble_gap_scan_start(NULL, &self->scan_buffer);
    if (err)
    {
        self->base.status.err_code = ciot_ble_scn_get_error(err);
    }
    return self->base.status.err_code;
}

ciot_err_t ciot_ble_scn_stop(ciot_ble_scn_t self)
{
    CIOT_ERR_NULL_CHECK(self);
    ciot_ble_scn_base_t *base = &self->base;
    int err = sd_ble_gap_scan_stop();
    if (err == NRF_SUCCESS)
    {
        base->status.state = CIOT_BLE_SCN_STATE_IDLE;
        ciot_iface_send_event_type(&base->iface, CIOT_EVENT_TYPE_STOPPED);
    }
    return CIOT_ERR_OK;
}

ciot_err_t ciot_ble_scn_task(ciot_ble_scn_t self)
{
    ciot_ble_scn_base_task(self);
    return CIOT_ERR_OK;
}

static ciot_err_t ciot_ble_scn_get_error(uint32_t nrf_error)
{
    switch (nrf_error)
    {
    case NRF_ERROR_INVALID_ADDR:
        return CIOT_ERR_INVALID_ADDR;
    case NRF_ERROR_INVALID_STATE:
        return CIOT_ERR_INVALID_STATE;
    case NRF_ERROR_INVALID_PARAM:
        return CIOT_ERR_INVALID_ARG;
    case NRF_ERROR_NOT_SUPPORTED:
        return CIOT_ERR_NOT_SUPPORTED;
    case NRF_ERROR_INVALID_LENGTH:
        return CIOT_ERR_INVALID_SIZE;
    case NRF_ERROR_RESOURCES:
        return CIOT_ERR_RESOURCES;
    default:
        return CIOT_ERR_UNKNOWN;
    }
}

#endif // CIOT_CONFIG_FEATURE_BLE_SCN == 1
