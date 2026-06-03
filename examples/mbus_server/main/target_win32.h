/**
 * @file target_win32.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2026-05-28
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef __TARGET_WIN32__H__
#define __TARGET_WIN32__H__

/**
 * @brief RS485 interface configuration
 *
 */
#define TARGET_RS485_BAUD_RATE 9600     ///< RS485 baud rate
#define TARGET_RS485_BRIDGE_MODE false  ///< RS485 bridge mode
#define TARGET_RS485_DTR true           ///< RS485 DTR
#define TARGET_RS485_FLOW_CONTROL false ///< RS485 flow control
#define TARGET_RS485_NUM 31             ///< RS485 number
#define TARGET_RS485_PARITY PARITY_NONE ///< RS485 parity
#define TARGET_RS485_RX_PIN 0           ///< RS485 RX pin number
#define TARGET_RS485_TX_PIN 0           ///< RS485 TX pin number
#define TARGET_RS485_RTS_PIN 0          ///< RS485 RTS pin number
#define TARGET_RS485_CTS_PIN 0          ///< RS485 CTS pin number
#define TARGET_RS485_MODE 1             ///< RS485 mode
#define TARGET_RS485_READ_TIMEOUT 100   ///< RS485 read timeout
#define TARGET_RS485_WRITE_TIMEOUT 100  ///< rs485 write timeout

/**
 * @brief Modbus server configuration
 * 
 */
#define TARGET_MBUS_SERVER_ID 1                           ///< Modbus server ID
#define TARGET_MBUS_COILS_COUNT 128                        ///< Modbus coils count
#define TARGET_MBUS_REGS_COUNT 128                         ///< Modbus registers count

#endif  //!__TARGET_WIN32__H__