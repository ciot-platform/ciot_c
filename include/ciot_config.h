/**
 * @file ciot_config.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-10-09
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef __CIOT_CONFIG__H__
#define __CIOT_CONFIG__H__

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

#define CIOT_VER 0, 1, 0

#if __has_include("ciot_custom_config.h")
#include "ciot_custom_config.h"
#else

#warning "ciot_custom_config.h not found. Using default configuration. Create a ciot_custom_config.h file to customize the ciot lib configurations."

#define CIOT_CONFIG_LOG_LEVEL CIOT_LOG_LEVEL_INFO
#define CIOT_CONFIG_APP_VER 0, 3, 0
#define CIOT_CONFIG_HARDWARE_NAME "CIOT BOARD"

#define CIOT_CONFIG_FEATURE_STORAGE 1  ///!< Enable Storage
#define CIOT_CONFIG_FEATURE_SYSTEM 1   ///!< Enable System
#define CIOT_CONFIG_FEATURE_UART 1     ///!< Enable UART
#define CIOT_CONFIG_FEATURE_USB 0      ///!< Enable USB
#define CIOT_CONFIG_FEATURE_BLE_SCN 1  ///!< Enable Bluetooth Low Energy Scanner
#define CIOT_CONFIG_FEATURE_ETHERNET 1 ///!< Enable Ethernet
#define CIOT_CONFIG_FEATURE_WIFI 1     ///!< Enable WiFi
#define CIOT_CONFIG_FEATURE_NTP 1      ///!< Enable NTP
#define CIOT_CONFIG_FEATURE_OTA 1      ///!< Enable OTA
#define CIOT_CONFIG_FEATURE_DFU 1      ///!< Enable DFU
#define CIOT_CONFIG_FEATURE_HTTPS 1    ///!< Enable HTTP Server
#define CIOT_CONFIG_FEATURE_HTTPC 1    ///!< Enable HTTP Client
#define CIOT_CONFIG_FEATURE_MQTTC 1    ///!< Enable MQTT Client
#define CIOT_CONFIG_FEATURE_TIMER 1    ///!< Enable system timer
#define CIOT_CONFIG_FEATURE_BRIDGE 1   ///!< Enable Bridge Implementation
#define CIOT_CONFIG_FEATURE_SERIALIZER 0    ///!< Enable serializer

#define CIOT_CONFIG_MESSAGE_LEN 330            ///!< CIOT expected message size
#define CIOT_CONFIG_MESSAGE_PAYLOAD_LEN 256    ///!< CIOT message payload size

#endif //__has_include("ciot_custom_config.h")

#endif //!__CIOT_CONFIG__H__