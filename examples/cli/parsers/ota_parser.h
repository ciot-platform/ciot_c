/**
 * @file ota_parser.h
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

ciot_err_t parse_ota_args(int argc, char **argv, ciot_ota_data_t *ota_data);
