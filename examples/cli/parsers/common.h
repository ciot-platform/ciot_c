/**
 * @file common.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-05-16
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef __COMMON__H__
#define __COMMON__H__

#include <stdbool.h>
#include <string.h>

#include "ciot_types.h"

const char *get_arg_value(const char *arg, const char *prefix);
bool parse_bool(const char *str);
ciot_iface_type_t parse_iface_type(const char *str);
ciot_data_type_t parse_data_type(const char *str);

#endif  //!__COMMON__H__