/**
 * @file ciot_sys_common.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-01-09
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "ciot_sys.h"
#include "ciot_config.h"

#ifndef CIOT_CONFIG_FEATURE_STORAGE
#define CIOT_CONFIG_FEATURE_STORAGE 0
#endif

#ifndef CIOT_CONFIG_FEATURE_SYSTEM
#define CIOT_CONFIG_FEATURE_SYSTEM 0
#endif

#ifndef CIOT_CONFIG_FEATURE_UART
#define CIOT_CONFIG_FEATURE_UART 0
#endif

#ifndef CIOT_CONFIG_FEATURE_USB
#define CIOT_CONFIG_FEATURE_USB 0
#endif

#ifndef CIOT_CONFIG_FEATURE_ETHERNET
#define CIOT_CONFIG_FEATURE_ETHERNET 0
#endif

#ifndef CIOT_CONFIG_FEATURE_WIFI
#define CIOT_CONFIG_FEATURE_WIFI 0
#endif

#ifndef CIOT_CONFIG_FEATURE_BLE_SCN
#define CIOT_CONFIG_FEATURE_BLE_SCN 0
#endif

#ifndef CIOT_CONFIG_FEATURE_GPIO
#define CIOT_CONFIG_FEATURE_GPIO 0
#endif

#ifndef CIOT_CONFIG_FEATURE_NTP
#define CIOT_CONFIG_FEATURE_NTP 0
#endif

#ifndef CIOT_CONFIG_FEATURE_OTA
#define CIOT_CONFIG_FEATURE_OTA 0
#endif

#ifndef CIOT_CONFIG_FEATURE_DFU
#define CIOT_CONFIG_FEATURE_DFU 0
#endif

#ifndef CIOT_CONFIG_FEATURE_HTTPC
#define CIOT_CONFIG_FEATURE_HTTPC 0
#endif

#ifndef CIOT_CONFIG_FEATURE_HTTPS
#define CIOT_CONFIG_FEATURE_HTTPS 0
#endif

#ifndef CIOT_CONFIG_FEATURE_MQTTC
#define CIOT_CONFIG_FEATURE_MQTTC 0
#endif

#ifndef CIOT_CONFIG_FEATURE_TIMER
#define CIOT_CONFIG_FEATURE_TIMER 0
#endif

#ifndef CIOT_CONFIG_FEATURE_SERIALIZER
#define CIOT_CONFIG_FEATURE_SERIALIZER 0
#endif

ciot_sys_hw_t ciot_sys_get_hw(void)
{
#if defined(ICACHE_FLASH) || defined(ICACHE_RAM_ATTR)
    return CIOT_SYS_HW_ESP8266;
#elif defined(_WIN32)
    return CIOT_SYS_HW_WIN32;
#elif defined(ARDUINO)
    return CIOT_SYS_HW_ARDUINO;
#elif defined(ESP_PLATFORM)
    return CIOT_SYS_HW_ESP32;
#elif defined(NRF51)
    return CIOT_SYS_HW_NRF51;
#elif defined(NRF52) || defined(NRF52840_XXAA)
    return CIOT_SYS_HW_NRF52;
#elif defined(__LINUX__) || defined(__gnu_linux__) || defined(__linux__)
    return CIOT_SYS_HW_LINUX;
#else
    return CIOT_SYS_HW_UNKNOWN;
#endif
}

ciot_err_t ciot_sys_update_features(ciot_sys_features_t *features)
{    
    CIOT_NULL_CHECK(features);

    features->hw.storage = CIOT_CONFIG_FEATURE_STORAGE;
    features->hw.system = CIOT_CONFIG_FEATURE_SYSTEM;
    features->hw.uart = CIOT_CONFIG_FEATURE_UART;
    features->hw.usb = CIOT_CONFIG_FEATURE_USB;
    features->hw.ethernet = CIOT_CONFIG_FEATURE_ETHERNET;
    features->hw.wifi = CIOT_CONFIG_FEATURE_WIFI;
    features->hw.ble_scn = CIOT_CONFIG_FEATURE_BLE_SCN;
    features->hw.gpio = CIOT_CONFIG_FEATURE_GPIO;
    features->sw.ntp = CIOT_CONFIG_FEATURE_NTP;
    features->sw.ota = CIOT_CONFIG_FEATURE_OTA;
    features->sw.dfu = CIOT_CONFIG_FEATURE_DFU;
    features->sw.http_client = CIOT_CONFIG_FEATURE_HTTPC;
    features->sw.http_server = CIOT_CONFIG_FEATURE_HTTPS;
    features->sw.mqtt_client = CIOT_CONFIG_FEATURE_MQTTC;
    features->sw.timer = CIOT_CONFIG_FEATURE_TIMER;
    features->sw.serializer = CIOT_CONFIG_FEATURE_SERIALIZER;

    return CIOT_OK;
}
