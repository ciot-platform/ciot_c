/**
 * @file get_parser.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-05-16
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "get_parser.h"
#include "msg_data_parser.h"
#include "ciot_storage_fs.h"

ciot_err_t parse_get_args(int argc, char **argv)
{
    for (int i = 2; i < argc; i++) {
        const char *val;
        if ((val = get_arg_value(argv[i], "--iface-id=")) != NULL) {
            if(strcmp(val, "all") == 0) {
                return CIOT_ERR_NOT_IMPLEMENTED;
            } else {
                int iface_id = atoi(val);
                ciot_msg_data_t data = { 0 };
                ciot_storage_t storage = ciot_storage_fs_new();
                char filename[16];
                sprintf(filename, "%d.dat", iface_id);
                ciot_err_t err = ciot_storage_load_data(storage, filename, &data);
                if(err == CIOT_ERR_OK) {
                    return print_msg_data(&data);
                } else {
                    printf("Error loading data: %d\n", err);
                }
            }
        }
    }
    return CIOT_ERR_NOT_FOUND;
}
