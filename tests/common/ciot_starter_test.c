#include <stdlib.h>
#include <string.h>

#include "unity.h"
#include "ciot.h"
#include "ciot_iface.h"

// ciot_start brings the ifaces up one by one and waits in receiver.event for the ack of each.
// An iface that already started can still send events on its own (e.g. the nRF USB during the
// enumeration with the host): they must not replace the ack of the iface being started, or the
// start waits CIOT_CONFIG_START_TIMEOUT_SECS for an ack that already came.

static ciot_t g_ciot;
static ciot_iface_t g_c;   // id 1: started first (like the USB), then sends events on its own
static ciot_iface_t g_a;   // id 2: acks its start from inside process_data (like the BLE)
static ciot_iface_t g_b;   // id 3: acks its start from inside process_data
static ciot_iface_t *g_ifaces[4];
static ciot_msg_data_t g_cfg;
static ciot_msg_data_t *g_cfgs[4];
static bool g_c_sends_during_a;
static int g_app_started_a;
static ciot_event_t g_app_event_a;

static ciot_err_t ack_process_data(ciot_iface_t *iface, ciot_msg_data_t *data)
{
    (void)data;
    return ciot_iface_send_event_type(iface, CIOT_EVENT_TYPE_STARTED);
}

// g_a acks, then g_c (already started) sends an event with no type and a STARTED of its own
static ciot_err_t a_process_data(ciot_iface_t *iface, ciot_msg_data_t *data)
{
    ciot_err_t err = ack_process_data(iface, data);
    if (g_c_sends_during_a)
    {
        ciot_event_t unknown = { 0 };
        unknown.type = CIOT_EVENT_TYPE_UNKNOWN;
        ciot_iface_send_event(&g_c, &unknown);
        ciot_iface_send_event_type(&g_c, CIOT_EVENT_TYPE_STARTED);
    }
    return err;
}

static ciot_err_t nop_get_data(ciot_iface_t *iface, ciot_msg_data_t *data)
{
    (void)iface;
    (void)data;
    return CIOT_ERR_OK;
}

static ciot_err_t nop_send_data(ciot_iface_t *iface, uint8_t *data, int size)
{
    (void)iface;
    (void)data;
    (void)size;
    return CIOT_ERR_OK;
}

static ciot_err_t app_handler(ciot_iface_t *sender, ciot_event_t *event, void *args)
{
    (void)args;
    if (sender == &g_a && event->type == CIOT_EVENT_TYPE_STARTED)
    {
        g_app_started_a++;
        g_app_event_a = *event;
    }
    return CIOT_ERR_OK;
}

static void init_iface(ciot_iface_t *iface, ciot_iface_type_t type, ciot_iface_process_data_fn *process_data)
{
    memset(iface, 0, sizeof(*iface));
    iface->info.type = type;
    iface->process_data = process_data;
    iface->get_data = nop_get_data;
    iface->send_data = nop_send_data;
}

static void starter_setup(bool c_sends_during_a)
{
    g_ciot = ciot_new();
    TEST_ASSERT_NOT_NULL(g_ciot);
    init_iface(&g_c, CIOT_IFACE_TYPE_USB, ack_process_data);
    init_iface(&g_a, CIOT_IFACE_TYPE_BLE, a_process_data);
    init_iface(&g_b, CIOT_IFACE_TYPE_BLE_ADV, ack_process_data);
    g_ifaces[0] = NULL;
    g_ifaces[1] = &g_c;
    g_ifaces[2] = &g_a;
    g_ifaces[3] = &g_b;
    memset(&g_cfg, 0, sizeof(g_cfg));
    g_cfg.which_type = CIOT_MSG_DATA_GET_DATA_TAG;
    g_cfgs[0] = NULL;
    g_cfgs[1] = &g_cfg;
    g_cfgs[2] = &g_cfg;
    g_cfgs[3] = &g_cfg;
    g_c_sends_during_a = c_sends_during_a;
    g_app_started_a = 0;
    memset(&g_app_event_a, 0, sizeof(g_app_event_a));
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, ciot_iface_set_event_handler(&g_ciot->iface, app_handler, NULL));

    ciot_cfg_t cfg = { 0 };
    cfg.ifaces.list = g_ifaces;
    cfg.ifaces.cfgs = g_cfgs;
    cfg.ifaces.count = 4;
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, ciot_start(g_ciot, &cfg));
}

static void starter_cleanup(void)
{
    free(g_ciot);
    g_ciot = NULL;
}

// A few tasks are enough when every ack is kept: the start timeout (seconds) never runs out here.
static void run_tasks(int n)
{
    for (int i = 0; i < n && g_ciot->status.state != CIOT_STATE_STARTED; i++)
    {
        ciot_task(g_ciot);
    }
}

static void assert_a_started_once_with_its_data(void)
{
    TEST_ASSERT_EQUAL(CIOT_STATE_STARTED, g_ciot->status.state);
    TEST_ASSERT_EQUAL(1, g_app_started_a);
    TEST_ASSERT_EQUAL(CIOT_EVENT_MSG_TAG, g_app_event_a.which_data);
    TEST_ASSERT_EQUAL(g_a.info.id, g_app_event_a.msg.iface.id);
    TEST_ASSERT_EQUAL(CIOT_IFACE_TYPE_BLE, g_app_event_a.msg.iface.type);
}

// Reference: with no other event, the synchronous acks start everything in a few tasks.
void test_ciot_starter_sync_acks_start_all()
{
    starter_setup(false);
    run_tasks(10);
    assert_a_started_once_with_its_data();
    starter_cleanup();
}

// An iface that already started sends events while another one starts: the start still goes on
// right away, and the application gets the ack of the iface being started with its data.
void test_ciot_starter_started_iface_events_dont_replace_ack()
{
    starter_setup(true);
    run_tasks(10);
    assert_a_started_once_with_its_data();
    starter_cleanup();
}

void test_ciot_starter()
{
    RUN_TEST(test_ciot_starter_sync_acks_start_all);
    RUN_TEST(test_ciot_starter_started_iface_events_dont_replace_ack);
}
