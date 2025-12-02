/**
 * @file ciot_usb.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "ciot_config.h"

#if CIOT_CONFIG_FEATURE_USB == 1 && defined(CIOT_PLATFORM_LINUX)

#include <stdlib.h>
#include "ciot_usb.h"
#include "ciot_err.h"

// static const char *TAG = "ciot_usb";

struct ciot_usb
{
    ciot_usb_base_t base;
};

ciot_usb_t ciot_usb_new(void *handle)
{
    ciot_usb_t self = calloc(1, sizeof(struct ciot_usb));
    ciot_usb_init(self);
    return self;
}

ciot_err_t ciot_usb_start(ciot_usb_t self, ciot_usb_cfg_t *cfg)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(cfg);
    return CIOT_ERR_NOT_SUPPORTED;
}

ciot_err_t ciot_usb_stop(ciot_usb_t self)
{
    CIOT_ERR_NULL_CHECK(self);
    return CIOT_ERR_NOT_SUPPORTED;
}

ciot_err_t ciot_usb_send_bytes(ciot_usb_t self, uint8_t *data, int size)
{
    return CIOT_ERR_NOT_SUPPORTED;
}

#endif // CIOT_CONFIG_FEATURE_USB == 1
