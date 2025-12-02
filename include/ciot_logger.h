/**
 * @file ciot_logger.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-18
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef __CIOT_LOGGER__H__
#define __CIOT_LOGGER__H__

#include <stdlib.h>
#include "ciot_types.h"

void ciot_logger_init(void *iface);
void ciot_log(ciot_log_level_t level, const char *tag, const char *fmt, ...);
void ciot_logd(const char *tag, const char *fmt, ...);
void ciot_logv(const char *tag, const char *fmt, ...);
void ciot_logi(const char *tag, const char *fmt, ...);
void ciot_logw(const char *tag, const char *fmt, ...);
void ciot_loge(const char *tag, const char *fmt, ...);

#endif  //!__CIOT_LOGGER__H__