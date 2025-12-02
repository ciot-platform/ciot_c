/**
 * @file ciot_ble.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-06-07
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "ciot_config.h"

#if CIOT_CONFIG_FEATURE_BLE == 1 && defined(CIOT_PLATFORM_NRF)

#include <stdlib.h>
#include <string.h>
#include "ble.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "nrf_pwr_mgmt.h"
#include "app_error.h"
#include "ciot_ble.h"
#include "ciot_err.h"
#include "ciot_utils.h"

#define APP_BLE_OBSERVER_PRIO 3 /**< Application's BLE observer priority. You shouldn't need to modify this value. */
#define APP_BLE_CONN_CFG_TAG 1  /**< A tag identifying the SoftDevice BLE configuration. */

struct ciot_ble
{
    ciot_ble_base_t base;
};

static void ciot_ble_power_management_init(ciot_ble_t self);
static void ciot_ble_stack_init(ciot_ble_t self);

#if (NRF_SD_BLE_API_VERSION == 2 || NRF_SD_BLE_API_VERSION == 3)
static void ciot_ble_evt_handler(ble_evt_t *p_ble_evt);
#else
static void ciot_ble_evt_handler(ble_evt_t const *p_ble_evt, void *p_context);
#endif

ciot_ble_t ble;

ciot_ble_t ciot_ble_new(void *handle)
{
    ciot_ble_t self = calloc(1, sizeof(struct ciot_ble));
    ciot_ble_init(self);
    ble = self;
    return self;
}

ciot_err_t ciot_ble_start(ciot_ble_t self, ciot_ble_cfg_t *cfg)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(cfg);

    ciot_ble_base_t *base = &self->base;
    base->cfg = *cfg;

    if (base->status.state != CIOT_BLE_STATE_STARTED)
    {
        ciot_ble_power_management_init(self);
        ciot_ble_stack_init(self);
        base->status.state = CIOT_BLE_STATE_STARTED;
    }

    uint32_t err = ciot_ble_set_mac(self, cfg->mac);
    if (err == NRF_SUCCESS)
    {
        base->status.using_sw_mac = true;
        // memcpy(base->macs.sw, cfg->mac.data, sizeof(base->macs.sw));
    }

    // if (base->iface.req_status.data_type == CIOT_BLE_)
    // {
    //     base->iface.req_status.state = CIOT_IFACE_REQ_STATE_IDLE;
    // }

    // ciot_event_t iface_event = {0};
    // iface_event.type = CIOT_EVENT_TYPE_STARTED;
    // iface_event.msg = ciot_msg_get(CIOT_MSG_TYPE_STATUS, &base->iface);
    // ciot_iface_send_event(&base->iface, &iface_event);
    ciot_iface_send_event_type(&base->iface, CIOT_EVENT_TYPE_STARTED);

    return CIOT_ERR_OK;
}

ciot_err_t ciot_ble_stop(ciot_ble_t self)
{
    CIOT_ERR_NULL_CHECK(self);
    return CIOT_ERR_NOT_IMPLEMENTED;
}

ciot_err_t ciot_ble_task(ciot_ble_t self)
{
    return CIOT_ERR_NOT_IMPLEMENTED;
}

ciot_err_t ciot_ble_set_mac(ciot_ble_t self, uint8_t mac[6])
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(mac);

    ciot_ble_base_t *base = &self->base;

    if (ciot_ble_mac_is_valid(mac) == false)
    {
        return CIOT_ERR_INVALID_ARG;
    }

    uint32_t err = 0;
    ble_gap_addr_t ble_addr = {0};
    ciot_copy_data(ble_addr.addr, mac, sizeof(ble_addr.addr), true);

    ciot_ble_scn_stop(base->ifaces.scn);

#if (NRF_SD_BLE_API_VERSION == 2)
    err = sd_ble_gap_address_set(BLE_GAP_ADDR_CYCLE_MODE_NONE, &ble_addr);
#else
    err = sd_ble_gap_addr_set(&ble_addr);
#endif

    if (err == NRF_SUCCESS)
    {
        base->status.using_sw_mac = true;
        memcpy(base->cfg.mac, mac, 6);
        memcpy(base->info.sw_mac, mac, 6);
        err = ciot_ble_scn_start(base->ifaces.scn, NULL);
    }

    return err;
}

ciot_err_t ciot_ble_get_mac(ciot_ble_t self, ciot_ble_mac_type_t type, uint8_t mac[6])
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(mac);

    ciot_ble_base_t *base = &self->base;
    uint32_t err = 0;

    switch (type)
    {
    case CIOT_BLE_MAC_TYPE_HARDWARE:
        memcpy(mac, base->info.hw_mac, 6);
        break;
    case CIOT_BLE_MAC_TYPE_SOFTWARE:
        memcpy(mac, base->info.sw_mac, 6);
        break;
    case CIOT_BLE_MAC_TYPE_REAL:
    {
        ble_gap_addr_t ble_addr;
#if (NRF_SD_BLE_API_VERSION == 2)
        err = sd_ble_gap_address_get(&ble_addr);
#else
        err = sd_ble_gap_addr_get(&ble_addr);
#endif
        // memcpy(mac, ble_addr.addr, 6);
        ciot_copy_data(mac, ble_addr.addr, sizeof(ble_addr.addr), 6);
        break;
    }
    default:
        return CIOT_ERR_INVALID_TYPE;
    }

    return err;
}

static void ciot_ble_power_management_init(ciot_ble_t self)
{
#if (NRF_SD_BLE_API_VERSION == 7)
    ret_code_t err_code;
    err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);
#endif
}

static void ciot_ble_stack_init(ciot_ble_t self)
{
    uint32_t err_code;

#if (NRF_SD_BLE_API_VERSION == 2 || NRF_SD_BLE_API_VERSION == 3)
    nrf_clock_lf_cfg_t clock_lf_cfg = NRF_CLOCK_LFCLKSRC;

    // Initialize the SoftDevice handler module.
    SOFTDEVICE_HANDLER_INIT(&clock_lf_cfg, NULL);

    ble_enable_params_t ble_enable_params;
    err_code = softdevice_enable_get_default_config(0, 0, &ble_enable_params);
    APP_ERROR_CHECK(err_code);

    // Check the ram settings against the used number of links
    CHECK_RAM_START_ADDR(CENTRAL_LINK_COUNT, PERIPHERAL_LINK_COUNT);

    err_code = softdevice_enable(&ble_enable_params);
    APP_ERROR_CHECK(err_code);

    // Register with the SoftDevice handler module for BLE events.
    err_code = softdevice_ble_evt_handler_set(ciot_ble_evt_handler);
    APP_ERROR_CHECK(err_code);
#else

    // TODO: verificar como configurar o clock
    // nrf_clock_lf_cfg_t clock_lf_cfg = NRF_CLOCK_LFCLKSRC;

    // Initialize the SoftDevice handler module.
    err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    // Configure the BLE stack using the default settings.
    // Fetch the start address of the application RAM.
    uint32_t ram_start = 0;
    err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
    APP_ERROR_CHECK(err_code);

    // Enable BLE stack.
    err_code = nrf_sdh_ble_enable(&ram_start);
    APP_ERROR_CHECK(err_code);

    NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, ciot_ble_evt_handler, NULL);
#endif

    // Get hardware MAC
    err_code = ciot_ble_get_mac(self, CIOT_BLE_MAC_TYPE_REAL, self->base.info.hw_mac);

    APP_ERROR_CHECK(err_code);
}

#if (NRF_SD_BLE_API_VERSION == 2 || NRF_SD_BLE_API_VERSION == 3)
static void ciot_ble_evt_handler(ble_evt_t *p_ble_evt)
#else
static void ciot_ble_evt_handler(ble_evt_t const *p_ble_evt, void *p_context)
#endif
{
#if (NRF_SD_BLE_API_VERSION == 2 || NRF_SD_BLE_API_VERSION == 3)
    void *p_context = NULL;
#endif

    ciot_ble_base_t *base = &ble->base;

    if (base->ifaces.scn != NULL)
    {
        ciot_ble_scn_handle_event(base->ifaces.scn, (void *)p_ble_evt, p_context);
    }

    if (base->ifaces.adv != NULL)
    {
        ciot_ble_adv_handle_event(base->ifaces.adv, (void *)p_ble_evt, p_context);
    }
}

#endif // CIOT_CONFIG_FEATURE_BLE == 1
