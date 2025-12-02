/**
 * @file ciot_ble_adv.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-06-07
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "ciot_config.h"

#if CIOT_CONFIG_FEATURE_BLE_ADV == 1 && defined(CIOT_PLATFORM_NRF)

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
#include "nrf_bootloader_info.h"
#endif
#include "ble_gap.h"
#include "ciot_ble_adv.h"
#include "ciot_err.h"

// static const char *TAG = "ciot_ble_adv";

#define APP_BLE_CONN_CFG_TAG 1    /**< A tag identifying the SoftDevice BLE configuration. */

static uint8_t m_adv_handle = BLE_GAP_ADV_SET_HANDLE_NOT_SET;

static uint8_t m_adv_data[BLE_GAP_ADV_SET_DATA_SIZE_MAX] = { 0 };

static ble_gap_adv_params_t m_adv_params = { 0 };

static ble_gap_adv_data_t m_adv_str = {
    .adv_data = {
        .p_data = m_adv_data,
        .len = BLE_GAP_ADV_SET_DATA_SIZE_MAX,
    },
    .scan_rsp_data = {.p_data = NULL, .len = 0}};

struct ciot_ble_adv
{
    ciot_ble_adv_base_t base;
};

ciot_ble_adv_t ciot_ble_adv_new(void *handle)
{
    ciot_ble_adv_t self = calloc(1, sizeof(struct ciot_ble_adv));
    ciot_ble_adv_init(self);
    return self;
}

ciot_err_t ciot_ble_adv_start(ciot_ble_adv_t self, ciot_ble_adv_cfg_t *cfg)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(cfg);

    ciot_ble_adv_base_t *base = &self->base;

    base->cfg = *cfg;

#if (NRF_SD_BLE_API_VERSION == 2 || NRF_SD_BLE_API_VERSION == 3)

    m_adv_params.properties.type = cfg->type;
    m_adv_params.p_peer_addr = NULL;
    m_adv_params.fp = cfg->filter_policy;
    m_adv_params.interval = cfg->interval;
    m_adv_params.timeout = cfg->duration;

    uint32_t err_code = sd_ble_gap_tx_power_set(cfg->tx_power);
    if (err_code)
    {
        return CIOT_ERR_FAIL;
    }
#else

    m_adv_params.properties.type = cfg->type;
    m_adv_params.p_peer_addr = NULL;
    m_adv_params.filter_policy = cfg->filter_policy;
    m_adv_params.interval = cfg->interval;
    m_adv_params.duration = cfg->duration;

    uint32_t err_code = sd_ble_gap_adv_set_configure(&m_adv_handle, &m_adv_str, &m_adv_params);
    if (err_code)
    {
        return CIOT_ERR_FAIL;
    }

    err_code = sd_ble_gap_tx_power_set(BLE_GAP_TX_POWER_ROLE_ADV, m_adv_handle, cfg->tx_power);
    if (err_code)
    {
        return CIOT_ERR_FAIL;
    }

    err_code = sd_ble_gap_tx_power_set(BLE_GAP_TX_POWER_ROLE_SCAN_INIT, 0, cfg->tx_power);
    if (err_code)
    {
        return CIOT_ERR_FAIL;
    }
#endif

    base->status.state = CIOT_BLE_ADV_STATE_STARTED;
    return ciot_iface_send_event_type(&base->iface, CIOT_EVENT_TYPE_STARTED);
}

ciot_err_t ciot_ble_adv_stop(ciot_ble_adv_t self)
{
    CIOT_ERR_NULL_CHECK(self);
    return CIOT_ERR_NOT_IMPLEMENTED;
}

ciot_err_t ciot_ble_adv_send_bytes(ciot_ble_adv_t self, uint8_t *data, int size)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_STATE_CHECK(self->base.status.state, CIOT_BLE_ADV_STATE_STARTED);

    if (size > sizeof(m_adv_data))
        size = sizeof(m_adv_data);

    memcpy(m_adv_data, data, size);

#if (NRF_SD_BLE_API_VERSION == 2 || NRF_SD_BLE_API_VERSION == 3)
    self->base.status.err_code = sd_ble_gap_adv_start(&m_adv_params);
#else
    self->base.status.err_code = sd_ble_gap_adv_start(m_adv_handle, APP_BLE_CONN_CFG_TAG);
#endif
    
    if (self->base.status.err_code == NRF_SUCCESS)
    {
        self->base.status.state = CIOT_BLE_ADV_STATE_SENDING;
        return CIOT_ERR_OK;
    }

    return CIOT_ERR_FAIL;
}

ciot_err_t ciot_ble_adv_handle_event(ciot_ble_adv_t self, void *event, void *event_args)
{
    CIOT_ERR_NULL_CHECK(self);

    const ble_evt_t *ev = event;

    switch (ev->header.evt_id)
    {
    case BLE_GAP_EVT_ADV_SET_TERMINATED:
        self->base.status.state = CIOT_BLE_ADV_STATE_STARTED;
        break;
    default:
        break;
    }

    return CIOT_ERR_OK;
}

#endif // CIOT_CONFIG_FEATURE_BLE_ADV == 1
