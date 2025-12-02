/**
 * @file main.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-12-16
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef __MAIN__H__
#define __MAIN__H__

#include "ciot.h"
#include "ciot_sys.h"
#include "ciot_uart.h"

#include "ciot_decoder_s.h"
#include "ciot_serializer_pb.h"

typedef enum device_iface_id
{
    DEVICE_IFACE_ID_NONE = -1,
    DEVICE_IFACE_ID_CIOT,
    DEVICE_IFACE_ID_SYS,
    DEVICE_IFACE_ID_UART,
    DEVICE_IFACE_ID_COUNT,
} device_iface_id_t;

typedef struct device_ifaces
{
    ciot_t ciot;
    ciot_sys_t sys;
    ciot_uart_t uart;
    ciot_iface_t *list[DEVICE_IFACE_ID_COUNT];
    ciot_msg_data_t *cfgs[DEVICE_IFACE_ID_CIOT];
} device_ifaces_t;

typedef struct device_decoder
{
    uint8_t buf[CIOT_CONFIG_MSG_SIZE];
    ciot_decoder_t handle;
} device_decoder_t;

typedef struct device
{
    ciot_serializer_t serializer;
    device_decoder_t decoder;
    device_ifaces_t ifaces;
} device_t;

#endif  //!__MAIN__H__