/**
 * @file ciot_crypt.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-10-04
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "ciot_config.h"
#include "ciot_crypt.h"

#if CIOT_CONFIG_FEATURE_CRYPT == 1

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "mbedtls/aes.h"
#include "mbedtls/base64.h"

static ciot_err_t ciot_crypt_enc_word(char *data, ciot_crypt_key_t *key, char *out);
static ciot_err_t ciot_crypt_dec_word(char *data, ciot_crypt_key_t *key, char *out);

static const char *TAG = "ciot_crypt";

ciot_err_t ciot_crypt_enc(ciot_crypt_t *crypt, char *data, char *out, int size)
{
    CIOT_ERR_NULL_CHECK(crypt);
    CIOT_ERR_NULL_CHECK(data);
    CIOT_ERR_NULL_CHECK(out);

    int err = 0;
    size_t len = strlen(data);
    size_t block_size = 16 * ((len / 16) + 1);
    char pad[block_size];
    char buf[block_size];
    memset(pad, 0, block_size);
    memset(buf, 0, block_size);

    sprintf(pad, "%s", data);

    for (size_t i = len; i < block_size; i++)
    {
        pad[i] = (block_size - len);
    }

    for (size_t i = 0; i < block_size; i+=16)
    {
        err = ciot_crypt_enc_word(&pad[i], &crypt->key, &buf[i]);
        if(err)
        {
            return err;
        }
    }
    
    err = mbedtls_base64_encode((unsigned char*)out, size, &len, (unsigned char*)buf, block_size);
    if(err)
    {
        return CIOT_ERR_INVALID_SIZE;
    }

    return CIOT_ERR_OK;
}

ciot_err_t ciot_crypt_dec(ciot_crypt_t *crypt, char *data, char *out, int size)
{
    CIOT_ERR_NULL_CHECK(crypt);
    CIOT_ERR_NULL_CHECK(data);
    CIOT_ERR_NULL_CHECK(out);

    int err = 0;
    size_t len = strlen(data);
    size_t decoded_size = 0;
    char buf[len];
    memset(buf, 0, len);

    err = mbedtls_base64_decode((unsigned char*)buf, len, &decoded_size, (unsigned char*)data, len);
    if(err)
    {
        return CIOT_ERR_INVALID_SIZE;
    }

    if(decoded_size >= size)
    {
        return CIOT_ERR_INVALID_SIZE;
    }

    for (size_t i = 0; i < decoded_size; i+=16)
    {
        err = ciot_crypt_dec_word(&buf[i], &crypt->key, &out[i]);
        if(err)
        {
            return err;
        }
    }

    int decrypted_size = decoded_size - out[decoded_size - 1];
    out[decrypted_size] = '\0';
    return CIOT_ERR_OK;
}

static ciot_err_t ciot_crypt_enc_word(char *data, ciot_crypt_key_t *key, char *out)
{
    mbedtls_aes_context aes;
    mbedtls_aes_init(&aes);
    
    int err = mbedtls_aes_setkey_enc(&aes, (const unsigned char *)key->data, key->size * 8);
    if(err)
    {
        CIOT_LOGE(TAG, "setkey enc mbedtls error: %d", err);
        mbedtls_aes_free(&aes);
        return CIOT_ERR_INVALID_KEY_SIZE;
    }

    err = mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_ENCRYPT, (const unsigned char *)data, (unsigned char *)out);
    if(err)
    {
        CIOT_LOGE(TAG, "encrypt mbedtls error: %d", err);
        mbedtls_aes_free(&aes);
        return CIOT_ERR_FAIL;
    }
    
    mbedtls_aes_free(&aes);

    return CIOT_ERR_OK;
}

static ciot_err_t ciot_crypt_dec_word(char *data, ciot_crypt_key_t *key, char *out)
{
    mbedtls_aes_context aes;
    mbedtls_aes_init(&aes);

    int err = mbedtls_aes_setkey_dec(&aes, key->data, key->size * 8);
    if(err)
    {
        CIOT_LOGE(TAG, "setkey dec mbedtls error: %d", err);
        mbedtls_aes_free(&aes);
        return CIOT_ERR_INVALID_KEY_SIZE;
    }

    err = mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_DECRYPT, (const unsigned char*)data, (unsigned char *)out);
    if(err)
    {
        CIOT_LOGE(TAG, "crypt ecb mbedtls error: %d", err);
        mbedtls_aes_free(&aes);
        return CIOT_ERR_INVALID_KEY_SIZE;
    }
    
    mbedtls_aes_free(&aes);
    return CIOT_ERR_OK;
}

#else

ciot_err_t ciot_crypt_enc(ciot_crypt_t *crypt, char *data, char *out, int size)
{
    return CIOT_ERR_DISABLED;
}

ciot_err_t ciot_crypt_dec(ciot_crypt_t *crypt, char *data, char *out, int size)
{
    return CIOT_ERR_DISABLED;
}

#endif
