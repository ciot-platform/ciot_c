/**
 * @file ciot.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-27
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef __CIOT__H__
#define __CIOT__H__

#include "ciot_iface.h"
#include "ciot_storage.h"
#include "ciot_types.h"

#if defined(ICACHE_FLASH) || defined(ICACHE_RAM_ATTR)
#define CIOT_TARGET_ESP8266
#elif defined(_WIN32)
#define CIOT_TARGET_WIN
#elif defined(ARDUINO)
#define CIOT_TARGET_INO
#elif defined(ESP_PLATFORM)
#define CIOT_TARGET_ESP32
#elif defined(NRF51) || defined(NRF52) || defined(NRF52840_XXAA)
#define CIOT_TARGET_NRF
#elif defined(__LINUX__) || defined(__gnu_linux__) || defined(__linux__)
#define CIOT_TARGET_LINUX
#else
#define CIOT_TARGET_UNKNOWN
#warning "Target undefined."
#endif

#define CIOT_VER 0,5,0,0
#define CIOT_IFACE_CFG_FILENAME "cfg%d.dat"

#if defined(CIOT_TARGET_WIN) || defined(CIOT_TARGET_LINUX)
#include "mongoose.h"
extern struct mg_mgr mg;
#define CIOT_HANDLE &mg
#define CIOT_MG_ENABLED
#ifndef CIOT_CONFIG_MG_POOL_INTERVAL_MS
#define CIOT_CONFIG_MG_POOL_INTERVAL_MS 10
#endif
#else
#define CIOT_HANDLE NULL
#endif

typedef struct ciot *ciot_t;

typedef struct ciot_ifaces
{
    ciot_iface_t **list;
    ciot_msg_data_t **cfgs;
    uint8_t count;
} ciot_ifaces_t;

typedef struct ciot_cfg
{
    ciot_ifaces_t ifaces;
    ciot_storage_t storage;
} ciot_cfg_t;

typedef struct ciot_starter
{
    uint32_t iface_id;
    bool waiting_result;
    uint64_t timer;
} ciot_starter_t;

typedef struct ciot_receiver
{
    bool serialized;
    ciot_iface_t *sender;
    ciot_event_t event;
} ciot_receiver_t;

struct ciot
{
    ciot_iface_t iface;
    ciot_cfg_t cfg;
    ciot_status_t status;
    ciot_info_t info;
    ciot_ifaces_t ifaces;
    ciot_starter_t starter;
    ciot_receiver_t receiver;
    ciot_err_t error;
    ciot_storage_t storage;
};

ciot_t ciot_new(void);
ciot_err_t ciot_init(ciot_t self);
ciot_err_t ciot_start(ciot_t self, ciot_cfg_t *cfg);
ciot_err_t ciot_stop(ciot_t self);
ciot_err_t ciot_task(ciot_t self);
ciot_err_t ciot_process_req(ciot_t self, ciot_req_t *req);
ciot_err_t ciot_get_cfg(ciot_t self, ciot_cfg_t *cfg);
ciot_err_t ciot_get_status(ciot_t self, ciot_status_t *status);
ciot_err_t ciot_get_info(ciot_t self, ciot_info_t *info);
ciot_err_t ciot_delete_cfg(ciot_t self, ciot_iface_info_t *iface);
ciot_err_t ciot_save_cfg(ciot_t self, ciot_iface_info_t *iface);
ciot_err_t ciot_load_cfg(ciot_t self, ciot_iface_info_t *iface, ciot_msg_data_t *cfg);
ciot_err_t ciot_get_ifaces_info(ciot_t self, ciot_info_t *info);
ciot_err_t ciot_delete_all(ciot_t self);
bool ciot_cfg_exists(ciot_t self, uint8_t iface_id);
const char *ciot_event_to_str(ciot_event_t *event);

#endif  //!__CIOT__H__