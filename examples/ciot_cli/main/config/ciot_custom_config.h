/**
 * @file ciot_custom_config.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-04-14
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef __CIOT_CUSTOM_CONFIG__H__
#define __CIOT_CUSTOM_CONFIG__H__

#if defined(CIOT_TARGET_WIN)
#include "ciot_custom_config_win.h"
#endif

#define CIOT_CONFIG_LOG_LEVEL CIOT_LOG_LEVEL_DEBUG ///!< Log Level
#define CIOT_CONFIG_HARDWARE_NAME "IoT Device"     ///!< Hardware Name
#define CIOT_CONFIG_APP_VER 0, 1, 0                ///!< Application version

#define CIOT_TARGET_MONGOOSE 1         ///!< Enable Mongoose Target
#define CIOT_CONFIG_FEATURE_STORAGE 1  ///!< Enable Storage
#define CIOT_CONFIG_FEATURE_SYSTEM 1   ///!< Enable System
#define CIOT_CONFIG_FEATURE_UART 1     ///!< Enable UART
#define CIOT_CONFIG_FEATURE_USB 0      ///!< Enable USB
#define CIOT_CONFIG_FEATURE_BLE 0      ///!< Enable Bluetooth
#define CIOT_CONFIG_FEATURE_BLE_SCN 0  ///!< Enable Bluetooth Scanner
#define CIOT_CONFIG_FEATURE_ETHERNET 0 ///!< Enable Ethernet
#define CIOT_CONFIG_FEATURE_WIFI 1     ///!< Enable WiFi
#define CIOT_CONFIG_FEATURE_GPIO 1     ///!< Enable GPIO
#define CIOT_CONFIG_FEATURE_NTP 0      ///!< Enable NTP
#define CIOT_CONFIG_FEATURE_OTA 1      ///!< Enable OTA
#define CIOT_CONFIG_FEATURE_HTTPS 1    ///!< Enable HTTP Server
#define CIOT_CONFIG_FEATURE_HTTPC 1    ///!< Enable HTTP Client
#define CIOT_CONFIG_FEATURE_MQTTC 1    ///!< Enable MQTT Client
#define CIOT_CONFIG_FEATURE_TIMER 1    ///!< Enable Timer
#define CIOT_CONFIG_FEATURE_SERIALIZER 1    ///!< Enable serialization

#define CIOT_CONFIG_MESSAGE_LEN 330         ///!< CIOT expected message size
#define CIOT_CONFIG_MESSAGE_PAYLOAD_LEN 256 ///!< CIOT message payload size

#define CIOT_CONFIG_STORAGE_TYPE CIOT_STORAGE_TYPE_FS

#define CIOT_CONFIG_UART_BAUD_RATE 115200
#define CIOT_CONFIG_UART_NUM 0
#define CIOT_CONFIG_UART_RX_PIN 0
#define CIOT_CONFIG_UART_TX_PIN 0
#define CIOT_CONFIG_UART_RTS_PIN 0
#define CIOT_CONFIG_UART_CTS_PIN 0
#define CIOT_CONFIG_UART_FLOW_CONTROL 0
#define CIOT_CONFIG_UART_PARITY 0
#define CIOT_CONFIG_UART_DTR 0
#define CIOT_CONFIG_UART_BRIDGE_MODE 0

#define CIOT_CONFIG_TCP_DHCP CIOT_TCP_DHCP_CFG_NO_CHANGE
#define CIOT_CONFIG_TCP_IP 0, 0, 0, 0
#define CIOT_CONFIG_TCP_GATEWAY 0, 0, 0, 0
#define CIOT_CONFIG_TCP_MASK 0, 0, 0, 0
#define CIOT_CONFIG_TCP_DNS 0, 0, 0, 0

#define CIOT_CONFIG_WIFI_AP_SSID "Ciot AP"
#define CIOT_CONFIG_WIFI_AP_PASSWORD "administrator"
#define CIOT_CONFIG_WIFI_AP_TCP CIOT_CONFIG_TCP

#define CIOT_CONFIG_WIFI_STA_SSID "Ciot STA"
#define CIOT_CONFIG_WIFI_STA_PASSWORD "administrator"
#define CIOT_CONFIG_WIFI_STA_TCP CIOT_CONFIG_TCP

#define CIOT_CONFIG_ETH CIOT_CONFIG_TCP

#define CIOT_CONFIG_NTP_OP_MODE 0
#define CIOT_CONFIG_NTP_SYNC_MODE 0
#define CIOT_CONFIG_NTP_SYNC_INTERVAL 0
#define CIOT_CONFIG_NTP_SERVERS_COUNT 3
#define CIOT_CONFIG_NTP_SERVER1 "pool.ntp.org"
#define CIOT_CONFIG_NTP_SERVER2 "time.google.com"
#define CIOT_CONFIG_NTP_SERVER3 "gps.ntp.br"

#define CIOT_CONFIG_HTTPC_URL "http://127.0.0.1:8080/v1/ciot"
#define CIOT_CONFIG_HTTPC_TRANSPORT CIOT_HTTPC_TRANSPORT_TCP
#define CIOT_CONFIG_HTTPC_METHOD CIOT_HTTPC_METHOD_POST
#define CIOT_CONFIG_HTTPC_TIMEOUT 20000

#define CIOT_CONFIG_HTTPS_ADDRESS "http://127.0.0.1"
#define CIOT_CONFIG_HTTPS_ROUTE "/v1/ciot"
#define CIOT_CONFIG_HTTPS_PORT 8080

#define CIOT_CONFIG_MQTTC_URL "mqtt://test.mosquitto.org"
#define CIOT_CONFIG_MQTTC_PORT 1883
#define CIOT_CONFIG_MQTTC_QOS 0
#define CIOT_CONFIG_MQTTC_USER ""
#define CIOT_CONFIG_MQTTC_PASS ""
#define CIOT_CONFIG_MQTTC_TOPIC_D2B "v1/ciot/d2b"
#define CIOT_CONFIG_MQTTC_TOPIC_B2D "v1/ciot/b2d"

#define CIOT_CONFIG_SYS \
    {                   \
    }

#define CIOT_CONFIG_STORAGE          \
    {                                \
        .type = CIOT_STORAGE_TYPE_FS \
    }

#define CIOT_CONFIG_UART                               \
    {                                                  \
        .baud_rate = CIOT_CONFIG_UART_BAUD_RATE,       \
        .num = CIOT_CONFIG_UART_NUM,                   \
        .rx_pin = CIOT_CONFIG_UART_RX_PIN,             \
        .tx_pin = CIOT_CONFIG_UART_TX_PIN,             \
        .rts_pin = CIOT_CONFIG_UART_RTS_PIN,           \
        .cts_pin = CIOT_CONFIG_UART_CTS_PIN,           \
        .flow_control = CIOT_CONFIG_UART_FLOW_CONTROL, \
        .parity = CIOT_CONFIG_UART_PARITY,             \
        .dtr = CIOT_CONFIG_UART_DTR,                   \
        .bridge_mode = CIOT_CONFIG_UART_BRIDGE_MODE,   \
    }

#define CIOT_CONFIG_TCP                       \
    {                                         \
        .dhcp = CIOT_TCP_DHCP_CFG_NO_CHANGE,  \
        .ip = {CIOT_CONFIG_TCP_IP},           \
        .gateway = {CIOT_CONFIG_TCP_GATEWAY}, \
        .mask = {CIOT_CONFIG_TCP_MASK},       \
        .dns = {CIOT_CONFIG_TCP_DNS},         \
    }

#define CIOT_CONFIG_WIFI_AP                       \
    {                                             \
        .ssid = CIOT_CONFIG_WIFI_AP_SSID,         \
        .password = CIOT_CONFIG_WIFI_AP_PASSWORD, \
        .tcp = CIOT_CONFIG_WIFI_AP_TCP,           \
        .type = CIOT_WIFI_IF_AP,                  \
    }

#define CIOT_CONFIG_WIFI_STA                       \
    {                                              \
        .ssid = CIOT_CONFIG_WIFI_STA_SSID,         \
        .password = CIOT_CONFIG_WIFI_STA_PASSWORD, \
        .tcp = CIOT_CONFIG_WIFI_STA_TCP,           \
        .type = CIOT_WIFI_IF_STA,                  \
    }

#define HG_TCP_CONFIG_NTP                                 \
    {                                                     \
        .op_mode = HG_TCP_CONFIG_NTP_OP_MODE,             \
        .sync_mode = HG_TCP_CONFIG_NTP_SYNC_MODE,         \
        .sync_interval = HG_TCP_CONFIG_NTP_SYNC_INTERVAL, \
        .server = {                                       \
            {CIOT_CONFIG_NTP_SERVER1},                    \
            {CIOT_CONFIG_NTP_SERVER2},                    \
            {CIOT_CONFIG_NTP_SERVER3}                     \
        }                                                 \
    }

#define CIOT_CONFIG_HTTPC                         \
    {                                             \
        .url = CIOT_CONFIG_HTTPC_URL,             \
        .method = CIOT_CONFIG_HTTPC_METHOD,       \
        .transport = CIOT_CONFIG_HTTPC_TRANSPORT, \
        .timeout = CIOT_CONFIG_HTTPC_TIMEOUT,     \
    }

#define CIOT_CONFIG_HTTPS                     \
    {                                         \
        .address = CIOT_CONFIG_HTTPS_ADDRESS, \
        .route = CIOT_CONFIG_HTTPS_ROUTE,     \
        .port = CIOT_CONFIG_HTTPS_PORT,       \
    }

#define CIOT_CONFIG_MQTTC                       \
    {                                           \
        .url = CIOT_CONFIG_MQTTC_URL,           \
        .port = CIOT_CONFIG_MQTTC_PORT,         \
        .qos = CIOT_CONFIG_MQTTC_QOS,           \
        .topics = {                             \
            .b2d = CIOT_CONFIG_MQTTC_TOPIC_B2D, \
            .d2b = CIOT_CONFIG_MQTTC_TOPIC_D2B, \
        }                                       \
    }

#endif //!__CIOT_CUSTOM_CONFIG__H__