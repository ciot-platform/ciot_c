/**
 * @file sys_parser.h
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

ciot_err_t parse_sys_args(int argc, char **argv, ciot_sys_data_t *sys_data);
ciot_err_t print_sys_data(ciot_sys_data_t *sys_data);
