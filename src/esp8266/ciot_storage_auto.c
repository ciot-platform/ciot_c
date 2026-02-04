/**
 * @file ciot_storage_auto.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-10-17
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "ciot_config.h"

#if CIOT_CONFIG_FEATURE_STORAGE == 1 && defined(CIOT_PLATFORM_ESP8266)

#include <stdlib.h>
#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "esp_spiffs.h"
#include "esp_partition.h"

#include "ciot_storage.h"
#include "ciot_storage_fs.h"
#include "ciot_storage_fat.h"
#include "ciot_storage_spiffs.c"

typedef struct ciot_storage_auto *ciot_storage_auto_t;

static const char *TAG = "ciot_storage_auto";

ciot_storage_t ciot_storage_auto_new(void)
{
    const esp_partition_t* partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_ANY, FS_PARTITION_LABLE);

    if(partition == NULL)
    {
        CIOT_LOGE(TAG, "No storage partition found");
        return NULL;
    }

    switch (partition->subtype)
    {
    case ESP_PARTITION_SUBTYPE_DATA_FAT:
        CIOT_LOGI(TAG, "Using FAT storage");
        return ciot_storage_fat_new();
    case ESP_PARTITION_SUBTYPE_DATA_SPIFFS:
        CIOT_LOGI(TAG, "Using SPIFFS storage");
        return ciot_storage_spiffs_new();
    default:
        CIOT_LOGE(TAG, "No valid storage partition found");
        return NULL;
    }
}

#endif  //!CIOT_CONFIG_FEATURE_STORAGE == 1
