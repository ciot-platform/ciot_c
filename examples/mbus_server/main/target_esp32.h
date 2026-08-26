/**
 * @file target_esp32.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2026-05-28
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef __TARGET_ESP32__H__
#define __TARGET_ESP32__H__

#include "driver/uart.h"

/**
 * @brief RS485 interface configuration
 *
 */
#define TARGET_RS485_BAUD_RATE 9600                        ///< RS485 baud rate
#define TARGET_RS485_BRIDGE_MODE false                     ///< RS485 bridge mode
#define TARGET_RS485_DTR false                             ///< RS485 DTR
#define TARGET_RS485_FLOW_CONTROL UART_HW_FLOWCTRL_DISABLE ///< RS485 flow control
#define TARGET_RS485_NUM UART_NUM_1                        ///< RS485 number
#define TARGET_RS485_PARITY UART_PARITY_DISABLE            ///< RS485 parity
#define TARGET_RS485_RX_PIN 34                             ///< RS485 RX pin number      // RO
#define TARGET_RS485_TX_PIN 32                             ///< RS485 TX pin number      // DI
#define TARGET_RS485_RTS_PIN 33                            ///< RS485 RTS pin number     // RE + DE
#define TARGET_RS485_CTS_PIN -1                            ///< RS485 CTS pin number     // Not used
#define TARGET_RS485_MODE UART_MODE_RS485_HALF_DUPLEX      ///< RS485 mode
#define TARGET_RS485_READ_TIMEOUT 500                      ///< RS485 read timeout in milliseconds
#define TARGET_RS485_WRITE_TIMEOUT 500                     ///< RS485 write timeout in milliseconds

/**
 * @brief Modbus server configuration
 * 
 */
#define TARGET_MBUS_SERVER_ID 1                           ///< Modbus server ID
#define TARGET_MBUS_COILS_COUNT 32                        ///< Modbus coils count
#define TARGET_MBUS_REGS_COUNT 32                         ///< Modbus registers count
#define TARGET_MBUS_SERVER_TCP_PORT 502                   ///< Modbus TCP server port

/**
 * @brief WiFi station configuration, needed to reach the Modbus TCP server
 *
 */
#define TARGET_WIFI_STA_SSID "CIOT ESP32 AP"     ///< WiFi network to join
#define TARGET_WIFI_STA_PASSWORD "admin123"      ///< WiFi network password

#endif  //!__TARGET_ESP32__H__