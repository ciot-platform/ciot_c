#include <stdlib.h>
#include <string.h>

#include "unity.h"
#include "ciot.h"
#include "ciot_iface.h"

static ciot_t g_ciot;
static ciot_iface_t g_iface_1;
static ciot_iface_t *g_ifaces[2];
static ciot_msg_data_t g_cfg_1;
static ciot_msg_data_t *g_cfgs[2];

static int g_app_handler_calls;
static ciot_err_t g_app_handler_ret;
static int g_sender_send_calls;

static ciot_err_t mock_iface_process_data(ciot_iface_t *iface, ciot_msg_data_t *data)
{
    (void)iface;
    (void)data;
    return CIOT_ERR_OK;
}

static ciot_err_t mock_iface_get_data(ciot_iface_t *iface, ciot_msg_data_t *data)
{
    (void)iface;
    (void)data;
    return CIOT_ERR_OK;
}

static ciot_err_t mock_iface_send_data(ciot_iface_t *iface, uint8_t *data, int size)
{
    (void)iface;
    (void)data;
    (void)size;
    g_sender_send_calls++;
    return CIOT_ERR_OK;
}

static ciot_err_t mock_app_event_handler(ciot_iface_t *sender, ciot_event_t *event, void *args)
{
    (void)sender;
    (void)event;
    (void)args;
    g_app_handler_calls++;
    return g_app_handler_ret;
}

static void queue_test_setup_common(void)
{
    g_ciot = ciot_new();
    TEST_ASSERT_NOT_NULL(g_ciot);

    memset(&g_iface_1, 0, sizeof(g_iface_1));
    g_iface_1.info.type = CIOT_IFACE_TYPE_CUSTOM;
    g_iface_1.process_data = mock_iface_process_data;
    g_iface_1.get_data = mock_iface_get_data;
    g_iface_1.send_data = mock_iface_send_data;

    g_ifaces[0] = NULL;
    g_ifaces[1] = &g_iface_1;

    memset(&g_cfg_1, 0, sizeof(g_cfg_1));
    g_cfg_1.which_type = CIOT_MSG_DATA_GET_DATA_TAG;

    g_cfgs[0] = NULL;
    g_cfgs[1] = &g_cfg_1;

    g_app_handler_calls = 0;
    g_app_handler_ret = CIOT_ERR_OK;
    g_sender_send_calls = 0;
}

static void queue_test_cleanup_common(void)
{
    if(g_ciot != NULL)
    {
        free(g_ciot);
        g_ciot = NULL;
    }
}

void test_ciot_queue_starting_discards_unexpected_event_and_continues(void)
{
    queue_test_setup_common();

    ciot_cfg_t cfg = { 0 };
    cfg.ifaces.list = g_ifaces;
    cfg.ifaces.cfgs = g_cfgs;
    cfg.ifaces.count = 2;

    TEST_ASSERT_EQUAL(CIOT_ERR_OK, ciot_start(g_ciot, &cfg));
    TEST_ASSERT_EQUAL(CIOT_STATE_STARTING, g_ciot->status.state);

    TEST_ASSERT_EQUAL(CIOT_ERR_OK, ciot_task(g_ciot));
    TEST_ASSERT_TRUE(g_ciot->starter.waiting_result);
    TEST_ASSERT_EQUAL_UINT8(0, g_ciot->receiver.queue.count);

    ciot_event_t unexpected = { 0 };
    unexpected.type = CIOT_EVENT_TYPE_DATA;
    unexpected.which_data = CIOT_EVENT_RAW_TAG;
    unexpected.raw.size = 1;
    unexpected.raw.bytes[0] = 0xAA;

    TEST_ASSERT_EQUAL(CIOT_ERR_OK, ciot_iface_send_event(&g_iface_1, &unexpected));
    TEST_ASSERT_EQUAL_UINT8(1, g_ciot->receiver.queue.count);

    TEST_ASSERT_EQUAL(CIOT_ERR_OK, ciot_task(g_ciot));
    TEST_ASSERT_TRUE(g_ciot->starter.waiting_result);
    TEST_ASSERT_EQUAL_UINT8(0, g_ciot->receiver.queue.count);
    TEST_ASSERT_EQUAL_UINT32(1, g_ciot->starter.iface_id);

    ciot_event_t expected_ack = { 0 };
    expected_ack.type = CIOT_EVENT_TYPE_STARTED;
    expected_ack.which_data = CIOT_EVENT_MSG_TAG;

    TEST_ASSERT_EQUAL(CIOT_ERR_OK, ciot_iface_send_event(&g_iface_1, &expected_ack));
    TEST_ASSERT_EQUAL_UINT8(1, g_ciot->receiver.queue.count);

    TEST_ASSERT_EQUAL(CIOT_ERR_OK, ciot_task(g_ciot));
    TEST_ASSERT_EQUAL(CIOT_STATE_STARTED, g_ciot->status.state);
    TEST_ASSERT_EQUAL_UINT8(0, g_ciot->receiver.queue.count);

    queue_test_cleanup_common();
}

void test_ciot_queue_custom_msg_handler_error_does_not_leave_dead_item(void)
{
    queue_test_setup_common();

    ciot_cfg_t cfg = { 0 };
    cfg.ifaces.list = g_ifaces;
    cfg.ifaces.cfgs = g_cfgs;
    cfg.ifaces.count = 2;

    TEST_ASSERT_EQUAL(CIOT_ERR_OK, ciot_start(g_ciot, &cfg));
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, ciot_task(g_ciot));

    ciot_event_t expected_ack = { 0 };
    expected_ack.type = CIOT_EVENT_TYPE_STARTED;
    expected_ack.which_data = CIOT_EVENT_MSG_TAG;
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, ciot_iface_send_event(&g_iface_1, &expected_ack));
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, ciot_task(g_ciot));
    TEST_ASSERT_EQUAL(CIOT_STATE_STARTED, g_ciot->status.state);

    TEST_ASSERT_EQUAL(CIOT_ERR_OK, ciot_iface_set_event_handler(&g_ciot->iface, mock_app_event_handler, NULL));

    g_app_handler_ret = CIOT_ERR_INVALID_STATE;

    ciot_event_t custom_msg = { 0 };
    custom_msg.type = CIOT_EVENT_TYPE_MSG;
    custom_msg.which_data = CIOT_EVENT_MSG_TAG;
    custom_msg.msg.type = CIOT_MSG_TYPE_REQUEST;
    custom_msg.msg.error = CIOT_ERR_OK;
    custom_msg.msg.has_iface = true;
    custom_msg.msg.iface.type = CIOT_IFACE_TYPE_CUSTOM;
    custom_msg.msg.iface.id = g_iface_1.info.id;
    custom_msg.msg.has_proxy = false;
    custom_msg.msg.has_data = true;
    custom_msg.msg.data.which_type = CIOT_MSG_DATA_GET_DATA_TAG;

    TEST_ASSERT_EQUAL(CIOT_ERR_OK, ciot_iface_send_event(&g_iface_1, &custom_msg));
    TEST_ASSERT_EQUAL_UINT8(0, g_ciot->receiver.queue.count);
    TEST_ASSERT_EQUAL_INT(1, g_app_handler_calls);
    TEST_ASSERT_TRUE(g_sender_send_calls > 0);

    queue_test_cleanup_common();
}
