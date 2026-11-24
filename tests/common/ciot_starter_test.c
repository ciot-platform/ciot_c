#include <stdlib.h>
#include <string.h>

#include "unity.h"
#include "ciot.h"
#include "ciot_iface.h"

// ciot_start brings the ifaces up one by one and waits for the ack of each. Another iface's event
// overwrites receiver.event (e.g. on nRF a USB STARTED right after the BLE STARTED): the ack of the
// iface being started must not be lost with it, or the start waits CIOT_CONFIG_START_TIMEOUT_SECS.

static ciot_t g_ciot;
static ciot_iface_t g_a;   // id 1: acks its start from inside process_data, then g_c sends an event
static ciot_iface_t g_b;   // id 2: acks its start from inside process_data
static ciot_iface_t g_c;   // id 3: not started by the core (no cfg), sends events on its own
static ciot_iface_t *g_ifaces[4];
static ciot_msg_data_t g_cfg;
static ciot_msg_data_t *g_cfgs[4];
static bool g_c_overwrites;
static int g_app_started_a;

static ciot_err_t a_process_data(ciot_iface_t *iface, ciot_msg_data_t *data)
{
    (void)data;
    ciot_err_t err = ciot_iface_send_event_type(iface, CIOT_EVENT_TYPE_STARTED);
    if (g_c_overwrites)
    {
        ciot_iface_send_event_type(&g_c, CIOT_EVENT_TYPE_STARTED);
    }
    return err;
}

static ciot_err_t b_process_data(ciot_iface_t *iface, ciot_msg_data_t *data)
{
    (void)data;
    return ciot_iface_send_event_type(iface, CIOT_EVENT_TYPE_STARTED);
}

static ciot_err_t nop_process_data(ciot_iface_t *iface, ciot_msg_data_t *data)
{
    (void)iface;
    (void)data;
    return CIOT_ERR_OK;
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

static void starter_setup(bool c_overwrites)
{
    g_ciot = ciot_new();
    TEST_ASSERT_NOT_NULL(g_ciot);
    init_iface(&g_a, CIOT_IFACE_TYPE_BLE, a_process_data);
    init_iface(&g_b, CIOT_IFACE_TYPE_BLE_ADV, b_process_data);
    init_iface(&g_c, CIOT_IFACE_TYPE_USB, nop_process_data);
    g_ifaces[0] = NULL;
    g_ifaces[1] = &g_a;
    g_ifaces[2] = &g_b;
    g_ifaces[3] = &g_c;
    memset(&g_cfg, 0, sizeof(g_cfg));
    g_cfg.which_type = CIOT_MSG_DATA_GET_DATA_TAG;
    g_cfgs[0] = NULL;
    g_cfgs[1] = &g_cfg;
    g_cfgs[2] = &g_cfg;
    g_cfgs[3] = NULL;
    g_c_overwrites = c_overwrites;
    g_app_started_a = 0;
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

static void run_tasks(int n)
{
    for (int i = 0; i < n && g_ciot->status.state != CIOT_STATE_STARTED; i++)
    {
        ciot_task(g_ciot);
    }
}

// Reference: without another event, the synchronous acks start everything in a few tasks.
void test_ciot_starter_sync_acks_start_all()
{
    starter_setup(false);
    run_tasks(10);
    TEST_ASSERT_EQUAL(CIOT_STATE_STARTED, g_ciot->status.state);
    TEST_ASSERT_EQUAL(1, g_app_started_a);
    starter_cleanup();
}

// The ack of g_a is overwritten in receiver.event by g_c: the start still goes on right away
// (before the fix it waited CIOT_CONFIG_START_TIMEOUT_SECS), and the application gets g_a STARTED.
void test_ciot_starter_ack_survives_other_event()
{
    starter_setup(true);
    run_tasks(10);
    TEST_ASSERT_EQUAL(CIOT_STATE_STARTED, g_ciot->status.state);
    TEST_ASSERT_EQUAL(1, g_app_started_a);
    starter_cleanup();
}

void test_ciot_starter()
{
    RUN_TEST(test_ciot_starter_sync_acks_start_all);
    RUN_TEST(test_ciot_starter_ack_survives_other_event);
}
