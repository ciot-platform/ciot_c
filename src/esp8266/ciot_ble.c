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

#if CIOT_CONFIG_FEATURE_BLE == 1 && defined(CIOT_PLATFORM_ESP8266)
 
#include <stdlib.h>
#include "ciot_ble.h"
#include "ciot_err.h"

// static const char *TAG = "ciot_ble";

struct ciot_ble
{
    ciot_ble_base_t base;
};

ciot_ble_t ciot_ble_new(void *handle)
{
    ciot_ble_t self = calloc(1, sizeof(struct ciot_ble));
    ciot_ble_init(self);
    return self;
}

ciot_err_t ciot_ble_start(ciot_ble_t self, ciot_ble_cfg_t *cfg)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(cfg);
    return CIOT_ERR_NOT_IMPLEMENTED;
}

ciot_err_t ciot_ble_stop(ciot_ble_t self)
{
    CIOT_ERR_NULL_CHECK(self);
    return CIOT_ERR_NOT_IMPLEMENTED;
}

ciot_err_t ciot_ble_set_mac(ciot_ble_t self, uint8_t mac[6])
{
    return CIOT_ERR_NOT_IMPLEMENTED;
}

#endif // CIOT_CONFIG_FEATURE_BLE == 1
