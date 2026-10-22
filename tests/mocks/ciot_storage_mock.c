/**
 * @file ciot_storage_mock.c
 * @brief In-memory fake of ciot_storage_t. See ciot_storage_mock.h.
 */

#include <stdlib.h>
#include <string.h>

#include "ciot_storage_mock.h"

typedef struct ciot_storage_mock_file
{
    bool used;
    char path[CIOT_STORAGE_MOCK_MAX_PATH];
    uint8_t bytes[CIOT_STORAGE_MOCK_MAX_FILE_SIZE];
    int size;
} ciot_storage_mock_file_t;

struct ciot_storage_mock
{
    struct ciot_storage base;
    ciot_storage_mock_file_t files[CIOT_STORAGE_MOCK_MAX_FILES];
};

typedef struct ciot_storage_mock *ciot_storage_mock_t;

static ciot_storage_mock_file_t *ciot_storage_mock_find(ciot_storage_mock_t self, const char *path)
{
    for (int i = 0; i < CIOT_STORAGE_MOCK_MAX_FILES; i++)
    {
        if (self->files[i].used && strncmp(self->files[i].path, path, CIOT_STORAGE_MOCK_MAX_PATH) == 0)
        {
            return &self->files[i];
        }
    }
    return NULL;
}

static ciot_err_t ciot_storage_mock_write_bytes(ciot_storage_t base, char *path, uint8_t *bytes, int size)
{
    ciot_storage_mock_t self = (ciot_storage_mock_t)base;
    ciot_storage_mock_file_t *file = ciot_storage_mock_find(self, path);
    if (file == NULL)
    {
        for (int i = 0; i < CIOT_STORAGE_MOCK_MAX_FILES; i++)
        {
            if (!self->files[i].used)
            {
                file = &self->files[i];
                break;
            }
        }
    }
    if (file == NULL || (size_t)size > sizeof(file->bytes))
    {
        return CIOT_ERR_FAIL;
    }
    file->used = true;
    strncpy(file->path, path, CIOT_STORAGE_MOCK_MAX_PATH - 1);
    file->path[CIOT_STORAGE_MOCK_MAX_PATH - 1] = '\0';
    memcpy(file->bytes, bytes, size);
    file->size = size;
    return CIOT_ERR_OK;
}

static ciot_err_t ciot_storage_mock_read_bytes(ciot_storage_t base, char *path, uint8_t *bytes, int *size)
{
    ciot_storage_mock_t self = (ciot_storage_mock_t)base;
    ciot_storage_mock_file_t *file = ciot_storage_mock_find(self, path);
    if (file == NULL)
    {
        *size = 0;
        return CIOT_ERR_NOT_FOUND;
    }
    if (bytes == NULL)
    {
        *size = file->size;
        return CIOT_ERR_OK;
    }
    int copy_size = (*size < file->size) ? *size : file->size;
    memcpy(bytes, file->bytes, copy_size);
    *size = copy_size;
    return CIOT_ERR_OK;
}

static ciot_err_t ciot_storage_mock_remove(ciot_storage_t base, char *path)
{
    ciot_storage_mock_t self = (ciot_storage_mock_t)base;
    ciot_storage_mock_file_t *file = ciot_storage_mock_find(self, path);
    if (file == NULL)
    {
        return CIOT_ERR_NOT_FOUND;
    }
    memset(file, 0, sizeof(*file));
    return CIOT_ERR_OK;
}

ciot_storage_t ciot_storage_mock_new(void)
{
    ciot_storage_mock_t self = calloc(1, sizeof(struct ciot_storage_mock));
    self->base.write_bytes = ciot_storage_mock_write_bytes;
    self->base.read_bytes = ciot_storage_mock_read_bytes;
    self->base.remove = ciot_storage_mock_remove;
    return &self->base;
}

void ciot_storage_mock_reset(ciot_storage_t base)
{
    ciot_storage_mock_t self = (ciot_storage_mock_t)base;
    memset(self->files, 0, sizeof(self->files));
}

void ciot_storage_mock_free(ciot_storage_t base)
{
    free((ciot_storage_mock_t)base);
}

bool ciot_storage_mock_has_file(ciot_storage_t base, const char *path)
{
    ciot_storage_mock_t self = (ciot_storage_mock_t)base;
    return ciot_storage_mock_find(self, (char *)path) != NULL;
}
