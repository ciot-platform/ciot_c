/**
 * @file ciot_custom_config_win.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-04-14
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef __CIOT_CUSTOM_CONFIG_WIN__H__
#define __CIOT_CUSTOM_CONFIG_WIN__H__

#include "winsock2.h"
#include "windows.h"

#define CIOT_CONFIG_WIFI_AP_MAC 0x74, 0x13, 0xEA, 0x82, 0x2E, 0xEB,  // Mock wifi ap mac
#define CIOT_CONFIG_WIFI_STA_MAC 0x74, 0x13, 0xEA, 0x82, 0x2E, 0xEA, // Mock wifi sta mac
#define CIOT_CONFIG_WIFI_ETH_MAC 0x04, 0xBF, 0x1B, 0x6B, 0x67, 0x08, // Mock eth mac

#endif  //!__CIOT_CUSTOM_CONFIG_WIN__H__