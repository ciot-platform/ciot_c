/**
 * @file ciot_storage.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-11-04
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __CIOT_STORAGE__H__
#define __CIOT_STORAGE__H__

#include "ciot_storage_data.h"
#include "ciot_iface.h"
#include "ciot_err.h"

typedef struct ciot_storage *ciot_storage_t;

ciot_storage_t ciot_storage_new(void *handle);
ciot_err_t ciot_storage_start(ciot_storage_t self, ciot_storage_cfg_t *cfg);
ciot_err_t ciot_storage_stop(ciot_storage_t self);
ciot_err_t ciot_storage_process_req(ciot_storage_t self, ciot_storage_req_t *req);
ciot_err_t ciot_storage_send_data(ciot_storage_t self, uint8_t *data, int size);

ciot_err_t ciot_storage_save(ciot_storage_t self, char *path, uint8_t *data, uint8_t size);
ciot_err_t ciot_storage_load(ciot_storage_t self, char *path, uint8_t *data, uint8_t size);
ciot_err_t ciot_storage_delete(ciot_storage_t self, char *path);
ciot_err_t ciot_storage_format(ciot_storage_t self);

#endif  //!__CIOT_STORAGE__H__