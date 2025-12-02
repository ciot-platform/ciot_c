/**
 * @file ciot_storage_spiffs.c
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
#include "ciot_storage.h"
#include "ciot_storage_fs.h"
#include "esp_spiffs.h"
#include "ciot_err.h"

#define FS_BASE_PATH "/fs"
#define FS_PARTITION_LABLE "storage"

#ifndef CONFIG_WL_SECTOR_SIZE
#define CONFIG_WL_SECTOR_SIZE 512
#endif //! CONFIG_WL_SECTOR_SIZE

struct ciot_storage_spiffs
{
    struct ciot_storage base;
};

typedef struct ciot_storage_spiffs *ciot_storage_spiffs_t;

static const char *TAG = "ciot_storage_spiffs";

static ciot_err_t ciot_storage_spiffs_init(ciot_storage_spiffs_t self);

ciot_storage_t ciot_storage_spiffs_new(void)
{
    ciot_storage_spiffs_t self = calloc(1, sizeof(struct ciot_storage_spiffs));
    ciot_storage_t base = &self->base;
    ciot_storage_spiffs_init(self);
    base->remove = ciot_storage_fs_delete;
    base->write_bytes = ciot_storage_fs_write_bytes;
    base->read_bytes = ciot_storage_fs_read_bytes;
    base->type = CIOT_STORAGE_TYPE_FS;
    return &self->base;
}

static ciot_err_t ciot_storage_spiffs_init(ciot_storage_spiffs_t self)
{
    esp_vfs_spiffs_conf_t cfg = {
        .base_path = FS_BASE_PATH,
        .partition_label = FS_PARTITION_LABLE,
        .max_files = 4,
        .format_if_mount_failed = true,
    };
    esp_err_t err = esp_vfs_spiffs_register(&cfg);
    if(err)
    {
        CIOT_LOGE(TAG, "Error mounting spiffs: %s", esp_err_to_name(err));
        return CIOT_ERR_FAIL;
    }
    return CIOT_ERR_OK;
}

#endif  //!CIOT_CONFIG_FEATURE_STORAGE == 1
