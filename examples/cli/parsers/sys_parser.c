/**
 * @file sys_parser.c
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
#include "sys_parser.h"

ciot_err_t parse_sys_args(int argc, char **argv, ciot_sys_data_t *sys_data)
{
    memset(sys_data, 0, sizeof(ciot_sys_data_t));

    sys_data->which_type = CIOT_SYS_DATA_CONFIG_TAG;
    for (int i = 2; i < argc; i++)
    {
        
    }

    return CIOT_ERR_NOT_IMPLEMENTED;
}

ciot_err_t print_sys_data(ciot_sys_data_t *sys_data)
{
    return CIOT_ERR_NOT_IMPLEMENTED;
}
