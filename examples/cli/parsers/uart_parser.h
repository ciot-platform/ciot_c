/**
 * @file uart_parser.h
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

ciot_err_t parse_uart_args(int argc, char **argv, ciot_uart_data_t *uart_data);
ciot_err_t print_uart_data(ciot_uart_data_t *uart_data);
