/**
 * @file ciot_sys_base.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-12-11
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "ciot_config.h"

#if CIOT_CONFIG_FEATURE_SYS == 1

#include "ciot_sys.h"
#include "ciot_types.h"

static ciot_err_t ciot_sys_process_data(ciot_iface_t *iface, ciot_msg_data_t *data);
static ciot_err_t ciot_sys_get_data(ciot_iface_t *iface, ciot_msg_data_t *data);
static ciot_err_t ciot_sys_send_data(ciot_iface_t *iface, uint8_t *data, int size);

static ciot_err_t ciot_sys_get_hw(ciot_sys_hw_t *hw);
static ciot_err_t ciot_sys_get_features(ciot_sys_features_t *features);
static ciot_err_t ciot_sys_process_cmd(ciot_sys_t self, ciot_sys_req_cmd_t cmd);

ciot_err_t ciot_sys_init(ciot_sys_t self)
{
    ciot_sys_base_t *base = (ciot_sys_base_t *)self;

    base->iface.ptr = self;
    base->iface.process_data = ciot_sys_process_data;
    base->iface.get_data = ciot_sys_get_data;
    base->iface.send_data = ciot_sys_send_data;
    base->iface.info.type = CIOT_IFACE_TYPE_SYS;

    uint8_t app_ver[] = {CIOT_CONFIG_APP_VER};
    memcpy(base->info.app_ver, app_ver, sizeof(app_ver));
    sprintf(base->info.hw_name, "%s", CIOT_CONFIG_HARDWARE_NAME);
    ciot_sys_get_hw(&base->info.hw_type);
    ciot_sys_get_features(&base->info.features);

    base->info.has_features = true;

    return CIOT_ERR_OK;
}

ciot_err_t ciot_sys_process_req(ciot_sys_t self, ciot_sys_req_t *req)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(req);

    switch (req->which_type)
    {
    case CIOT_SYS_REQ_CMD_TAG:
        return ciot_sys_process_cmd(self, req->cmd);
        break;
    default:
        break;
    }

    return CIOT_ERR_OK;
}

static ciot_err_t ciot_sys_process_data(ciot_iface_t *iface, ciot_msg_data_t *data)
{
    CIOT_ERR_TYPE_CHECK(data->which_type, CIOT_MSG_DATA_SYS_TAG);

    ciot_sys_t self = iface->ptr;
    ciot_sys_data_t *sys = &data->sys;

    switch (sys->which_type)
    {
    case CIOT_SYS_DATA_CONFIG_TAG:
        return ciot_sys_start(self, &sys->config);
    case CIOT_SYS_DATA_REQUEST_TAG:
        return ciot_sys_process_req(self, &sys->request);
    default:
        return CIOT_ERR_INVALID_TYPE;
    }

    return CIOT_ERR_NOT_IMPLEMENTED;
}

static ciot_err_t ciot_sys_get_data(ciot_iface_t *iface, ciot_msg_data_t *data)
{
    CIOT_ERR_TYPE_CHECK(data->which_type, CIOT_MSG_DATA_GET_DATA_TAG);

    ciot_sys_base_t *self = iface->ptr;
    ciot_data_type_t data_type = data->get_data.type;
    data->which_type = CIOT_MSG_DATA_SYS_TAG;

    switch (data_type)
    {
    case CIOT_DATA_TYPE_CONFIG:
        data->sys.which_type = CIOT_SYS_DATA_CONFIG_TAG;
        data->sys.config = self->cfg;
        break;
    case CIOT_DATA_TYPE_STATUS:
        data->sys.which_type = CIOT_SYS_DATA_STATUS_TAG;
        data->sys.status = self->status;
        break;
    case CIOT_DATA_TYPE_INFO:
        data->sys.which_type = CIOT_SYS_DATA_INFO_TAG;
        data->sys.info = self->info;
        break;
    default:
        break;
    }

    return CIOT_ERR_OK;
}

static ciot_err_t ciot_sys_send_data(ciot_iface_t *iface, uint8_t *data, int size)
{
    return CIOT_ERR_NOT_SUPPORTED;
}

static ciot_err_t ciot_sys_get_hw(ciot_sys_hw_t *hw)
{
#if defined(ICACHE_FLASH) || defined(ICACHE_RAM_ATTR)
    *hw = CIOT_SYS_HW_ESP8266;
#elif defined(_WIN32)
    *hw = CIOT_SYS_HW_WIN32;
#elif defined(ARDUINO)
    *hw = CIOT_SYS_HW_ARDUINO;
#elif defined(ESP_PLATFORM)
    *hw = CIOT_SYS_HW_ESP32;
#elif defined(NRF51)
    *hw = CIOT_SYS_HW_NRF51;
#elif defined(NRF52) || defined(NRF52840_XXAA)
    *hw = CIOT_SYS_HW_NRF52;
#elif defined(__LINUX__) || defined(__gnu_linux__) || defined(__linux__)
    *hw = CIOT_SYS_HW_LINUX;
#else
    *hw = CIOT_SYS_HW_UNKNOWN;
#endif
    return CIOT_ERR_OK;
}

static ciot_err_t ciot_sys_get_features(ciot_sys_features_t *features)
{
    features->ble_adv = CIOT_CONFIG_FEATURE_BLE_ADV;
    features->ble_scn = CIOT_CONFIG_FEATURE_BLE_SCN;
    features->ble = CIOT_CONFIG_FEATURE_BLE;
    features->eth = CIOT_CONFIG_FEATURE_ETH;
    features->gpio = CIOT_CONFIG_FEATURE_GPIO;
    features->http_client = CIOT_CONFIG_FEATURE_HTTP_CLIENT;
    features->http_server = CIOT_CONFIG_FEATURE_HTTP_SERVER;
    features->mbus_client = CIOT_CONFIG_FEATURE_MBUS_CLIENT;
    features->mbus_server = CIOT_CONFIG_FEATURE_MBUS_SERVER;
    features->mqtt_client = CIOT_CONFIG_FEATURE_MQTT_CLIENT;
    features->ntp = CIOT_CONFIG_FEATURE_NTP;
    features->ota = CIOT_CONFIG_FEATURE_OTA;
    features->pwm = CIOT_CONFIG_FEATURE_PWM;
    features->sys = CIOT_CONFIG_FEATURE_SYS;
    features->tcp = CIOT_CONFIG_FEATURE_TCP;
    features->timer = CIOT_CONFIG_FEATURE_TIMER;
    features->uart = CIOT_CONFIG_FEATURE_UART;
    features->usb = CIOT_CONFIG_FEATURE_USB;
    features->wifi = CIOT_CONFIG_FEATURE_WIFI;
    features->storage = CIOT_CONFIG_FEATURE_STORAGE;
    features->serializer = CIOT_CONFIG_FEATURE_SERIALIZER;
    features->crypt = CIOT_CONFIG_FEATURE_CRYPT;
    features->dfu = CIOT_CONFIG_FEATURE_DFU;
    return CIOT_ERR_OK;
}

static ciot_err_t ciot_sys_process_cmd(ciot_sys_t self, ciot_sys_req_cmd_t cmd)
{
    switch (cmd)
    {
    case CIOT_SYS_REQ_CMD_RESTART:
        return ciot_sys_restart();
    case CIOT_SYS_REQ_CMD_INIT_DFU:
        return ciot_sys_init_dfu();
    default:
        break;
    }
    return CIOT_ERR_INVALID_TYPE;
}

ciot_err_t ciot_sys_get_status(ciot_sys_t self, ciot_sys_status_t *status)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(status);
    ciot_sys_base_t *base = (ciot_sys_base_t*)self;
    *status = base->status;
    return CIOT_ERR_OK;
}

ciot_err_t ciot_sys_get_info(ciot_sys_t self, ciot_sys_info_t *info)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(info);
    uint8_t app_ver[] = { CIOT_CONFIG_APP_VER };
    memcpy(info->app_ver, app_ver, sizeof(app_ver));
    strcpy(info->hw_name, CIOT_CONFIG_HARDWARE_NAME);
    ciot_sys_get_hw(&info->hw_type);
    ciot_sys_get_features(&info->features);
    return CIOT_ERR_OK;
}

#endif // CIOT_CONFIG_FEATURE_SYS == 1