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

#if __has_include("ciot_custom_config.h")
#include "ciot_custom_config.h"
#else

#define CIOT_CONFIG_LOG_LEVEL CIOT__LOG_LEVEL__LOG_LEVEL_INFO
#define CIOT_CONFIG_HARDWARE_NAME "CIoT Device"
#define CIOT_CONFIG_APP_VER 0, 1, 0             

#define CIOT_CONFIG_FEATURE_STORAGE 0
#define CIOT_CONFIG_FEATURE_SYSTEM 0
#define CIOT_CONFIG_FEATURE_UART 0
#define CIOT_CONFIG_FEATURE_USB 0
#define CIOT_CONFIG_FEATURE_ETHERNET 0
#define CIOT_CONFIG_FEATURE_WIFI 0
#define CIOT_CONFIG_FEATURE_BLE_SCN 0
#define CIOT_CONFIG_FEATURE_GPIO 0
#define CIOT_CONFIG_FEATURE_NTP 0
#define CIOT_CONFIG_FEATURE_OTA 0
#define CIOT_CONFIG_FEATURE_DFU 0
#define CIOT_CONFIG_FEATURE_HTTP_CLIENT 0
#define CIOT_CONFIG_FEATURE_HTTP_SERVER 0
#define CIOT_CONFIG_FEATURE_MQTT_CLIENT 0
#define CIOT_CONFIG_FEATURE_TIMER 0
#define CIOT_CONFIG_FEATURE_SERIALIZER 0
#define CIOT_CONFIG_FEATURE_CRYPT 0

#endif