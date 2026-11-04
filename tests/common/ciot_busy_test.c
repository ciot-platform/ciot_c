#include <stdlib.h>
#include <string.h>

#include "unity.h"
#include "ciot.h"
#include "ciot_iface.h"

// Messages that the core does not need to process (data a device sends on its own, e.g. advs and
// acks from a BLE coprocessor over a UART) must reach the application even while the core is busy.

static ciot_t g_ciot;
static ciot_iface_t g_req_iface;
static ciot_iface_t g_dev_iface;
static ciot_iface_t *g_ifaces[3];
static ciot_msg_data_t g_cfg;
static ciot_msg_data_t *g_cfgs[3];

static int g_app_calls;
static ciot_msg_t g_app_last_msg;
static int g_busy_calls;
static ciot_err_t g_app_ret;
static uint8_t g_sent[512];
static int g_sent_size;

static ciot_err_t mock_process_data(ciot_iface_t *iface, ciot_msg_data_t *data)
{
    (void)iface;
    (void)data;
    return CIOT_ERR_OK;
}

static ciot_err_t mock_get_data(ciot_iface_t *iface, ciot_msg_data_t *data)
{
    (void)iface;
    (void)data;
    return CIOT_ERR_OK;
}

static ciot_err_t mock_send_data(ciot_iface_t *iface, uint8_t *data, int size)
{
    (void)iface;
    if(size > 0 && size <= (int)sizeof(g_sent))
    {
        memcpy(g_sent, data, size);
        g_sent_size = size;
    }
    return CIOT_ERR_OK;
}

static ciot_err_t mock_app_handler(ciot_iface_t *sender, ciot_event_t *event, void *args)
{
    (void)sender;
    (void)args;
    if(event->type == CIOT_EVENT_TYPE_MSG && event->which_data == CIOT_EVENT_MSG_TAG)
    {
        g_app_calls++;
        g_app_last_msg = event->msg;
    }
    return g_app_ret;
}

static ciot_err_t mock_busy_handler(ciot_iface_t *sender, ciot_event_t *event, void *args)
{
    (void)sender;
    (void)event;
    (void)args;
    g_busy_calls++;
    return CIOT_ERR_OK;
}

static void init_iface(ciot_iface_t *iface, ciot_iface_type_t type)
{
    memset(iface, 0, sizeof(*iface));
    iface->info.type = type;
    iface->process_data = mock_process_data;
    iface->get_data = mock_get_data;
    iface->send_data = mock_send_data;
}

static void busy_test_setup(void)
{
    g_ciot = ciot_new();
    TEST_ASSERT_NOT_NULL(g_ciot);

    // id 1: receives requests (e.g. MQTT); id 2: a device that sends data on its own (e.g. a UART)
    init_iface(&g_req_iface, CIOT_IFACE_TYPE_MQTT_CLIENT);
    init_iface(&g_dev_iface, CIOT_IFACE_TYPE_UART);

    g_ifaces[0] = NULL;
    g_ifaces[1] = &g_req_iface;
    g_ifaces[2] = &g_dev_iface;

    memset(&g_cfg, 0, sizeof(g_cfg));
    g_cfg.which_type = CIOT_MSG_DATA_GET_DATA_TAG;
    g_cfgs[0] = NULL;
    g_cfgs[1] = &g_cfg;
    g_cfgs[2] = NULL;

    g_app_calls = 0;
    g_busy_calls = 0;
    g_app_ret = CIOT_ERR_OK;
    g_sent_size = 0;
    memset(&g_app_last_msg, 0, sizeof(g_app_last_msg));

    ciot_cfg_t cfg = { 0 };
    cfg.ifaces.list = g_ifaces;
    cfg.ifaces.cfgs = g_cfgs;
    cfg.ifaces.count = 3;

    TEST_ASSERT_EQUAL(CIOT_ERR_OK, ciot_start(g_ciot, &cfg));
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, ciot_task(g_ciot));

    ciot_event_t started = { 0 };
    started.type = CIOT_EVENT_TYPE_STARTED;
    started.which_data = CIOT_EVENT_MSG_TAG;
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, ciot_iface_send_event(&g_req_iface, &started));
    for (int i = 0; i < 4 && g_ciot->status.state != CIOT_STATE_STARTED; i++)
    {
        TEST_ASSERT_EQUAL(CIOT_ERR_OK, ciot_task(g_ciot));
    }
    TEST_ASSERT_EQUAL(CIOT_STATE_STARTED, g_ciot->status.state);

    TEST_ASSERT_EQUAL(CIOT_ERR_OK, ciot_iface_set_event_handler(&g_ciot->iface, mock_app_handler, NULL));
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, ciot_set_busy_handler(g_ciot, mock_busy_handler, NULL));
}

static void busy_test_cleanup(void)
{
    if(g_ciot != NULL)
    {
        free(g_ciot);
        g_ciot = NULL;
    }
}

static ciot_event_t raw_event(ciot_msg_t *msg)
{
    ciot_event_t event = { 0 };
    event.type = CIOT_EVENT_TYPE_MSG;
    event.which_data = CIOT_EVENT_RAW_TAG;
    event.raw.size = ciot_serializer_to_bytes(event.raw.bytes, sizeof(event.raw.bytes), msg, CIOT_MSG_FIELDS);
    TEST_ASSERT_GREATER_THAN(0, event.raw.size);
    return event;
}

// A request for a core iface: takes the receiver and makes the core busy until the next ciot_task
static void make_core_busy(void)
{
    ciot_msg_t req = { 0 };
    req.id = 100;
    req.type = CIOT_MSG_TYPE_REQUEST;
    req.has_iface = true;
    req.iface.type = CIOT_IFACE_TYPE_MQTT_CLIENT;
    req.iface.id = 1;
    req.has_data = true;
    req.data.which_type = CIOT_MSG_DATA_GET_DATA_TAG;
    req.data.get_data.type = CIOT_DATA_TYPE_STATUS;

    ciot_event_t event = raw_event(&req);
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, ciot_iface_send_event(&g_req_iface, &event));
    TEST_ASSERT_EQUAL(CIOT_STATE_BUSY, g_ciot->status.state);
    TEST_ASSERT_EQUAL(0, g_app_calls);
}

// A message the device sends on its own, like an ack: a response without iface
static ciot_msg_t device_data_msg(uint32_t id)
{
    ciot_msg_t msg = { 0 };
    msg.id = id;
    msg.type = CIOT_MSG_TYPE_RESPONSE;
    msg.has_data = true;
    msg.data.which_type = CIOT_MSG_DATA_GET_DATA_TAG;
    msg.data.get_data.type = CIOT_DATA_TYPE_INFO;
    return msg;
}

#if CIOT_CONFIG_DIRECT_MSG == 1

void test_ciot_busy_device_data_does_not_take_receiver(void)
{
    busy_test_setup();

    ciot_msg_t msg = device_data_msg(7);
    ciot_event_t event = raw_event(&msg);
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, ciot_iface_send_event(&g_dev_iface, &event));

    // Before: the core went busy and the message reached the application only on the next ciot_task
    TEST_ASSERT_EQUAL(CIOT_STATE_STARTED, g_ciot->status.state);
    TEST_ASSERT_EQUAL(1, g_app_calls);
    TEST_ASSERT_EQUAL_UINT32(7, g_app_last_msg.id);

    busy_test_cleanup();
}

// Device data is decoded apart from receiver.event, so another sender's request is never overwritten
void test_ciot_busy_device_data_leaves_receiver_untouched(void)
{
    busy_test_setup();

    g_ciot->receiver.event.msg.id = 55;
    ciot_msg_t msg = device_data_msg(11);
    ciot_event_t event = raw_event(&msg);
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, ciot_iface_send_event(&g_dev_iface, &event));

    TEST_ASSERT_EQUAL(1, g_app_calls);
    TEST_ASSERT_EQUAL_UINT32(11, g_app_last_msg.id);
    TEST_ASSERT_EQUAL_UINT32(55, g_ciot->receiver.event.msg.id);

    busy_test_cleanup();
}

void test_ciot_busy_device_data_reaches_app_while_busy(void)
{
    busy_test_setup();
    make_core_busy();

    ciot_msg_t msg = device_data_msg(8);
    ciot_event_t event = raw_event(&msg);
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, ciot_iface_send_event(&g_dev_iface, &event));

    TEST_ASSERT_EQUAL(0, g_busy_calls);
    TEST_ASSERT_EQUAL(1, g_app_calls);
    TEST_ASSERT_EQUAL_UINT32(8, g_app_last_msg.id);

    // The pending request is untouched and still processed on the next task
    TEST_ASSERT_EQUAL(CIOT_STATE_BUSY, g_ciot->status.state);
    TEST_ASSERT_EQUAL_UINT32(100, g_ciot->receiver.event.msg.id);
    TEST_ASSERT_TRUE(g_ciot->receiver.sender == &g_req_iface);
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, ciot_task(g_ciot));
    TEST_ASSERT_EQUAL(CIOT_STATE_STARTED, g_ciot->status.state);

    busy_test_cleanup();
}

void test_ciot_busy_custom_msg_reaches_app_while_busy(void)
{
    busy_test_setup();
    make_core_busy();

    ciot_msg_t msg = { 0 };
    msg.id = 9;
    msg.has_iface = true;
    msg.iface.type = CIOT_IFACE_TYPE_CUSTOM;
    msg.iface.id = 5;
    msg.has_data = true;
    msg.data.which_type = CIOT_MSG_DATA_GET_DATA_TAG;
    ciot_event_t event = raw_event(&msg);
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, ciot_iface_send_event(&g_dev_iface, &event));

    TEST_ASSERT_EQUAL(0, g_busy_calls);
    TEST_ASSERT_EQUAL(1, g_app_calls);
    TEST_ASSERT_EQUAL_UINT32(9, g_app_last_msg.id);
    TEST_ASSERT_EQUAL(CIOT_STATE_BUSY, g_ciot->status.state);
    TEST_ASSERT_EQUAL_UINT32(100, g_ciot->receiver.event.msg.id);

    busy_test_cleanup();
}

#else // CIOT_CONFIG_DIRECT_MSG == 0 (default): device data takes the receiver, as before

void test_ciot_busy_device_data_takes_receiver_by_default(void)
{
    busy_test_setup();

    ciot_msg_t msg = device_data_msg(7);
    ciot_event_t event = raw_event(&msg);
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, ciot_iface_send_event(&g_dev_iface, &event));

    // Reaches the application only from ciot_task
    TEST_ASSERT_EQUAL(CIOT_STATE_BUSY, g_ciot->status.state);
    TEST_ASSERT_EQUAL(0, g_app_calls);
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, ciot_task(g_ciot));
    TEST_ASSERT_EQUAL(CIOT_STATE_STARTED, g_ciot->status.state);
    TEST_ASSERT_EQUAL(1, g_app_calls);
    TEST_ASSERT_EQUAL_UINT32(7, g_app_last_msg.id);

    busy_test_cleanup();
}

void test_ciot_busy_device_data_dropped_while_busy_by_default(void)
{
    busy_test_setup();
    make_core_busy();

    ciot_msg_t msg = device_data_msg(8);
    ciot_event_t event = raw_event(&msg);
    TEST_ASSERT_EQUAL(CIOT_ERR_BUSY, ciot_iface_send_event(&g_dev_iface, &event));

    TEST_ASSERT_EQUAL(1, g_busy_calls);
    TEST_ASSERT_EQUAL(0, g_app_calls);
    TEST_ASSERT_EQUAL_UINT32(100, g_ciot->receiver.event.msg.id);

    busy_test_cleanup();
}

#endif // CIOT_CONFIG_DIRECT_MSG

// A frame that does not decode is dropped: it neither reaches the application nor takes the receiver
void test_ciot_busy_corrupt_msg_dropped(void)
{
    busy_test_setup();

    ciot_event_t event = { 0 };
    event.type = CIOT_EVENT_TYPE_MSG;
    event.which_data = CIOT_EVENT_RAW_TAG;
    event.raw.bytes[0] = 0xFF;
    event.raw.bytes[1] = 0xFF;
    event.raw.bytes[2] = 0xFF;
    event.raw.size = 3;
    TEST_ASSERT_EQUAL(CIOT_ERR_DESERIALIZATION, ciot_iface_send_event(&g_dev_iface, &event));

    TEST_ASSERT_EQUAL(0, g_app_calls);
    TEST_ASSERT_EQUAL(CIOT_STATE_STARTED, g_ciot->status.state);

    busy_test_cleanup();
}

// The error sent back for a failed CUSTOM msg carries the iface id it was addressed to
void test_ciot_busy_custom_error_carries_iface_id(void)
{
    busy_test_setup();
    g_app_ret = CIOT_ERR_FAIL;

    ciot_msg_t msg = { 0 };
    msg.id = 12;
    msg.has_iface = true;
    msg.iface.type = CIOT_IFACE_TYPE_CUSTOM;
    msg.iface.id = 5;
    msg.has_data = true;
    msg.data.which_type = CIOT_MSG_DATA_GET_DATA_TAG;
    ciot_event_t event = raw_event(&msg);
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, ciot_iface_send_event(&g_dev_iface, &event));

    TEST_ASSERT_GREATER_THAN(0, g_sent_size);
    ciot_msg_t sent = { 0 };
    TEST_ASSERT_GREATER_THAN(0, ciot_serializer_from_bytes(g_sent, g_sent_size, &sent, CIOT_MSG_FIELDS));
    TEST_ASSERT_EQUAL(CIOT_IFACE_TYPE_CUSTOM, sent.iface.type);
    TEST_ASSERT_EQUAL_UINT32(5, sent.iface.id);
    TEST_ASSERT_EQUAL(CIOT_ERR_FAIL, sent.error);

    busy_test_cleanup();
}

void test_ciot_busy_request_still_rejected_while_busy(void)
{
    busy_test_setup();
    make_core_busy();

    ciot_msg_t req = { 0 };
    req.id = 101;
    req.type = CIOT_MSG_TYPE_REQUEST;
    req.has_iface = true;
    req.iface.type = CIOT_IFACE_TYPE_UART;
    req.iface.id = 2;
    req.has_data = true;
    req.data.which_type = CIOT_MSG_DATA_GET_DATA_TAG;
    ciot_event_t event = raw_event(&req);
    TEST_ASSERT_EQUAL(CIOT_ERR_BUSY, ciot_iface_send_event(&g_dev_iface, &event));

    TEST_ASSERT_EQUAL(1, g_busy_calls);
    TEST_ASSERT_EQUAL(0, g_app_calls);
    TEST_ASSERT_EQUAL_UINT32(100, g_ciot->receiver.event.msg.id);

    busy_test_cleanup();
}

// An answer to a request pending on the sender still goes through the receiver (ciot_busy_task forwards it)
void test_ciot_busy_answer_to_pending_request_takes_receiver(void)
{
    busy_test_setup();

    g_dev_iface.req_status.state = CIOT_IFACE_REQ_STATE_SENDED;

    ciot_msg_t msg = device_data_msg(10);
    ciot_event_t event = raw_event(&msg);
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, ciot_iface_send_event(&g_dev_iface, &event));

    TEST_ASSERT_EQUAL(CIOT_STATE_BUSY, g_ciot->status.state);
    TEST_ASSERT_EQUAL(0, g_app_calls);

    busy_test_cleanup();
}
