/**
 * @file ciot_custom_config.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-11-01
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef __CIOT_CUSTOM_CONFIG__H__
#define __CIOT_CUSTOM_CONFIG__H__

#include "bsp.h"
#include "nrf_uart.h"

#define CIOT_CONFIG_APP_VER 0, 4, 0
#define CIOT_CONFIG_HARDWARE_NAME "CIOT PCA10056 BLE SCN"
#define CIOT_CONFIG_LOG_LEVEL CIOT_LOG_LEVEL_DEBUG        ///!< Log Level

#define CIOT_CONFIG_FEATURE_STORAGE 0       ///!< Enable Storage
#define CIOT_CONFIG_FEATURE_SYSTEM 1        ///!< Enable System
#define CIOT_CONFIG_FEATURE_UART 1          ///!< Enable UART
#define CIOT_CONFIG_FEATURE_USB 0           ///!< Enable USB
#define CIOT_CONFIG_FEATURE_BLE 1           ///!< Enable Bluetooth Stack
#define CIOT_CONFIG_FEATURE_BLE_SCN 1       ///!< Enable Bluetooth Low Energy Scanner
#define CIOT_CONFIG_FEATURE_ETHERNET 0      ///!< Enable Ethernet
#define CIOT_CONFIG_FEATURE_WIFI 0          ///!< Enable WiFi
#define CIOT_CONFIG_FEATURE_NTP 0           ///!< Enable NTP
#define CIOT_CONFIG_FEATURE_OTA 0           ///!< Enable OTA
#define CIOT_CONFIG_FEATURE_HTTPS 0         ///!< Enable HTTP Server
#define CIOT_CONFIG_FEATURE_HTTPC 0         ///!< Enable HTTP Client
#define CIOT_CONFIG_FEATURE_MQTTC 0         ///!< Enable MQTT Client
#define CIOT_CONFIG_MESSAGE_LEN 330         ///!< CIOT expected message size
#define CIOT_CONFIG_MESSAGE_PAYLOAD_LEN 256 ///!< CIOT message payload size

#define CIOT_CONFIG_BLE_SCN_INTERVAL 5
#define CIOT_CONFIG_BLE_SCN_WINDOW 5
#define CIOT_CONFIG_BLE_SCN_TIMEOUT 0
#define CIOT_CONFIG_BLE_SCN_ACTIVE false
#define CIOT_CONFIG_BLE_SCN_BRIGDE_MODE true

#define CIOT_CONFIG_UART_RX_BUF_SIZE 256
#define CIOT_CONFIG_UART_TX_BUF_SIZE 256
#define CIOT_CONFIG_UART_BAUD UART_BAUDRATE_BAUDRATE_Baud115200
#define CIOT_CONFIG_UART_HWFC NRF_UART_HWFC_DISABLED
#define CIOT_CONFIG_UART_PARITY NRF_UART_PARITY_EXCLUDED
#define CIOT_CONFIG_UART_BRIDGE_MODE false

#define CIOT_CONFIG_UART0_PORT 0
#define CIOT_CONFIG_UART0_RX_PIN RX_PIN_NUMBER
#define CIOT_CONFIG_UART0_TX_PIN TX_PIN_NUMBER
#define CIOT_CONFIG_UART0_BRIDGE_MODE false

#define CIOT_CONFIG_UART1_PORT 1
#define CIOT_CONFIG_UART1_RX_PIN 12
#define CIOT_CONFIG_UART1_TX_PIN 9
#define CIOT_CONFIG_UART1_BRIDGE_MODE false

#endif //!__CIOT_CUSTOM_CONFIG__H__