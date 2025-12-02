/**
 * @file ciot_crypt_test.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-10-04
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <string.h>

#include "unity.h"
#include "ciot_crypt.h"

#define CIOT_CRYPT_INVALID_KEY (uint8_t*) "12345678"
#define CIOT_CRYPT_INVALID_KEY_SIZE 8

#define CIOT_CRYPT_VALID_KEY (uint8_t*) "1234567890123456"
#define CIOT_CRYPT_VALID_KEY_SIZE 16

#define CIOT_CRYPT_DATA_DECRYPTED "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat"
#define CIOT_CRYPT_DATA_ENCRYPTED "YFGMkV1Cyz4ipXOXE6jw27qv4BCWYnJdwNnRIFVpJKe+k9CMC/j1KLPb801/eo9Hieb5D8Y6Npng08DbEU7wpYRYpB6wBn2NTql3qIkhYYvQzsPi9rY3gZtMBpe+5MV83d71wUEgGVnsu224gvCmOG0ATRhTu5ltNq6AICqw+fyScYexCTVP2aEDNIp+13WcD1LSkNOO6xWyOlSH4xhbsJGDLMq379g3IEoDS9CltUrO+vDlJ4EcNy72j6frD4KElPVgk9SgAbtqvyZ7vcwV3K5qK8GdpEVi7hpCQ9pQXskUL2oAnyR6U/A64xgu9G87"

void test_ciot_crypt_enc_null_crypt(void)
{
    char *data = CIOT_CRYPT_DATA_DECRYPTED;
    char out[sizeof(CIOT_CRYPT_DATA_ENCRYPTED)];
    ciot_err_t err = ciot_crypt_enc(NULL, data, out, sizeof(out));
    TEST_ASSERT(err == CIOT_ERR_NULL_ARG);
}

void test_ciot_crypt_enc_null_data(void)
{
    char out[sizeof(CIOT_CRYPT_DATA_ENCRYPTED)];
    ciot_crypt_t crypt = { 0 };
    ciot_err_t err = ciot_crypt_enc(&crypt, NULL, out, sizeof(out));
    TEST_ASSERT(err == CIOT_ERR_NULL_ARG);
}

void test_ciot_crypt_enc_null_buffer(void)
{
    char *data = CIOT_CRYPT_DATA_DECRYPTED;
    ciot_crypt_t crypt = { 0 };
    ciot_err_t err = ciot_crypt_enc(&crypt, data, NULL, 0);
    TEST_ASSERT(err == CIOT_ERR_NULL_ARG);
}

void test_ciot_crypt_enc_invalid_key_size(void)
{
    char out[sizeof(CIOT_CRYPT_DATA_ENCRYPTED)];
    char *data = CIOT_CRYPT_DATA_DECRYPTED;
    ciot_crypt_t crypt = {
        .key.data = CIOT_CRYPT_INVALID_KEY,
        .key.size = CIOT_CRYPT_INVALID_KEY_SIZE,
    };
    ciot_err_t err = ciot_crypt_enc(&crypt, data, out, sizeof(out));
    TEST_ASSERT(err == CIOT_ERR_INVALID_KEY_SIZE);
}

void test_ciot_crypt_enc_invalid_size(void)
{
    char out[5];
    char *data = CIOT_CRYPT_DATA_DECRYPTED;
    ciot_crypt_t crypt = {
        .key.data = CIOT_CRYPT_VALID_KEY,
        .key.size = CIOT_CRYPT_VALID_KEY_SIZE,
    };
    ciot_err_t err = ciot_crypt_enc(&crypt, data, out, sizeof(out));
    TEST_ASSERT(err == CIOT_ERR_INVALID_SIZE);
}

void test_ciot_crypt_enc_ok(void)
{
    char out[sizeof(CIOT_CRYPT_DATA_ENCRYPTED)];
    char *data = CIOT_CRYPT_DATA_DECRYPTED;
    ciot_crypt_t crypt = {
        .key.data = CIOT_CRYPT_VALID_KEY,
        .key.size = CIOT_CRYPT_VALID_KEY_SIZE,
    };
    ciot_err_t err = ciot_crypt_enc(&crypt, data, out, sizeof(out));
    TEST_ASSERT(err == CIOT_ERR_OK);
    TEST_ASSERT(strcmp(out, CIOT_CRYPT_DATA_ENCRYPTED) == 0);
}

void test_ciot_crypt_dec_null_crypt(void)
{
    char *data = CIOT_CRYPT_DATA_ENCRYPTED;
    char out[sizeof(CIOT_CRYPT_DATA_ENCRYPTED)];
    ciot_err_t err = ciot_crypt_dec(NULL, data, out, sizeof(out));
    TEST_ASSERT(err == CIOT_ERR_NULL_ARG);
}

void test_ciot_crypt_dec_null_data(void)
{
    char out[sizeof(CIOT_CRYPT_DATA_ENCRYPTED)];
    ciot_crypt_t crypt = { 0 };
    ciot_err_t err = ciot_crypt_dec(&crypt, NULL, out, sizeof(out));
    TEST_ASSERT(err == CIOT_ERR_NULL_ARG);
}

void test_ciot_crypt_dec_null_buffer(void)
{
    char *data = CIOT_CRYPT_DATA_ENCRYPTED;
    ciot_crypt_t crypt = { 0 };
    ciot_err_t err = ciot_crypt_dec(&crypt, data, NULL, 0);
    TEST_ASSERT(err == CIOT_ERR_NULL_ARG);
}

void test_ciot_crypt_dec_invalid_key_size(void)
{
    char out[sizeof(CIOT_CRYPT_DATA_ENCRYPTED)];
    char *data = CIOT_CRYPT_DATA_ENCRYPTED;
    ciot_crypt_t crypt = {
        .key.data = CIOT_CRYPT_INVALID_KEY,
        .key.size = CIOT_CRYPT_INVALID_KEY_SIZE,
    };
    ciot_err_t err = ciot_crypt_dec(&crypt, data, out, sizeof(out));
    TEST_ASSERT(err == CIOT_ERR_INVALID_KEY_SIZE);
}

void test_ciot_crypt_dec_invalid_size(void)
{
    char out[5];
    char *data = CIOT_CRYPT_DATA_ENCRYPTED;
    ciot_crypt_t crypt = {
        .key.data = CIOT_CRYPT_VALID_KEY,
        .key.size = CIOT_CRYPT_VALID_KEY_SIZE,
    };
    ciot_err_t err = ciot_crypt_dec(&crypt, data, out, sizeof(out));
    TEST_ASSERT(err == CIOT_ERR_INVALID_SIZE);
}

void test_ciot_crypt_dec_ok(void)
{
    char out[sizeof(CIOT_CRYPT_DATA_ENCRYPTED)];
    char *data = CIOT_CRYPT_DATA_ENCRYPTED;
    ciot_crypt_t crypt = {
        .key.data = CIOT_CRYPT_VALID_KEY,
        .key.size = CIOT_CRYPT_VALID_KEY_SIZE,
    };
    ciot_err_t err = ciot_crypt_dec(&crypt, data, out, sizeof(out));
    TEST_ASSERT(err == CIOT_ERR_OK);
    TEST_ASSERT(strcmp(out, CIOT_CRYPT_DATA_DECRYPTED) == 0);
}

void test_ciot_crypt_pkcs7_padding(void)
{
    const char *input = "1234567890abcdef";
    char encrypted[128] = {0};
    char decrypted[128] = {0};
    ciot_crypt_t crypt = {
        .key = {
            .data = CIOT_CRYPT_VALID_KEY,
            .size = CIOT_CRYPT_VALID_KEY_SIZE
        }
    };
    ciot_err_t err = ciot_crypt_enc(&crypt, (char*)input, encrypted, sizeof(encrypted));
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, err);
    TEST_ASSERT_NOT_EQUAL(0, strlen(encrypted));
    err = ciot_crypt_dec(&crypt, encrypted, decrypted, sizeof(decrypted));
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, err);
    TEST_ASSERT_EQUAL_STRING(input, decrypted);
}

void test_ciot_crypt(void)
{
    RUN_TEST(test_ciot_crypt_enc_null_crypt);
    RUN_TEST(test_ciot_crypt_enc_null_data);
    RUN_TEST(test_ciot_crypt_enc_null_buffer);
    RUN_TEST(test_ciot_crypt_enc_invalid_key_size);
    RUN_TEST(test_ciot_crypt_enc_invalid_size);
    RUN_TEST(test_ciot_crypt_enc_ok);

    RUN_TEST(test_ciot_crypt_dec_null_crypt);
    RUN_TEST(test_ciot_crypt_dec_null_data);
    RUN_TEST(test_ciot_crypt_dec_null_buffer);
    RUN_TEST(test_ciot_crypt_dec_invalid_key_size);
    RUN_TEST(test_ciot_crypt_dec_invalid_size);
    RUN_TEST(test_ciot_crypt_dec_ok);
    RUN_TEST(test_ciot_crypt_pkcs7_padding);
}
