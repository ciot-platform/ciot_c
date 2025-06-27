/**
 * @file ciot_log.h
 * @ingroup core
 * @brief Header file for CIOT logging functionality.
 * @version 0.1
 * @date 2023-10-18
 * @author Wesley Santos (wesleypro37@gmail.com)
 * @copyright Copyright (c) 2023
 */

#ifndef __CIOT_LOG__H__
#define __CIOT_LOG__H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include "ciot_config.h"

#if CIOT_CONFIG_FEATURE_LOGGER

#include "ciot_logger.h"

#define CIOT_LOGD ciot_logd
#define CIOT_LOGV ciot_logv
#define CIOT_LOGI ciot_logi
#define CIOT_LOGW ciot_logw
#define CIOT_LOGE ciot_loge

#elif defined(IDF_VER)

#include "esp_log.h"

#ifndef CONFIG_LOG_MAXIMUM_LEVEL
#define CONFIG_LOG_MAXIMUM_LEVEL ESP_LOG_INFO
#endif

#ifndef CIOT_CONFIG_LOG_LEVEL
#define CIOT_CONFIG_LOG_LEVEL CIOT_LOG_LEVEL_INFO
#endif

// Forwarding logging macros to ESP-IDF logging macros
#define CIOT_LOGD ESP_LOGD
#define CIOT_LOGV ESP_LOGV
#define CIOT_LOGI ESP_LOGI
#define CIOT_LOGW ESP_LOGW
#define CIOT_LOGE ESP_LOGE
#define CIOT_LOG_BUFFER_HEX(tag, data, size) ESP_LOG_BUFFER_HEX_LEVEL(tag, data, size, ESP_LOG_DEBUG)

#else

#include <stdio.h>

/**
 * @brief Macro to log hexadecimal data.
 */
#define CIOT_LOG_BUFFER_HEX(TAG, data, size) CIOT_LOG_HEX(TAG, data, size, CIOT_LOG_LEVEL_DEBUG)

/**
 * @brief Macro to log debug level messages.
 */
#define CIOT_LOGD(TAG, MASK, ...)                          \
    do                                                     \
    {                                                      \
        if (CIOT_CONFIG_LOG_LEVEL >= CIOT_LOG_LEVEL_DEBUG) \
        {                                                  \
            printf("[%s] <DBG>: ", TAG);                   \
            printf(MASK, ##__VA_ARGS__);                   \
            printf("\n");                                  \
        }                                                  \
    } while (0)

/**
 * @brief Macro to log verbose level messages.
 */
#define CIOT_LOGV(TAG, MASK, ...)                            \
    do                                                       \
    {                                                        \
        if (CIOT_CONFIG_LOG_LEVEL >= CIOT_LOG_LEVEL_VERBOSE) \
        {                                                    \
            printf("[%s] <VRB>: ", TAG);                     \
            printf(MASK, ##__VA_ARGS__);                     \
            printf("\n");                                    \
        }                                                    \
    } while (0)

/**
 * @brief Macro to log info level messages.
 */
#define CIOT_LOGI(TAG, MASK, ...)                         \
    do                                                    \
    {                                                     \
        if (CIOT_CONFIG_LOG_LEVEL >= CIOT_LOG_LEVEL_INFO) \
        {                                                 \
            printf("[%s] <INF>: ", TAG);                  \
            printf(MASK, ##__VA_ARGS__);                  \
            printf("\n");                                 \
        }                                                 \
    } while (0)

/**
 * @brief Macro to log warning level messages.
 */
#define CIOT_LOGW(TAG, MASK, ...)                            \
    do                                                       \
    {                                                        \
        if (CIOT_CONFIG_LOG_LEVEL >= CIOT_LOG_LEVEL_WARNING) \
        {                                                    \
            printf("[%s] <WRN>: ", TAG);                     \
            printf(MASK, ##__VA_ARGS__);                     \
            printf("\n");                                    \
        }                                                    \
    } while (0)

/**
 * @brief Macro to log error level messages.
 */
#define CIOT_LOGE(TAG, MASK, ...)                                  \
    do                                                             \
    {                                                              \
        if (CIOT_CONFIG_LOG_LEVEL >= CIOT_LOG_LEVEL_ERROR)         \
        {                                                          \
            printf("[%s] <ERR>: %s:%d ", TAG, __FILE__, __LINE__); \
            printf(MASK, ##__VA_ARGS__);                           \
            printf("\n");                                          \
        }                                                          \
    } while (0)

#endif // IDF_VER

/**
 * @brief Macro to log a message pointer with additional information.
 */
#define CIOT_LOG_MSG(TAG, LOG_MACRO, header, sender, msg)               \
    LOG_MACRO(TAG, header "msg:%ld sender:%ld:%s type:%u iface:%ld:%s", \
              msg.id,                                                   \
              sender->info.id,                                          \
              ciot_iface_to_str(sender),                                \
              msg.data.which_type,                                      \
              msg.iface.id,                                             \
              ciot_iface_type_to_str(msg.iface.type))

/**
 * @brief Macro to log hexadecimal data.
 */
#define CIOT_LOG_HEX(TAG, data, size, level)  \
    do                                        \
    {                                         \
        if (CIOT_CONFIG_LOG_LEVEL >= level)   \
        {                                     \
            uint8_t *u8ptr = data;            \
            for (size_t i = 0; i < size; i++) \
            {                                 \
                printf("%02X", u8ptr[i]);     \
            }                                 \
            printf("\n");                     \
        }                                     \
    } while (0)

/**
 * @brief Macro to log hexadecimal data.
 */
#define CIOT_LOG_HEX_V2(TAG, data, size, level, terminator) \
    do                                                      \
    {                                                       \
        if (CIOT_CONFIG_LOG_LEVEL >= level)                 \
        {                                                   \
            uint8_t *u8ptr = data;                          \
            for (size_t i = 0; i < size; i++)               \
            {                                               \
                printf("%02X", u8ptr[i]);                   \
            }                                               \
            printf(terminator);                             \
        }                                                   \
    } while (0)

#ifdef __cplusplus
}
#endif

#endif //!__CIOT_LOG__H__