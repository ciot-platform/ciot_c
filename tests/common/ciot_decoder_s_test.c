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
#include "ciot_config.h"

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

static uint8_t send_buf[CIOT_CONFIG_MSG_SIZE + 4];
static int send_size;
static int send_calls;
static ciot_err_t send_ret;

static ciot_err_t mock_send_data(ciot_iface_t *self, uint8_t *data, int size)
{
    send_calls++;
    if(send_ret == CIOT_ERR_OK && size <= (int)sizeof(send_buf))
    {
        memcpy(send_buf, data, size);
        send_size = size;
    }
    return send_ret;
}

static void send_setup(ciot_err_t ret)
{
    static uint8_t buf[32];
    memset(&iface, 0, sizeof(iface));
    iface.send_data = mock_send_data;
    iface.decoder = ciot_decoder_s_new(buf, sizeof(buf));
    send_size = 0;
    send_calls = 0;
    send_ret = ret;
}

// The whole frame goes to the iface in one call, so the iface can accept or refuse it whole
void test_ciot_decoder_s_send_whole_frame()
{
    uint8_t data[] = { 0x01, 0x02, 0x03, 0x04, 0x05 };
    uint8_t expected[] = { '{', 0x05, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, '}' };
    send_setup(CIOT_ERR_OK);

    ciot_err_t err = iface.decoder->send(iface.decoder, &iface, data, sizeof(data));

    TEST_ASSERT_EQUAL(CIOT_ERR_OK, err);
    TEST_ASSERT_EQUAL(1, send_calls);
    TEST_ASSERT_EQUAL(sizeof(expected), send_size);
    TEST_ASSERT_EQUAL_MEMORY(expected, send_buf, sizeof(expected));
}

// A frame larger than the receive buffer of this decoder can still be sent, and decodes back
void test_ciot_decoder_s_send_max_size_frame_decodes()
{
    static uint8_t data[CIOT_CONFIG_MSG_SIZE];
    static uint8_t rx_buf[CIOT_CONFIG_MSG_SIZE + 4];
    for (size_t i = 0; i < sizeof(data); i++) data[i] = (uint8_t)i;
    send_setup(CIOT_ERR_OK);

    ciot_err_t err = iface.decoder->send(iface.decoder, &iface, data, sizeof(data));
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, err);
    TEST_ASSERT_EQUAL(1, send_calls);
    TEST_ASSERT_EQUAL(sizeof(data) + 4, send_size);

    ciot_decoder_t rx = ciot_decoder_s_new(rx_buf, sizeof(rx_buf));
    for (int i = 0; i < send_size; i++)
    {
        err = rx->decode(rx, send_buf[i]);
        if(err != CIOT_ERR_OK) break;
    }
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, err);
    TEST_ASSERT_EQUAL(CIOT_DECODER_STATE_DONE, rx->state);
    TEST_ASSERT_EQUAL(sizeof(data), rx->result.size);
    TEST_ASSERT_EQUAL_MEMORY(data, rx->result.buf, sizeof(data));
}

// The error of send_data reaches the caller (ciot_iface_send_msg / ciot_iface_send_rsp)
void test_ciot_decoder_s_send_propagates_error()
{
    uint8_t data[] = { 0x01, 0x02, 0x03 };
    send_setup(CIOT_ERR_OVERFLOW);

    ciot_err_t err = iface.decoder->send(iface.decoder, &iface, data, sizeof(data));

    TEST_ASSERT_EQUAL(CIOT_ERR_OVERFLOW, err);
    TEST_ASSERT_EQUAL(1, send_calls);
}

// A payload that does not fit in a frame is refused before anything is sent
void test_ciot_decoder_s_send_too_big()
{
    static uint8_t data[CIOT_CONFIG_MSG_SIZE + 1];
    send_setup(CIOT_ERR_OK);

    ciot_err_t err = iface.decoder->send(iface.decoder, &iface, data, sizeof(data));

    TEST_ASSERT_EQUAL(CIOT_ERR_OVERFLOW, err);
    TEST_ASSERT_EQUAL(0, send_calls);
}

void test_ciot_decoder_s()
{
    RUN_TEST(test_ciot_decoder_s_decode_buffer_null);
    RUN_TEST(test_ciot_decoder_s_decode_overflow);
    RUN_TEST(test_ciot_decoder_s_decode_protocol_violation);
    RUN_TEST(test_ciot_decoder_s_decode_ok);
    RUN_TEST(test_ciot_decoder_s_send_whole_frame);
    RUN_TEST(test_ciot_decoder_s_send_max_size_frame_decodes);
    RUN_TEST(test_ciot_decoder_s_send_propagates_error);
    RUN_TEST(test_ciot_decoder_s_send_too_big);
}
