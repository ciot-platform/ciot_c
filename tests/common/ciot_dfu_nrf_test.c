/**
 * @file ciot_dfu_nrf_test.c
 * @brief Session handling of ciot_dfu_nrf: ciot_dfu_nrf_reset() starts a new session
 * on the iface that is still open, ciot_dfu_nrf_stop() also closes it.
 *
 * The transport is a mock iface: the config request "opens" it and answers with
 * STARTED, as ciot_uart_start does, and the stop request "closes" it.
 */

#include <stdlib.h>
#include <string.h>

#include "unity.h"
#include "ciot_iface.h"
#include "ciot_dfu_nrf.h"

static ciot_iface_t g_link;
static ciot_msg_data_t g_link_cfg;
static int g_link_opens;
static int g_link_closes;
static ciot_err_t g_link_open_err;
static uint8_t g_tx[64];
static int g_tx_size;

static ciot_err_t link_process_data(ciot_iface_t *iface, ciot_msg_data_t *data)
{
    if (data == &g_link_cfg)
    {
        if (g_link_open_err != CIOT_ERR_OK)
        {
            return g_link_open_err;
        }
        g_link_opens++;
        // Like ciot_uart_start: STARTED on every start, open or not.
        ciot_iface_send_event_type(iface, CIOT_EVENT_TYPE_STARTED);
        return CIOT_ERR_OK;
    }
    if (data->common.which_type == CIOT_COMMON_STOP_TAG)
    {
        g_link_closes++;
        return CIOT_ERR_OK;
    }
    return CIOT_ERR_INVALID_ARG;
}

static ciot_err_t link_get_data(ciot_iface_t *iface, ciot_msg_data_t *data)
{
    return CIOT_ERR_OK;
}

static ciot_err_t link_send_data(ciot_iface_t *iface, uint8_t *data, int size)
{
    for (int i = 0; i < size && g_tx_size < (int)sizeof(g_tx); i++)
    {
        g_tx[g_tx_size++] = data[i];
    }
    return CIOT_ERR_OK;
}

static ciot_dfu_nrf_t new_dfu(void)
{
    memset(&g_link, 0, sizeof(g_link));
    memset(&g_link_cfg, 0, sizeof(g_link_cfg));
    g_link.process_data = link_process_data;
    g_link.get_data = link_get_data;
    g_link.send_data = link_send_data;
    g_link_cfg.which_type = CIOT_MSG_DATA_UART_TAG;
    g_link_opens = 0;
    g_link_closes = 0;
    g_link_open_err = CIOT_ERR_OK;
    g_tx_size = 0;

    ciot_dfu_nrf_cfg_t cfg = {
        .dfu.type = CIOT_DFU_TYPE_UART,
        .max_package_len = 64,
        .iface = &g_link,
        .iface_cfg = &g_link_cfg,
    };
    ciot_dfu_nrf_t dfu = ciot_dfu_nrf_new(&cfg);
    ciot_dfu_nrf_set_test_mode(dfu, true);
    return dfu;
}

static ciot_err_t start(ciot_dfu_nrf_t dfu)
{
    ciot_dfu_cfg_t cfg = { .type = CIOT_DFU_TYPE_UART };
    return ciot_dfu_nrf_start(dfu, &cfg);
}

// Runs the task until the ping goes out and checks the SLIP frame [09 01 C0].
static void send_ping(ciot_dfu_nrf_t dfu)
{
    g_tx_size = 0;
    TEST_ASSERT_EQUAL(CIOT_DFU_NRF_STATE_SEND_PING, ciot_dfu_nrf_state(dfu));
    ciot_dfu_nrf_task(dfu);
    TEST_ASSERT_EQUAL(CIOT_DFU_NRF_STATE_WAITING_PING_RESP, ciot_dfu_nrf_state(dfu));
    uint8_t ping[] = {0x09, 0x01, 0xC0};
    TEST_ASSERT_EQUAL(sizeof(ping), g_tx_size);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(ping, g_tx, sizeof(ping));
}

// The bootloader answers the ping with result code `code`: [60 09 code C0].
static void answer_ping(uint8_t code)
{
    uint8_t rsp[] = {0x60, 0x09, code, 0xC0};
    ciot_iface_process_data(&g_link, rsp, sizeof(rsp), CIOT_EVENT_TYPE_MSG);
}

// A ping-only session ends in COMPLETED with the link open. reset starts the next one
// on the same link, stop closes it.
void test_ciot_dfu_nrf_reset_starts_new_session_on_open_iface(void)
{
    ciot_dfu_nrf_t dfu = new_dfu();

    TEST_ASSERT_EQUAL(CIOT_ERR_OK, start(dfu));
    TEST_ASSERT_EQUAL(1, g_link_opens);
    send_ping(dfu);
    answer_ping(0x01);
    TEST_ASSERT_EQUAL(CIOT_DFU_NRF_STATE_COMPLETED, ciot_dfu_nrf_state(dfu));
    TEST_ASSERT_EQUAL(0, g_link_closes);

    // A session must end (reset or stop) before the next one starts.
    TEST_ASSERT_EQUAL(CIOT_ERR_INVALID_STATE, start(dfu));

    TEST_ASSERT_EQUAL(CIOT_ERR_OK, ciot_dfu_nrf_reset(dfu));
    TEST_ASSERT_EQUAL(CIOT_DFU_NRF_STATE_IDLE, ciot_dfu_nrf_state(dfu));
    TEST_ASSERT_EQUAL(0, g_link_closes);
    TEST_ASSERT_NULL(g_link.event_handler);

    TEST_ASSERT_EQUAL(CIOT_ERR_OK, start(dfu));
    TEST_ASSERT_EQUAL(2, g_link_opens);
    send_ping(dfu);
    answer_ping(0x01);
    TEST_ASSERT_EQUAL(CIOT_DFU_NRF_STATE_COMPLETED, ciot_dfu_nrf_state(dfu));

    TEST_ASSERT_EQUAL(CIOT_ERR_OK, ciot_dfu_nrf_stop(dfu));
    TEST_ASSERT_EQUAL(CIOT_DFU_NRF_STATE_IDLE, ciot_dfu_nrf_state(dfu));
    TEST_ASSERT_EQUAL(1, g_link_closes);
}

// The first ping refused with OP_CODE_NOT_SUPPORTED is retried once per session,
// not once per ciot_dfu_nrf_t.
void test_ciot_dfu_nrf_reset_allows_ping_retry_again(void)
{
    ciot_dfu_nrf_t dfu = new_dfu();

    TEST_ASSERT_EQUAL(CIOT_ERR_OK, start(dfu));
    send_ping(dfu);
    answer_ping(0x02);
    TEST_ASSERT_EQUAL(CIOT_DFU_NRF_STATE_SEND_PING, ciot_dfu_nrf_state(dfu));
    send_ping(dfu);
    answer_ping(0x02);
    TEST_ASSERT_EQUAL(CIOT_DFU_NRF_STATE_ERROR, ciot_dfu_nrf_state(dfu));

    TEST_ASSERT_EQUAL(CIOT_ERR_OK, ciot_dfu_nrf_reset(dfu));
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, start(dfu));
    send_ping(dfu);
    answer_ping(0x02);
    TEST_ASSERT_EQUAL(CIOT_DFU_NRF_STATE_SEND_PING, ciot_dfu_nrf_state(dfu));
    send_ping(dfu);
    answer_ping(0x01);
    TEST_ASSERT_EQUAL(CIOT_DFU_NRF_STATE_COMPLETED, ciot_dfu_nrf_state(dfu));
}

// A frame cut in the previous session must not corrupt the first response of the next.
void test_ciot_dfu_nrf_reset_drops_partial_frame(void)
{
    ciot_dfu_nrf_t dfu = new_dfu();

    TEST_ASSERT_EQUAL(CIOT_ERR_OK, start(dfu));
    send_ping(dfu);
    answer_ping(0x01);
    uint8_t cut[] = {0x60, 0x03};
    ciot_iface_process_data(&g_link, cut, sizeof(cut), CIOT_EVENT_TYPE_MSG);

    TEST_ASSERT_EQUAL(CIOT_ERR_OK, ciot_dfu_nrf_reset(dfu));
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, start(dfu));
    send_ping(dfu);
    answer_ping(0x01);
    TEST_ASSERT_EQUAL(CIOT_DFU_NRF_STATE_COMPLETED, ciot_dfu_nrf_state(dfu));
}

static int g_opens_at_request;

static ciot_err_t data_link_send_data(ciot_iface_t *iface, uint8_t *data, int size)
{
    if (g_opens_at_request < 0)
    {
        g_opens_at_request = g_link_opens;
    }
    return CIOT_ERR_OK;
}

// The DFU link is open, with the SLIP decoder, before the INIT_DFU request goes out,
// also after a stop closed it: the bootloader must start on an idle line.
void test_ciot_dfu_nrf_start_bootloader_opens_link_first(void)
{
    ciot_dfu_nrf_t dfu = new_dfu();
    ciot_iface_t data_link = { .send_data = data_link_send_data };

    g_opens_at_request = -1;
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, ciot_dfu_nrf_start_bootloader(dfu, &data_link, 1));
    TEST_ASSERT_EQUAL(1, g_opens_at_request);
    TEST_ASSERT_NOT_NULL(g_link.decoder);

    TEST_ASSERT_EQUAL(CIOT_ERR_OK, start(dfu));
    send_ping(dfu);
    answer_ping(0x01);
    TEST_ASSERT_EQUAL(CIOT_DFU_NRF_STATE_COMPLETED, ciot_dfu_nrf_state(dfu));
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, ciot_dfu_nrf_stop(dfu));
    TEST_ASSERT_EQUAL(1, g_link_closes);

    g_opens_at_request = -1;
    data_link.req_status.state = CIOT_IFACE_REQ_STATE_IDLE;
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, ciot_dfu_nrf_start_bootloader(dfu, &data_link, 1));
    TEST_ASSERT_EQUAL(3, g_opens_at_request);
}

// Bytes received while the device resets into the bootloader (link already open,
// no session yet) must not prefix the ping response.
void test_ciot_dfu_nrf_start_drops_bytes_received_before_it(void)
{
    ciot_dfu_nrf_t dfu = new_dfu();
    ciot_iface_t data_link = { .send_data = data_link_send_data };

    g_opens_at_request = -1;
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, ciot_dfu_nrf_start_bootloader(dfu, &data_link, 1));
    uint8_t noise[] = {0x00, 0x60};
    ciot_iface_process_data(&g_link, noise, sizeof(noise), CIOT_EVENT_TYPE_MSG);

    TEST_ASSERT_EQUAL(CIOT_ERR_OK, start(dfu));
    send_ping(dfu);
    answer_ping(0x01);
    TEST_ASSERT_EQUAL(CIOT_DFU_NRF_STATE_COMPLETED, ciot_dfu_nrf_state(dfu));
}

// A link that can't be opened before the bootloader runs doesn't stop INIT_DFU.
void test_ciot_dfu_nrf_start_bootloader_sends_request_when_open_fails(void)
{
    ciot_dfu_nrf_t dfu = new_dfu();
    ciot_iface_t data_link = { .send_data = data_link_send_data };

    g_link_open_err = CIOT_ERR_NOT_IMPLEMENTED;
    g_opens_at_request = -1;
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, ciot_dfu_nrf_start_bootloader(dfu, &data_link, 1));
    TEST_ASSERT_EQUAL(0, g_opens_at_request);
}

// A handler left by a session that ended in ERROR without reset doesn't see the
// STARTED event of the early open.
void test_ciot_dfu_nrf_start_bootloader_releases_left_over_handler(void)
{
    ciot_dfu_nrf_t dfu = new_dfu();
    ciot_iface_t data_link = { .send_data = data_link_send_data };

    TEST_ASSERT_EQUAL(CIOT_ERR_OK, start(dfu));
    TEST_ASSERT_NOT_NULL(g_link.event_handler);

    g_opens_at_request = -1;
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, ciot_dfu_nrf_start_bootloader(dfu, &data_link, 1));
    TEST_ASSERT_NULL(g_link.event_handler);
    TEST_ASSERT_EQUAL(CIOT_DFU_NRF_STATE_SEND_PING, ciot_dfu_nrf_state(dfu)); // from start, not moved by the open
}

void test_ciot_dfu_nrf(void)
{
    RUN_TEST(test_ciot_dfu_nrf_start_drops_bytes_received_before_it);
    RUN_TEST(test_ciot_dfu_nrf_start_bootloader_sends_request_when_open_fails);
    RUN_TEST(test_ciot_dfu_nrf_start_bootloader_releases_left_over_handler);
    RUN_TEST(test_ciot_dfu_nrf_start_bootloader_opens_link_first);
    RUN_TEST(test_ciot_dfu_nrf_reset_starts_new_session_on_open_iface);
    RUN_TEST(test_ciot_dfu_nrf_reset_allows_ping_retry_again);
    RUN_TEST(test_ciot_dfu_nrf_reset_drops_partial_frame);
}
