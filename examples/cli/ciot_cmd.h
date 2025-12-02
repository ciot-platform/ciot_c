/**
 * @file ciot_cmd.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-05-16
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#pragma once

#include "ciot_types.h"
#include "ciot_iface.h"

typedef struct {
    int sender;
    ciot_msg_t msg;
    ciot_iface_t *iface;
    ciot_get_data_t get_data;
    ciot_err_t err;
    bool completed;
} ciot_cmd_context_t;

ciot_err_t parse_ciot_cmd(int argc, char **argv, ciot_cmd_context_t *ctx);
