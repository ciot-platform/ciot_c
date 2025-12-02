/**
 * @file mqtt_client_parser.h
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

ciot_err_t parse_mqtt_client_args(int argc, char **argv, ciot_mqtt_client_data_t *mqtt_client_data);
ciot_err_t print_mqtt_client_data(ciot_mqtt_client_data_t *mqtt_client_data);
