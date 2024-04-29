/**
 * @file ciot_uart.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-10-10
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef __CIOT_UART_DATA__H__
#define __CIOT_UART_DATA__H__

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include <stdbool.h>

#include "ciot_common_types.h"

typedef enum __attribute__((packed))
{
    CIOT_UART_STATE_CLOSED,
    CIOT_UART_STATE_STARTED,
    CIOT_UART_STATE_INTERNAL_ERROR,
    CIOT_UART_STATE_CIOT_S_ERROR,
} ciot_uart_state_t;

typedef enum __attribute__((packed))
{
    CIOT_UART_ERR_NONE,
    CIOT_UART_ERR_BREAK,
    CIOT_UART_ERR_BUFFER_FULL,
    CIOT_UART_ERR_FIFO_OVERFLOW,
    CIOT_UART_ERR_FRAME,
    CIOT_UART_ERR_PARITY,
    CIOT_UART_ERR_DATA_BREAK,
    CIOT_UART_ERR_UNKNOWN_EVENT,
    CIOT_UART_ERR_OPEN,
} ciot_uart_error_t;

typedef enum __attribute__((packed))
{
    CIOT_UART_REQ_UNKNOWN,
    CIOT_UART_REQ_SEND_DATA,
    CIOT_UART_REQ_SEND_BYTES,
    CIOT_UART_REQ_ENABLE_BRIDGE_MODE,
} ciot_uart_req_type_t;

typedef struct __attribute__((packed))
{
    uint32_t baud_rate;
    uint8_t num;
    int8_t rx_pin;
    int8_t tx_pin;
    int8_t rts_pin;
    int8_t cts_pin;
    uint16_t parity;
    uint8_t flow_control : 1;
    uint8_t dtr : 1;
    uint8_t bridge_mode : 1;
    uint8_t reserved : 5;
} ciot_uart_cfg_t;

typedef struct __attribute__((packed))
{
    ciot_uart_state_t state;
    ciot_uart_error_t error;
} ciot_uart_status_t;

typedef struct __attribute__((packed))
{
    uint8_t size;
    uint8_t data[255];
} ciot_uart_req_send_data_t;

typedef union __attribute__((packed))
{
    ciot_uart_req_send_data_t send_data;
} ciot_uart_req_data_u;

typedef struct __attribute__((packed))
{
    ciot_uart_req_type_t type;
    ciot_uart_req_data_u data;
} ciot_uart_req_t;

typedef union ciot_uart_event
{
    ciot_event_data_t data;
} ciot_uart_event_u;

typedef union __attribute__((packed))
{
    ciot_uart_cfg_t config;
    ciot_uart_status_t status;
    ciot_uart_req_t request;
    ciot_uart_event_u event;
} ciot_uart_data_u;

#ifdef __cplusplus
}
#endif

#endif //!__CIOT_UART_DATA__H__