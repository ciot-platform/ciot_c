/**
 * @file ciot_storage.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-12-09
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef __CIOT_STORAGE__H__
#define __CIOT_STORAGE__H__

#include <inttypes.h>
#include "ciot_types.h"
#include "ciot_serializer.h"

typedef struct ciot_storage *ciot_storage_t;
typedef ciot_err_t ciot_storage_write_bytes_fn(ciot_storage_t self, char *path, uint8_t *bytes, int size);
typedef ciot_err_t ciot_storage_read_bytes_fn(ciot_storage_t self, char *path, uint8_t *bytes, int *size);
typedef ciot_err_t ciot_storage_remove_fn(ciot_storage_t self, char *path);

struct ciot_storage
{
    ciot_storage_type_t type;
    ciot_serializer_t serializer;
    ciot_storage_write_bytes_fn *write_bytes;
    ciot_storage_read_bytes_fn *read_bytes;
    ciot_storage_remove_fn *remove;
};

ciot_err_t ciot_storage_save_obj(ciot_storage_t self, char *path, void *obj, const void *obj_type, int size);
ciot_err_t ciot_storage_load_obj(ciot_storage_t self, char *path, void *obj, const void *obj_type);
ciot_err_t ciot_storage_save_data(ciot_storage_t self, char *path, ciot_msg_data_t *data);
ciot_err_t ciot_storage_load_data(ciot_storage_t self, char *path, ciot_msg_data_t *data);

#endif  //!__CIOT_STORAGE__H__