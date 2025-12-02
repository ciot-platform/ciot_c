/**
 * @file ota_parser.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-05-16
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "common.h"
#include "ota_parser.h"

ciot_err_t parse_ota_args(int argc, char **argv, ciot_ota_data_t *ota_data) {
    memset(ota_data, 0, sizeof(ciot_ota_data_t));

    ota_data->which_type = CIOT_OTA_DATA_CONFIG_TAG;
    for (int i = 2; i < argc; i++) {
        const char *val;
        if ((val = get_arg_value(argv[i], "--url=")) != NULL) {
            strncpy(ota_data->config.url, val, sizeof(ota_data->config.url) - 1);
        } else if ((val = get_arg_value(argv[i], "--force=")) != NULL) {
            ota_data->config.force = parse_bool(val);
        } else if ((val = get_arg_value(argv[i], "--restart=")) != NULL) {
            ota_data->config.restart = parse_bool(val);
        } else if ((val = get_arg_value(argv[i], "--type=")) != NULL) {
            ota_data->config.type = atoi(val);
        }
    }

    return CIOT_ERR_OK;
}
