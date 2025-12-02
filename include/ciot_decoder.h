/**
 * @file ciot_decoder.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-27
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef __CIOT_DECODER__H__
#define __CIOT_DECODER__H__

#include "ciot_err.h"

typedef struct ciot_iface ciot_iface_t; // Forward declaration.

typedef enum ciot_decoder_state
{
    CIOT_DECODER_STATE_ERROR=-1,
    CIOT_DECODER_STATE_IDLE,
    CIOT_DECODER_STATE_DECODING,
    CIOT_DECODER_STATE_DONE,
} ciot_decoder_state_t;

typedef struct ciot_decoder *ciot_decoder_t;
typedef ciot_err_t (ciot_decoder_decode_fn)(ciot_decoder_t self, uint8_t byte);
typedef ciot_err_t (ciot_decoder_encode_fn)(ciot_decoder_t self, uint8_t *bytes, int size);
typedef ciot_err_t (ciot_decoder_send_fn)(ciot_decoder_t self, ciot_iface_t *iface, uint8_t *bytes, int size);

typedef struct ciot_decoder_result
{
    uint8_t *buf;
    int size;
} ciot_decoder_result_t;

struct ciot_decoder {
    ciot_decoder_state_t state;
    ciot_decoder_decode_fn *decode;
    ciot_decoder_encode_fn *encode;
    ciot_decoder_send_fn *send;
    ciot_decoder_result_t result;
};

#endif  //!__CIOT_DECODER__H__
