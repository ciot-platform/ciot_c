/**
 * @file msg_data_parser.h
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

ciot_err_t parse_msg_data_args(int argc, char **argv, ciot_msg_data_t *msg_data);
ciot_err_t print_msg_data(ciot_msg_data_t *msg_data);
