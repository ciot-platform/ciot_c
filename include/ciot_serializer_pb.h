/**
 * @file ciot_serializer_pb.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-12-17
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef __CIOT_SERIALIZER_PB__H__
#define __CIOT_SERIALIZER_PB__H__

#include "ciot_serializer.h"

ciot_serializer_t ciot_serializer_pb_new(void);
int ciot_serializer_pb_to_bytes(uint8_t *bytes, int size, void *data, const void *type);
int ciot_serializer_pb_from_bytes(uint8_t *bytes, int size, void *data, const void *type);

#endif  //!__CIOT_SERIALIZER_PB__H__