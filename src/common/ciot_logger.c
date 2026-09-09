/**
 * @file ciot_logger.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-18
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <stdarg.h>
#include <stdio.h>
#include "ciot_logger.h"
#include "ciot_config.h"
#include "ciot_iface.h"
#include "ciot_log.h"

#if CIOT_CONFIG_FEATURE_IFACE_LOGGER

#ifndef CIOT_CONFIG_LOG_BUFFER_SIZE
#define CIOT_CONFIG_LOG_BUFFER_SIZE 512
#endif

typedef struct ciot_logger
{
    ciot_iface_t *iface;
    ciot_msg_t msg;
    ciot_log_data_t log;
} ciot_logger_t;

static ciot_logger_t logger;

static char log_buffer[CIOT_CONFIG_LOG_BUFFER_SIZE];

static const char *ciot_log_level_to_str(ciot_log_level_t level)
{
    switch (level)
    {
    case CIOT_LOG_LEVEL_NONE:
        return "NONE";
    case CIOT_LOG_LEVEL_ERROR:
        return "ERROR";
    case CIOT_LOG_LEVEL_WARNING:
        return "WARNING";
    case CIOT_LOG_LEVEL_INFO:
        return "INFO";
    case CIOT_LOG_LEVEL_VERBOSE:
        return "VERBOSE";
    case CIOT_LOG_LEVEL_DEBUG:
        return "DEBUG";
    default:
        return "UNKNOWN";
    }
}

void ciot_logger_init(void *iface)
{
    logger.iface = (ciot_iface_t*)iface;
    logger.msg.has_data = true;
    logger.msg.data.which_type = CIOT_MSG_DATA_LOG_TAG;
}

static void ciot_log_internal(ciot_log_level_t level, const char *tag, const char *fmt, va_list args)
{
    if(logger.iface == NULL) return;
    if(level > CIOT_CONFIG_LOG_LEVEL) return;
#ifdef CIOT_CONFIG_LOGGER_LOG_CIOT_MSG
    // Log ciot msg instead ascii string
    logger.log.level = level;
    strcpy(logger.log.tag, tag);
    vsprintf(logger.log.message, fmt, args);
    logger.msg.data.log = logger.log;
    ciot_iface_send_msg(logger.iface, &logger.msg);
#else
    sprintf(log_buffer, "[%s] <%s>: ", tag, ciot_log_level_to_str(level));
    vsprintf(log_buffer + strlen(log_buffer), fmt, args);
    strcat(log_buffer, "\n");
    ciot_iface_send_bytes(logger.iface, (uint8_t*)log_buffer, strlen(log_buffer));
#endif  
}

void ciot_log(ciot_log_level_t level, const char *tag, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    ciot_log_internal(level, tag, fmt, args);
    va_end(args);
}

void ciot_logd(const char *tag, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    ciot_log_internal(CIOT_LOG_LEVEL_DEBUG, tag, fmt, args);
    va_end(args);
}

void ciot_logv(const char *tag, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    ciot_log_internal(CIOT_LOG_LEVEL_VERBOSE, tag, fmt, args);
    va_end(args);
}

void ciot_logi(const char *tag, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    ciot_log_internal(CIOT_LOG_LEVEL_INFO, tag, fmt, args);
    va_end(args);
}

void ciot_logw(const char *tag, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    ciot_log_internal(CIOT_LOG_LEVEL_WARNING, tag, fmt, args);
    va_end(args);
}

void ciot_loge(const char *tag, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    ciot_log_internal(CIOT_LOG_LEVEL_ERROR, tag, fmt, args);
    va_end(args);
}

void ciot_log_hex(const char *tag, const uint8_t *data, size_t size, ciot_log_level_t level)
{
    if(logger.iface == NULL) return;
    if(level > CIOT_CONFIG_LOG_LEVEL) return;

    sprintf(log_buffer, "[%s] <%s>: ", tag, ciot_log_level_to_str(level));
    for(size_t i = 0; i < size; i++)
    {
        sprintf(log_buffer + strlen(log_buffer), "%02X", data[i]);
    }
    strcat(log_buffer, "\n");
    ciot_iface_send_bytes(logger.iface, (uint8_t*)log_buffer, strlen(log_buffer));
}

#endif