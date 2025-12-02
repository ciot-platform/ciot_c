/**
 * @file ciot_ntp.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "ciot_config.h"

#if CIOT_CONFIG_FEATURE_NTP == 1 && defined(CIOT_PLATFORM_WIN)

#include <stdlib.h>
#include "ciot_ntp.h"

// static const char *TAG = "ciot_ntp";

struct ciot_ntp
{
    ciot_ntp_base_t base;
};

void *notif_args;

ciot_ntp_t ciot_ntp_new(void *handle)
{
    ciot_ntp_t self = calloc(1, sizeof(struct ciot_ntp));
    ciot_ntp_init(self);
    return self;
}

ciot_err_t ciot_ntp_start(ciot_ntp_t self, ciot_ntp_cfg_t *cfg)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(cfg);

    return CIOT_ERR_NOT_SUPPORTED;
}

ciot_err_t ciot_ntp_stop(ciot_ntp_t self)
{
    CIOT_ERR_NULL_CHECK(self);
    return CIOT_ERR_NOT_SUPPORTED;
}

#endif // CIOT_CONFIG_FEATURE_NTP == 1
