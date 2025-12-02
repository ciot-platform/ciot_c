/**
 * @file ciot_storage_nvs.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-07-04
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef __CIOT_STORAGE_NVS__H__
#define __CIOT_STORAGE_NVS__H__

#include "ciot_storage.h"

ciot_storage_t ciot_storage_nvs_new(void);
ciot_err_t ciot_storage_nvs_delete(ciot_storage_t self, char *path);
ciot_err_t ciot_storage_nvs_write_bytes(ciot_storage_t self, char *path, uint8_t *bytes, int size);
ciot_err_t ciot_storage_nvs_read_bytes(ciot_storage_t self, char *path, uint8_t *bytes, int *size);

#endif  //!__CIOT_STORAGE_NVS__H__
