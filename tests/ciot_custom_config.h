/**
 * @file ciot_custom_config.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-13
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef __CIOT_CUSTOM_CONFIG__H__
#define __CIOT_CUSTOM_CONFIG__H__
 
#include "ciot_log.h"
#include "ciot_serializer_pb.h"

#define CIOT_CONFIG_LOG_LEVEL CIOT_LOG_LEVEL_NONE
#define CIOT_CONFIG_HARDWARE_NAME "CIoT Tests"
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
#define CIOT_CONFIG_FEATURE_WIFI_MULTI 1

#define CIOT_CONFIG_FEATURE_STORAGE 1
#define CIOT_CONFIG_FEATURE_SERIALIZER 1
#define CIOT_CONFIG_FEATURE_DFU 1
#define CIOT_CONFIG_FEATURE_CRYPT 1

#define CIOT_CONFIG_MSG_SIZE 256
#define CIOT_CONFIG_URL_SIZE 48
#define CIOT_CONFIG_MQTT_TOPIC_SIZE 48

#define ciot_serializer_to_bytes ciot_serializer_pb_to_bytes
#define ciot_serializer_from_bytes ciot_serializer_pb_from_bytes

#endif  //!__CIOT_CUSTOM_CONFIG__H__
