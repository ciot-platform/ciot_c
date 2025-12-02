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

#if CIOT_CONFIG_FEATURE_BLE_SCN == 1

#include <stdlib.h>
#include "ciot_ble_scn.h"
#include "ciot_err.h"

// static const char *TAG = "ciot_ble_scn";

struct ciot_ble_scn
{
    ciot_ble_scn_base_t base;
};

ciot_ble_scn_t ciot_ble_scn_new(void *handle)
{
    ciot_ble_scn_t self = calloc(1, sizeof(struct ciot_ble_scn));
    ciot_ble_scn_init(self);
    return self;
}

ciot_err_t ciot_ble_scn_start(ciot_ble_scn_t self, ciot_ble_scn_cfg_t *cfg)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(cfg);
    return CIOT_ERR_NOT_IMPLEMENTED;
}

ciot_err_t ciot_ble_scn_stop(ciot_ble_scn_t self)
{
    CIOT_ERR_NULL_CHECK(self);
    return CIOT_ERR_NOT_IMPLEMENTED;
}

#endif  //!CIOT_CONFIG_FEATURE_BLE_SCN == 1