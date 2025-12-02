/**
 * @file ciot_decoder_s_test.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-04-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <stdlib.h>
#include <string.h>
#include "unity.h"
#include "ciot_iface.h"
#include "ciot_decoder_s.h"

static ciot_iface_t iface;
static uint8_t encode_result[64];
static int encode_result_size;

void test_ciot_decoder_s_decode_buffer_null()
{
    iface.decoder = ciot_decoder_s_new(NULL, 0);
    ciot_err_t err = iface.decoder->decode(iface.decoder, 0);
    TEST_ASSERT(err == CIOT_ERR_NULL_ARG);
}

void test_ciot_decoder_s_decode_overflow()
{
    ciot_err_t err;
    uint8_t buf[4];
    uint8_t data[] = { '{', 0x05, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, '}' };
    iface.decoder = ciot_decoder_s_new(buf, sizeof(buf));

    for (size_t i = 0; i < sizeof(data); i++)
    {
        err = iface.decoder->decode(iface.decoder, data[i]);
        if(err != CIOT_ERR_OK) break;
    }
    
    TEST_ASSERT(iface.decoder->state == CIOT_DECODER_STATE_ERROR);
    TEST_ASSERT(err == CIOT_ERR_OVERFLOW);
}

void test_ciot_decoder_s_decode_protocol_violation()
{
    ciot_err_t err;
    uint8_t buf[32];
    uint8_t data[] = { '{', 0x05, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, '}' };
    iface.decoder = ciot_decoder_s_new(buf, sizeof(buf));

    for (size_t i = 0; i < sizeof(data); i++)
    {
        err = iface.decoder->decode(iface.decoder, data[i]);
        if(err != CIOT_ERR_OK) break;
    }
    
    TEST_ASSERT(iface.decoder->state == CIOT_DECODER_STATE_ERROR);
    TEST_ASSERT(err == CIOT_ERR_PROTOCOL_VIOLATION);
}

void test_ciot_decoder_s_decode_ok()
{
    ciot_err_t err;
    uint8_t buf[32] = { 0 };
    uint8_t data[] = { '{', 0x05, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, '}' };
    uint8_t expected[] = { 0x01, 0x02, 0x03, 0x04, 0x05 };
    iface.decoder = ciot_decoder_s_new(buf, sizeof(buf));

    for (size_t i = 0; i < sizeof(data); i++)
    {
        err = iface.decoder->decode(iface.decoder, data[i]);
        if(err != CIOT_ERR_OK) break;
    }
    
    TEST_ASSERT_EQUAL(err, CIOT_ERR_OK);
    TEST_ASSERT_EQUAL_MEMORY(expected, iface.decoder->result.buf, sizeof(expected));
}

void test_ciot_decoder_s()
{
    RUN_TEST(test_ciot_decoder_s_decode_buffer_null);
    RUN_TEST(test_ciot_decoder_s_decode_overflow);
    RUN_TEST(test_ciot_decoder_s_decode_protocol_violation);
    RUN_TEST(test_ciot_decoder_s_decode_ok);
}
