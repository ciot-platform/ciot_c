/**
 * @file target.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2026-05-28
 *
 * @copyright Copyright (c) 2026
 *
 */

#ifndef __TARGET__H__
#define __TARGET__H__

#ifdef TARGET_ESP32
#include "target_esp32.h"
#define TARGET_SELECTED
#endif

#ifdef TARGET_WIN32
#include "target_win32.h"
#define TARGET_SELECTED
#endif

#if !defined(TARGET_SELECTED)
#error "Target is not selected"
#endif

#define TARGET_RS485_CFG                             \
    {                                                \
        .which_type = CIOT_MSG_DATA_UART_TAG,        \
        .uart = {                                    \
            .which_type = CIOT_UART_DATA_CONFIG_TAG, \
            .config = {                              \
                .baud_rate = TARGET_RS485_BAUD_RATE, \
                .num = TARGET_RS485_NUM,             \
                .gpio.rx = TARGET_RS485_RX_PIN,      \
                .gpio.tx = TARGET_RS485_TX_PIN,      \
                .gpio.rts = TARGET_RS485_RTS_PIN,    \
                .gpio.cts = TARGET_RS485_CTS_PIN,    \
                .mode = TARGET_RS485_MODE,           \
                .read_timeout = TARGET_RS485_READ_TIMEOUT,  \
                .write_timeout = TARGET_RS485_WRITE_TIMEOUT, \
            },                                       \
        },                                           \
    }

#define TARGET_MBUS_SERVER_CFG                              \
    {                                                       \
        .which_type = CIOT_MSG_DATA_MBUS_SERVER_TAG,        \
        .mbus_server = {                                    \
            .which_type = CIOT_MBUS_SERVER_DATA_CONFIG_TAG, \
            .config = {                                     \
                .which_type = CIOT_MBUS_SERVER_CFG_RTU_TAG, \
                .rtu = {                                    \
                    .server_id = TARGET_MBUS_SERVER_ID,     \
                },                                          \
            },                                              \
        },                                                  \
    }

/**
 * @brief Modbus TCP server configuration. Supported on ESP32 (over WiFi, see
 * TARGET_WIFI_STA_CFG below) and on any Mongoose-backed target (Win32/Linux,
 * or a future Mongoose-capable MCU port).
 */
#define TARGET_MBUS_SERVER_TCP_CFG                          \
    {                                                       \
        .which_type = CIOT_MSG_DATA_MBUS_SERVER_TAG,        \
        .mbus_server = {                                    \
            .which_type = CIOT_MBUS_SERVER_DATA_CONFIG_TAG, \
            .config = {                                     \
                .which_type = CIOT_MBUS_SERVER_CFG_TCP_TAG, \
                .tcp = {                                    \
                    .port = TARGET_MBUS_SERVER_TCP_PORT,    \
                    .max_connections = 1,                   \
                },                                          \
            },                                              \
        },                                                  \
    }

#ifdef TARGET_ESP32

#define TARGET_WIFI_STA_CFG                          \
    {                                                 \
        .which_type = CIOT_MSG_DATA_WIFI_TAG,        \
        .wifi = {                                    \
            .which_type = CIOT_WIFI_DATA_CONFIG_TAG, \
            .config = {                              \
                .type = CIOT_WIFI_TYPE_STA,           \
                .ssid = TARGET_WIFI_STA_SSID,         \
                .password = TARGET_WIFI_STA_PASSWORD, \
            },                                        \
        },                                            \
    }

#endif //!TARGET_ESP32

#endif //!__TARGET__H__