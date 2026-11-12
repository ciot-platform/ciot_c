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
#include "app_util_platform.h"
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

// Start the SoftDevice scanner with the stored parameters (ciot_ble_scn_start, ciot_ble_scn_resume)
static uint32_t ciot_ble_scn_sd_start(ciot_ble_scn_t self)
{
#if NRF_SD_BLE_API_VERSION == 2 || NRF_SD_BLE_API_VERSION == 3
    return sd_ble_gap_scan_start(&self->scan_params);
#else
    self->scan_buffer.p_data = self->buffer;
    self->scan_buffer.len = BLE_GAP_SCAN_BUFFER_MIN;
    return sd_ble_gap_scan_start(&self->scan_params, &self->scan_buffer);
#endif
}

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

    base->hold = false;
    sd_ble_gap_scan_stop();

    self->scan_params.interval = base->cfg.interval;
    self->scan_params.window = base->cfg.window;
    self->scan_params.active = base->cfg.active;
    self->scan_params.timeout = base->cfg.timeout;

    uint32_t err = ciot_ble_scn_sd_start(self);
    if (err == NRF_SUCCESS)
    {
        base->status.state = base->cfg.active
                                 ? CIOT_BLE_SCN_STATE_ACTIVE
                                 : CIOT_BLE_SCN_STATE_PASSIVE;
        ciot_iface_send_event_type(&base->iface, CIOT_EVENT_TYPE_STARTED);
    }

    base->status.err_code = ciot_ble_scn_get_error(err);

    return err;
}

// Resume the scanner paused by the SoftDevice after each scan report (API 5+). With API 2/3 the
// scanner doesn't pause after a report: nothing to do.
ciot_err_t ciot_ble_scn_continue(ciot_ble_scn_t self)
{
#if NRF_SD_BLE_API_VERSION != 2 && NRF_SD_BLE_API_VERSION != 3
    uint32_t err = sd_ble_gap_scan_start(NULL, &self->scan_buffer);
    if (err)
    {
        self->base.status.err_code = ciot_ble_scn_get_error(err);
    }
#endif
    return self->base.status.err_code;
}

// Stop the scanner from the scan filter, after the report being handled, to free the radio (e.g.
// for an advertising burst that must start right away) until ciot_ble_scn_resume,
// ciot_ble_scn_start or ciot_ble_scn_stop. Only skipping ciot_ble_scn_continue is not enough: the
// paused scanner keeps its scan window, and the first advertising event waits for its end. Don't
// call it on the report of a scannable advert: stopping there aborts the SCAN_REQ that follows.
// The scan state (status) is kept: it is a short pause, not a stop. If the scanner is not running
// (e.g. already stopped), nothing is held, so ciot_ble_scn_resume doesn't start it.
ciot_err_t ciot_ble_scn_hold(ciot_ble_scn_t self)
{
    CIOT_ERR_NULL_CHECK(self);
    uint32_t err = sd_ble_gap_scan_stop();
    if (err != NRF_SUCCESS)
    {
        return ciot_ble_scn_get_error(err);
    }
    self->base.hold = true;
    return CIOT_ERR_OK;
}

// Restart a scanner held by ciot_ble_scn_hold with its current parameters. Safe from an interrupt
// (e.g. a timer): without a hold (already resumed, or restarted by ciot_ble_scn_start or stopped
// by ciot_ble_scn_stop) it does nothing. The scan timeout, if any, starts again. A scanner already
// running (NRF_ERROR_INVALID_STATE) is not an error.
ciot_err_t ciot_ble_scn_resume(ciot_ble_scn_t self)
{
    CIOT_ERR_NULL_CHECK(self);
    bool hold;
    CRITICAL_REGION_ENTER();
    hold = self->base.hold;
    self->base.hold = false;
    CRITICAL_REGION_EXIT();
    if (!hold)
    {
        return CIOT_ERR_OK;
    }
    uint32_t err = ciot_ble_scn_sd_start(self);
    if (err != NRF_SUCCESS && err != NRF_ERROR_INVALID_STATE)
    {
        self->base.status.err_code = ciot_ble_scn_get_error(err);
        return self->base.status.err_code;
    }
    return CIOT_ERR_OK;
}

// A held scanner is already stopped in the SoftDevice (sd_ble_gap_scan_stop fails with
// NRF_ERROR_INVALID_STATE), but it is still a running scan for the application: it goes to IDLE
// and sends STOPPED, and the hold is dropped so ciot_ble_scn_resume doesn't restart it.
ciot_err_t ciot_ble_scn_stop(ciot_ble_scn_t self)
{
    CIOT_ERR_NULL_CHECK(self);
    ciot_ble_scn_base_t *base = &self->base;
    bool hold;
    CRITICAL_REGION_ENTER();
    hold = base->hold;
    base->hold = false;
    CRITICAL_REGION_EXIT();
    uint32_t err = sd_ble_gap_scan_stop();
    if (err == NRF_SUCCESS || hold)
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
