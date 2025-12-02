/**
 * @file ciot_parser.c
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
#include "ciot_parser.h"
#include "ciot_iface.h"

ciot_err_t parse_ciot_args(int argc, char **argv, ciot_data_t *ciot_data)
{
    return CIOT_ERR_NOT_IMPLEMENTED;
}

ciot_err_t print_ciot_data(ciot_data_t *ciot_data)
{
    switch (ciot_data->which_type)
    {
    case CIOT_DATA_STATUS_TAG:
        printf("--state=%d\n", ciot_data->status.state);
        for (int i = 0; i < ciot_data->status.ifaces_count; i++)
        {
            printf("--iface-type=%d\n", ciot_iface_type_to_str(ciot_data->status.ifaces[i].type));
            printf("--iface-state=%d\n", ciot_data->status.ifaces[i].state);
        }
        break;
    case CIOT_DATA_REQUEST_TAG:
        /* code */
        break;
    case CIOT_DATA_INFO_TAG:
        printf("--version=%d.%d.%d.%d\n", ciot_data->info.version[0], ciot_data->info.version[1], ciot_data->info.version[2], ciot_data->info.version[3]);
        printf("--ifaces_count=%d\n", ciot_data->info.ifaces_count);
        for (int i = 0; i < ciot_data->info.ifaces_count; i++)
        {
            printf("--iface-type=%d\n", ciot_iface_type_to_str(ciot_data->info.ifaces[i]));
        }
        break;
    default:
        break;
    }
    return CIOT_ERR_NOT_IMPLEMENTED;
}
