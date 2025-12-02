/**
 * @file ciot_storage_fs.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-07-04
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include "ciot_log.h"
#include "ciot_storage_fs.h"

struct ciot_storage_fs
{
    struct ciot_storage base;
};

typedef struct ciot_storage_fs *ciot_storage_fs_t;

static const char *TAG = "ciot_storage";

ciot_storage_t ciot_storage_fs_new(void)
{
    ciot_storage_fs_t self = calloc(1, sizeof(struct ciot_storage_fs));
    ciot_storage_t base = &self->base;
    base->remove = ciot_storage_fs_delete;
    base->write_bytes = ciot_storage_fs_write_bytes;
    base->read_bytes = ciot_storage_fs_read_bytes;
    base->type = CIOT_STORAGE_TYPE_FS;
    return &self->base;
}

ciot_err_t ciot_storage_fs_write_bytes(ciot_storage_t self, char *path, uint8_t *bytes, int size)
{
    FILE *f = fopen(path, "wb");
    if(f != NULL)
    {
        fwrite(bytes, size, 1, f);
        fclose(f);
        return CIOT_ERR_OK;
    }
    CIOT_LOGE(TAG, "Error writing on file %s", path);
    return CIOT_ERR_FAIL;
}

ciot_err_t ciot_storage_fs_read_bytes(ciot_storage_t self, char *path, uint8_t *bytes, int *size)
{
    FILE *f = fopen(path, "rb");
    if(f == NULL)
    {
        *size = 0;
        return CIOT_ERR_NOT_FOUND;
    }

    if(bytes == NULL)
    {
        fseek(f, 0, SEEK_END);
        *size = ftell(f);
        fclose(f);
        return CIOT_ERR_OK;
    }

    int idx = 0;
    int b;
    while ((b = fgetc(f)) != EOF) {
        bytes[idx++] = (uint8_t)b;
        if(idx == *size)
        {
            break;
        }
    }
    fclose(f);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_storage_fs_delete(ciot_storage_t self, char *path)
{
    remove(path);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_storage_fs_read_text(ciot_storage_t self, char *path, char *buf)
{
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        return CIOT_ERR_NOT_FOUND;
    }
    
    fseek(file, 0, SEEK_END);
    long filesize = ftell(file);
    rewind(file);

    fread(buf, sizeof(char), filesize, file);
    buf[filesize] = '\0';

    fclose(file);

    return CIOT_ERR_OK;
}
