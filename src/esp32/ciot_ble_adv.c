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

#if CIOT_CONFIG_FEATURE_BLE_ADV == 1 && defined(CIOT_PLATFORM_ESP32)

#include <stdlib.h>
#include "ciot_ble_adv.h"
#include "ciot_err.h"

// static const char *TAG = "ciot_ble_adv";

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
    return CIOT_ERR_NOT_IMPLEMENTED;
}

ciot_err_t ciot_ble_adv_stop(ciot_ble_adv_t self)
{
    CIOT_ERR_NULL_CHECK(self);
    return CIOT_ERR_NOT_IMPLEMENTED;
}

ciot_err_t ciot_ble_adv_send_bytes(ciot_ble_adv_t self, uint8_t *data, int size)
{
    return CIOT_ERR_NOT_IMPLEMENTED;
}

#endif  //!CIOT_CONFIG_FEATURE_BLE_ADV == 1