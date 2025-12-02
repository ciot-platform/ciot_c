/**
 * @file ciot_decoder_s.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-12
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef __CIOT_DECODER_S__H__
#define __CIOT_DECODER_S__H__

#include <inttypes.h>
#include "ciot_decoder.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CIOT_DECODER_S_START_CH '{'
#define CIOT_DECODER_S_END_CH '}'

ciot_decoder_t ciot_decoder_s_new(uint8_t *buf, int size);

#ifdef __cplusplus
}
#endif

#endif  //!__CIOT_DECODER_S__H__