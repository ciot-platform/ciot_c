/**
 * @file ciot_storage_partition.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "ciot_config.h"

#if CIOT_CONFIG_FEATURE_STORAGE == 1 && defined(CIOT_PLATFORM_ESP8266)

#include <stdlib.h>
#include "ciot_storage_partition.h"

struct ciot_storage_partition
{
    struct ciot_storage base;
};

typedef struct ciot_storage_partition *ciot_storage_partition_t;

ciot_storage_t ciot_storage_partition_new(int type, int subtype)
{
    ciot_storage_partition_t self = calloc(1, sizeof(struct ciot_storage_partition));
    ciot_storage_t base = &self->base;
    // ciot_storage_partition_init(self);
    base->type = CIOT_IFACE_TYPE_UNDEFINED;
    return &self->base;
}

#endif  //!CIOT_CONFIG_FEATURE_STORAGE == 1