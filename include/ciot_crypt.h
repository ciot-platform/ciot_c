/**
 * @file ciot_crypt.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-10-04
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef __CIOT_CRYPT__H__
#define __CIOT_CRYPT__H__

#include <inttypes.h>
#include "ciot_err.h"

typedef struct ciot_crypt_key
{
    const uint8_t *data;
    int size;
} ciot_crypt_key_t;

typedef struct ciot_crypt
{
    ciot_crypt_key_t key;
} ciot_crypt_t;

ciot_err_t ciot_crypt_enc(ciot_crypt_t *crypt, char *data, char *out, int size);
ciot_err_t ciot_crypt_dec(ciot_crypt_t *crypt, char *data, char *out, int size);

#endif  //!__CIOT_CRYPT__H__