/**
 * @file main.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-04-14
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <stdbool.h>

#include "ciot_log.h"
#include "ciot_device.h"

static const char *TAG = "main";

int app_main(void)
{
    CIOT_LOGI(TAG, "Hardware Name: %s", CIOT_CONFIG_HARDWARE_NAME);
    CIOT_LOGI(TAG, "Version: %d.%d.%d", CIOT_CONFIG_APP_VER);

    ciot_device_t device = ciot_device_new();
    ciot_device_start(device);

    while (true)
    {
        ciot_device_task(device);
    }

    return 0;
}