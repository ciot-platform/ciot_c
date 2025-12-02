/**
 * @file ciot_config.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-06-10
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef __CIOT_CONFIG__H__
#define __CIOT_CONFIG__H__

#ifdef ESP_PLATFORM
#define CIOT_PLATFORM_ESP32 1
#elif defined(__linux__)
#define CIOT_PLATFORM_LINUX 1
#define CIOT_PLATFORM_MONGOOSE 1
#elif defined(_WIN32)
#define CIOT_PLATFORM_WIN 1
#define CIOT_PLATFORM_MONGOOSE 1
#elif defined(ICACHE_FLASH) || defined(ICACHE_RAM_ATTR)
#define CIOT_PLATFORM_ESP8266 1
#else
#define CIOT_PLATFORM_DEFAULT 1
#warning "Unknown platform, default configuration will be used"
#endif

#if __has_include("ciot_custom_config.h")
#include "ciot_custom_config.h"
#else

#include "ciot_serializer_pb.h"

#define CIOT_CONFIG_LOG_LEVEL CIOT_LOG_LEVEL_INFO
#define CIOT_CONFIG_HARDWARE_NAME "CIoT Device"
#define CIOT_CONFIG_APP_VER 0, 1, 0

#define CIOT_CONFIG_FEATURE_BLE_ADV 1
#define CIOT_CONFIG_FEATURE_BLE_SCN 1
#define CIOT_CONFIG_FEATURE_BLE 1
#define CIOT_CONFIG_FEATURE_ETH 1
#define CIOT_CONFIG_FEATURE_GPIO 1
#define CIOT_CONFIG_FEATURE_HTTP_CLIENT 1
#define CIOT_CONFIG_FEATURE_HTTP_SERVER 1
#define CIOT_CONFIG_FEATURE_MBUS_CLIENT 1
#define CIOT_CONFIG_FEATURE_MBUS_SERVER 1
#define CIOT_CONFIG_FEATURE_MQTT_CLIENT 1
#define CIOT_CONFIG_FEATURE_NTP 1
#define CIOT_CONFIG_FEATURE_OTA 1
#define CIOT_CONFIG_FEATURE_PWM 1
#define CIOT_CONFIG_FEATURE_SYS 1
#define CIOT_CONFIG_FEATURE_TCP 1
#define CIOT_CONFIG_FEATURE_TIMER 1
#define CIOT_CONFIG_FEATURE_UART 1
#define CIOT_CONFIG_FEATURE_USB 1
#define CIOT_CONFIG_FEATURE_WIFI 1

#define CIOT_CONFIG_FEATURE_STORAGE 1
#define CIOT_CONFIG_FEATURE_SERIALIZER 1
#define CIOT_CONFIG_FEATURE_CRYPT 0
#define CIOT_CONFIG_FEATURE_DFU 1

#define CIOT_CONFIG_MSG_SIZE 256
#define CIOT_CONFIG_URL_SIZE 48
#define CIOT_CONFIG_MQTT_TOPIC_SIZE 48
#define CIOT_CONFIG_START_TIMEOUT_SECS 5
#define CIOT_CONFIG_BUSY_TIMEOUT_SECS 30

// #define CIOT_CONFIG_HTTP_SERVER_ALLOW_ORIGIN "*"

#define ciot_serializer_to_bytes ciot_serializer_pb_to_bytes
#define ciot_serializer_from_bytes ciot_serializer_pb_from_bytes

#endif // __has_include("ciot_custom_config.h")
#endif // __CIOT_CONFIG__H__
