/**
 * @file http_client_parser.h
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

ciot_err_t parse_http_client_args(int argc, char **argv, ciot_http_client_data_t *http_client_data);
ciot_err_t print_http_client_data(ciot_http_client_data_t *http_client_data);