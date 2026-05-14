/**
 * @file ciot_wifi_multi_test.c
 * @author your name (you@domain.com)
 * @brief Unit tests for wifi_multi implementation with mock wifi_sta
 * @version 0.1
 * @date 2026-04-23
 *
 * @copyright Copyright (c) 2026
 *
 */

#include <string.h>
#include <stdlib.h>

#include "unity.h"
#include "ciot_wifi_multi.h"
#include "ciot_wifi.h"
#include "ciot_iface.h"
#include "ciot_err.h"
#include "ciot_tcp.h"

// Create and setup test fixture
static ciot_wifi_multi_t test_multi_wifi = NULL;
static ciot_wifi_base_t mock_wifi_base = {0};
static ciot_iface_t *mock_wifi_iface = (ciot_iface_t*)&mock_wifi_base;
static bool mock_wifi_fail_on_start = false;
static char mock_wifi_fail_ssid[33] = {0};
static uint32_t mock_wifi_start_calls = 0;

static ciot_err_t mock_wifi_start(ciot_wifi_t wifi, ciot_wifi_cfg_t *cfg, void *args)
{
    (void)args;
    CIOT_ERR_NULL_CHECK(wifi);
    CIOT_ERR_NULL_CHECK(cfg);

    ciot_wifi_base_t *base = (ciot_wifi_base_t *)wifi;
    mock_wifi_start_calls++;
    base->cfg = *cfg;

    bool fail_for_ssid = mock_wifi_fail_ssid[0] != '\0' && strcmp(cfg->ssid, mock_wifi_fail_ssid) == 0;

    if (mock_wifi_fail_on_start || cfg->disabled || fail_for_ssid)
    {
        base->status.tcp.state = CIOT_TCP_STATE_STOPPED;
        base->status.disconnect_reason = CIOT_ERR_DISCONNECTION;
        ciot_iface_send_event_type(&base->iface, CIOT_EVENT_TYPE_STOPPED);
    }
    else
    {
        base->status.tcp.state = CIOT_TCP_STATE_CONNECTED;
        base->status.disconnect_reason = CIOT_ERR_OK;
        ciot_iface_send_event_type(&base->iface, CIOT_EVENT_TYPE_STARTED);
    }
    return CIOT_ERR_OK;
}

static ciot_err_t mock_wifi_stop(ciot_wifi_t wifi, void *args)
{
    (void)args;
    CIOT_ERR_NULL_CHECK(wifi);

    ciot_wifi_base_t *base = (ciot_wifi_base_t *)wifi;
    base->status.tcp.state = CIOT_TCP_STATE_STOPPED;
    ciot_iface_send_event_type(&base->iface, CIOT_EVENT_TYPE_STOPPED);
    return CIOT_ERR_OK;
}

static ciot_err_t mock_wifi_get_status(ciot_wifi_t wifi, ciot_wifi_status_t *status, void *args)
{
    (void)args;
    CIOT_ERR_NULL_CHECK(wifi);
    CIOT_ERR_NULL_CHECK(status);

    ciot_wifi_base_t *base = (ciot_wifi_base_t *)wifi;
    *status = base->status;
    return CIOT_ERR_OK;
}

static ciot_err_t mock_wifi_get_info(ciot_wifi_t wifi, ciot_wifi_info_t *info, void *args)
{
    (void)args;
    CIOT_ERR_NULL_CHECK(wifi);
    CIOT_ERR_NULL_CHECK(info);

    ciot_wifi_base_t *base = (ciot_wifi_base_t *)wifi;
    *info = base->info;
    return CIOT_ERR_OK;
}

// Mock iface functions for wifi_multi
static ciot_err_t mock_wifi_multi_process_data(ciot_iface_t *iface, ciot_msg_data_t *data)
{
    return CIOT_ERR_OK;
}

static ciot_err_t mock_wifi_multi_get_data(ciot_iface_t *iface, ciot_msg_data_t *data)
{
    return CIOT_ERR_OK;
}

static ciot_err_t mock_wifi_multi_send_data(ciot_iface_t *iface, uint8_t *data, int size)
{
    return CIOT_ERR_NOT_SUPPORTED;
}

static ciot_err_t start_test_multi_wifi(ciot_wifi_multi_t wifi_multi, ciot_wifi_multi_cfg_t *cfg)
{
    return ciot_wifi_multi_start(wifi_multi, cfg);
}

void setUp(void)
{
    mock_wifi_fail_on_start = false;
    memset(mock_wifi_fail_ssid, 0, sizeof(mock_wifi_fail_ssid));
    mock_wifi_start_calls = 0;
    memset(&mock_wifi_base, 0, sizeof(ciot_wifi_base_t));
    mock_wifi_base.tcp = ciot_tcp_new((ciot_iface_t*)&mock_wifi_base, CIOT_TCP_TYPE_WIFI_STA);
    ciot_wifi_init((ciot_wifi_t)&mock_wifi_base);
    
    // Setup mock wifi iface
    mock_wifi_iface->ptr = &mock_wifi_iface;  // Non-null pointer to act as mock wifi_t
    mock_wifi_iface->info.type = CIOT_IFACE_TYPE_WIFI;
    mock_wifi_iface->process_data = mock_wifi_multi_process_data;
    mock_wifi_iface->get_data = mock_wifi_multi_get_data;
    mock_wifi_iface->send_data = mock_wifi_multi_send_data;
    
    // Create wifi_multi with injected mock wifi
    test_multi_wifi = ciot_wifi_multi_new((ciot_wifi_t)&mock_wifi_base);
    TEST_ASSERT_NOT_NULL(test_multi_wifi);

    ciot_wifi_multi_wifi_ops_t wifi_ops = {
        .start = mock_wifi_start,
        .stop = mock_wifi_stop,
        .get_status = mock_wifi_get_status,
        .get_info = mock_wifi_get_info,
        .args = NULL,
    };
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, ciot_wifi_multi_set_wifi_ops(test_multi_wifi, &wifi_ops));
}

void tearDown(void)
{
    if (test_multi_wifi != NULL) {
        free(test_multi_wifi);
        test_multi_wifi = NULL;
    }
}
void test_ciot_wifi_multi_new_creates_internal_wifi(void)
{
    ciot_wifi_multi_t multi = ciot_wifi_multi_new(NULL);
    TEST_ASSERT_NOT_NULL(multi);
    free(multi);
}

// ============================================================================
// Test: Null pointer checks
// ============================================================================

void test_ciot_wifi_multi_new_with_injected_wifi(void)
{
    ciot_wifi_multi_t multi = ciot_wifi_multi_new((ciot_wifi_t)mock_wifi_iface);
    TEST_ASSERT_NOT_NULL(multi);
    free(multi);
}

void test_ciot_wifi_multi_init_null_check(void)
{
    ciot_err_t err = ciot_wifi_multi_init(NULL);
    TEST_ASSERT_EQUAL(CIOT_ERR_NULL_ARG, err);
}

void test_ciot_wifi_multi_start_null_check(void)
{
    ciot_err_t err = ciot_wifi_multi_start(NULL, NULL);
    TEST_ASSERT_EQUAL(CIOT_ERR_NULL_ARG, err);
}

void test_ciot_wifi_multi_stop_null_check(void)
{
    ciot_err_t err = ciot_wifi_multi_stop(NULL);
    TEST_ASSERT_EQUAL(CIOT_ERR_NULL_ARG, err);
}

void test_ciot_wifi_multi_task_in_idle_does_nothing(void)
{
    ciot_err_t err = ciot_wifi_multi_task(test_multi_wifi);
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, err);

    ciot_wifi_multi_status_t status = {0};
    err = ciot_wifi_multi_get_status(test_multi_wifi, &status);
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, err);
    TEST_ASSERT_EQUAL(CIOT_WIFI_MULTI_ACTIVE_INDEX_NONE, status.active_index);
    TEST_ASSERT_EQUAL(0, status.next_switch_s);
    TEST_ASSERT_FALSE(status.started);
}

void test_ciot_wifi_multi_start_requires_preconfigured_items(void)
{
    ciot_wifi_multi_cfg_t cfg = {0};
    cfg.items_count = 1;
    cfg.initial_index = 0;

    ciot_err_t err = ciot_wifi_multi_start(test_multi_wifi, &cfg);
    TEST_ASSERT_EQUAL(CIOT_ERR_INVALID_STATE, err);
}

// ============================================================================
// Test: Configuration and basic operations
// ============================================================================

void test_ciot_wifi_multi_start_with_single_network(void)
{
    ciot_wifi_multi_cfg_t cfg = {0};
    cfg.items_count = 1;
    strncpy((char*)cfg.items[0].ssid, "TestSSID", sizeof(cfg.items[0].ssid) - 1);
    strncpy((char*)cfg.items[0].password, "TestPass", sizeof(cfg.items[0].password) - 1);
    cfg.items[0].valid = true;
    cfg.initial_index = 0;
    
    ciot_err_t err = start_test_multi_wifi(test_multi_wifi, &cfg);
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, err);
    
    ciot_wifi_multi_status_t status = {0};
    err = ciot_wifi_multi_get_status(test_multi_wifi, &status);
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, err);
    TEST_ASSERT_EQUAL(1, status.items_count);
    TEST_ASSERT_EQUAL(0, status.active_index);
    TEST_ASSERT_EQUAL(1, status.valid_count);
    TEST_ASSERT_TRUE(status.started);
}

void test_ciot_wifi_multi_start_keeps_current_connection_when_ssid_matches(void)
{
    // Simulate an already connected Wi-Fi interface using the same target SSID.
    strncpy((char*)mock_wifi_base.cfg.ssid, "SSID1", sizeof(mock_wifi_base.cfg.ssid) - 1);
    mock_wifi_base.status.tcp.state = CIOT_TCP_STATE_CONNECTED;

    ciot_wifi_multi_cfg_t cfg = {0};
    cfg.items_count = 1;
    strncpy((char*)cfg.items[0].ssid, "SSID1", sizeof(cfg.items[0].ssid) - 1);
    strncpy((char*)cfg.items[0].password, "Pass1", sizeof(cfg.items[0].password) - 1);
    cfg.items[0].valid = true;
    cfg.initial_index = 0;

    ciot_err_t err = start_test_multi_wifi(test_multi_wifi, &cfg);
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, err);
    TEST_ASSERT_EQUAL(0, mock_wifi_start_calls);

    ciot_wifi_multi_status_t status = {0};
    err = ciot_wifi_multi_get_status(test_multi_wifi, &status);
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, err);
    TEST_ASSERT_TRUE(status.started);
    TEST_ASSERT_EQUAL(0, status.active_index);
}

void test_ciot_wifi_multi_start_with_two_networks(void)
{
    ciot_wifi_multi_cfg_t cfg = {0};
    cfg.items_count = 2;
    strncpy((char*)cfg.items[0].ssid, "SSID1", sizeof(cfg.items[0].ssid) - 1);
    strncpy((char*)cfg.items[0].password, "Pass1", sizeof(cfg.items[0].password) - 1);
    cfg.items[0].valid = true;
    strncpy((char*)cfg.items[1].ssid, "SSID2", sizeof(cfg.items[1].ssid) - 1);
    strncpy((char*)cfg.items[1].password, "Pass2", sizeof(cfg.items[1].password) - 1);
    cfg.items[1].valid = true;
    cfg.initial_index = 0;
    
    ciot_err_t err = start_test_multi_wifi(test_multi_wifi, &cfg);
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, err);
    
    ciot_wifi_multi_status_t status = {0};
    err = ciot_wifi_multi_get_status(test_multi_wifi, &status);
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, err);
    TEST_ASSERT_EQUAL(2, status.items_count);
    TEST_ASSERT_EQUAL(0, status.active_index);
    TEST_ASSERT_EQUAL(2, status.valid_count);
}

void test_ciot_wifi_multi_start_with_invalid_initial_index(void)
{
    ciot_wifi_multi_cfg_t cfg = {0};
    cfg.items_count = 2;
    strncpy((char*)cfg.items[0].ssid, "SSID1", sizeof(cfg.items[0].ssid) - 1);
    strncpy((char*)cfg.items[0].password, "Pass1", sizeof(cfg.items[0].password) - 1);
    cfg.items[0].valid = true;
    strncpy((char*)cfg.items[1].ssid, "SSID2", sizeof(cfg.items[1].ssid) - 1);
    strncpy((char*)cfg.items[1].password, "Pass2", sizeof(cfg.items[1].password) - 1);
    cfg.items[1].valid = true;
    cfg.initial_index = 1;  // Valid, should use it
    
    ciot_err_t err = start_test_multi_wifi(test_multi_wifi, &cfg);
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, err);
    
    ciot_wifi_multi_status_t status = {0};
    err = ciot_wifi_multi_get_status(test_multi_wifi, &status);
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, err);
    TEST_ASSERT_EQUAL(1, status.active_index);
}

void test_ciot_wifi_multi_stop(void)
{
    ciot_wifi_multi_cfg_t cfg = {0};
    cfg.items_count = 1;
    strncpy((char*)cfg.items[0].ssid, "TestSSID", sizeof(cfg.items[0].ssid) - 1);
    strncpy((char*)cfg.items[0].password, "TestPass", sizeof(cfg.items[0].password) - 1);
    cfg.items[0].valid = true;
    cfg.initial_index = 0;
    
    start_test_multi_wifi(test_multi_wifi, &cfg);
    
    ciot_err_t err = ciot_wifi_multi_stop(test_multi_wifi);
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, err);
    
    ciot_wifi_multi_status_t status = {0};
    err = ciot_wifi_multi_get_status(test_multi_wifi, &status);
    TEST_ASSERT_EQUAL(CIOT_WIFI_MULTI_ACTIVE_INDEX_NONE, status.active_index);
}

// ============================================================================
// Test: Network selection and rotation
// ============================================================================

void test_ciot_wifi_multi_next_rotates_to_next_network(void)
{
    ciot_wifi_multi_cfg_t cfg = {0};
    cfg.items_count = 2;
    strncpy((char*)cfg.items[0].ssid, "SSID1", sizeof(cfg.items[0].ssid) - 1);
    strncpy((char*)cfg.items[0].password, "Pass1", sizeof(cfg.items[0].password) - 1);
    cfg.items[0].valid = true;
    strncpy((char*)cfg.items[1].ssid, "SSID2", sizeof(cfg.items[1].ssid) - 1);
    strncpy((char*)cfg.items[1].password, "Pass2", sizeof(cfg.items[1].password) - 1);
    cfg.items[1].valid = true;
    cfg.initial_index = 0;
    
    start_test_multi_wifi(test_multi_wifi, &cfg);
    
    // Call next to rotate to network 1
    ciot_err_t err = ciot_wifi_multi_next(test_multi_wifi);
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, err);
    
    ciot_wifi_multi_status_t status = {0};
    ciot_wifi_multi_get_status(test_multi_wifi, &status);
    TEST_ASSERT_EQUAL(1, status.active_index);
}

void test_ciot_wifi_multi_next_wraps_around(void)
{
    ciot_wifi_multi_cfg_t cfg = {0};
    cfg.items_count = 2;
    strncpy((char*)cfg.items[0].ssid, "SSID1", sizeof(cfg.items[0].ssid) - 1);
    strncpy((char*)cfg.items[0].password, "Pass1", sizeof(cfg.items[0].password) - 1);
    cfg.items[0].valid = true;
    strncpy((char*)cfg.items[1].ssid, "SSID2", sizeof(cfg.items[1].ssid) - 1);
    strncpy((char*)cfg.items[1].password, "Pass2", sizeof(cfg.items[1].password) - 1);
    cfg.items[1].valid = true;
    cfg.initial_index = 1;  // Start at last item
    
    start_test_multi_wifi(test_multi_wifi, &cfg);
    
    ciot_wifi_multi_status_t status = {0};
    ciot_wifi_multi_get_status(test_multi_wifi, &status);
    TEST_ASSERT_EQUAL(1, status.active_index);
    
    // Call next to wrap around to network 0
    ciot_err_t err = ciot_wifi_multi_next(test_multi_wifi);
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, err);
    
    ciot_wifi_multi_get_status(test_multi_wifi, &status);
    TEST_ASSERT_EQUAL(0, status.active_index);
}

void test_ciot_wifi_multi_next_skips_invalid_networks(void)
{
    ciot_wifi_multi_cfg_t cfg = {0};
    cfg.items_count = 2;
    strncpy((char*)cfg.items[0].ssid, "SSID1", sizeof(cfg.items[0].ssid) - 1);
    strncpy((char*)cfg.items[0].password, "Pass1", sizeof(cfg.items[0].password) - 1);
    cfg.items[0].valid = true;
    strncpy((char*)cfg.items[1].ssid, "SSID2", sizeof(cfg.items[1].ssid) - 1);
    strncpy((char*)cfg.items[1].password, "Pass2", sizeof(cfg.items[1].password) - 1);
    cfg.items[1].valid = false;  // Mark as invalid
    cfg.initial_index = 0;
    
    start_test_multi_wifi(test_multi_wifi, &cfg);
    
    ciot_wifi_multi_status_t status = {0};
    ciot_wifi_multi_get_status(test_multi_wifi, &status);
    TEST_ASSERT_EQUAL(0, status.active_index);
    TEST_ASSERT_EQUAL(1, status.valid_count);
    
    // Call next - should stay on network 0 (only valid option)
    ciot_err_t err = ciot_wifi_multi_next(test_multi_wifi);
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, err);
    
    ciot_wifi_multi_get_status(test_multi_wifi, &status);
    TEST_ASSERT_EQUAL(0, status.active_index);
}

void test_ciot_wifi_multi_start_schedules_next_switch_when_enabled(void)
{
    ciot_wifi_multi_cfg_t cfg = {0};
    cfg.items_count = 2;
    cfg.switch_interval_s = 10;
    strncpy((char*)cfg.items[0].ssid, "SSID1", sizeof(cfg.items[0].ssid) - 1);
    strncpy((char*)cfg.items[0].password, "Pass1", sizeof(cfg.items[0].password) - 1);
    cfg.items[0].valid = true;
    strncpy((char*)cfg.items[1].ssid, "SSID2", sizeof(cfg.items[1].ssid) - 1);
    strncpy((char*)cfg.items[1].password, "Pass2", sizeof(cfg.items[1].password) - 1);
    cfg.items[1].valid = true;
    cfg.initial_index = 0;

    ciot_err_t err = start_test_multi_wifi(test_multi_wifi, &cfg);
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, err);

    ciot_wifi_multi_status_t status = {0};
    ciot_wifi_multi_get_status(test_multi_wifi, &status);
    TEST_ASSERT_NOT_EQUAL(0, status.next_switch_s);
    TEST_ASSERT_TRUE(status.next_switch_s >= 10);
}

void test_ciot_wifi_multi_task_rotates_when_switch_is_due(void)
{
    ciot_wifi_multi_cfg_t cfg = {0};
    cfg.items_count = 2;
    cfg.switch_interval_s = 10;
    strncpy((char*)cfg.items[0].ssid, "SSID1", sizeof(cfg.items[0].ssid) - 1);
    strncpy((char*)cfg.items[0].password, "Pass1", sizeof(cfg.items[0].password) - 1);
    cfg.items[0].valid = true;
    strncpy((char*)cfg.items[1].ssid, "SSID2", sizeof(cfg.items[1].ssid) - 1);
    strncpy((char*)cfg.items[1].password, "Pass2", sizeof(cfg.items[1].password) - 1);
    cfg.items[1].valid = true;
    cfg.initial_index = 0;

    ciot_err_t err = start_test_multi_wifi(test_multi_wifi, &cfg);
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, err);

    ciot_wifi_multi_base_t *base = (ciot_wifi_multi_base_t *)test_multi_wifi;
    base->status.next_switch_s = 1;

    err = ciot_wifi_multi_task(test_multi_wifi);
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, err);

    ciot_wifi_multi_status_t status = {0};
    ciot_wifi_multi_get_status(test_multi_wifi, &status);
    TEST_ASSERT_EQUAL(1, status.active_index);
    TEST_ASSERT_NOT_EQUAL(0, status.next_switch_s);
}

void test_ciot_wifi_multi_task_rotates_and_returns_to_first_network(void)
{
    ciot_wifi_multi_cfg_t cfg = {0};
    cfg.items_count = 2;
    cfg.switch_interval_s = 10;
    strncpy((char*)cfg.items[0].ssid, "SSID1", sizeof(cfg.items[0].ssid) - 1);
    strncpy((char*)cfg.items[0].password, "Pass1", sizeof(cfg.items[0].password) - 1);
    cfg.items[0].valid = true;
    strncpy((char*)cfg.items[1].ssid, "SSID2", sizeof(cfg.items[1].ssid) - 1);
    strncpy((char*)cfg.items[1].password, "Pass2", sizeof(cfg.items[1].password) - 1);
    cfg.items[1].valid = true;
    cfg.initial_index = 0;

    ciot_err_t err = start_test_multi_wifi(test_multi_wifi, &cfg);
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, err);

    ciot_wifi_multi_base_t *base = (ciot_wifi_multi_base_t *)test_multi_wifi;
    ciot_wifi_multi_status_t status = {0};

    // Verify initial state: network 0 is active
    ciot_wifi_multi_get_status(test_multi_wifi, &status);
    TEST_ASSERT_EQUAL(0, status.active_index);

    // First task: rotate from network 0 to network 1
    base->status.next_switch_s = 1;
    err = ciot_wifi_multi_task(test_multi_wifi);
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, err);
    ciot_wifi_multi_get_status(test_multi_wifi, &status);
    TEST_ASSERT_EQUAL(1, status.active_index);
    TEST_ASSERT_EQUAL(true, status.items[1].selected);

    // Second task: rotate back from network 1 to network 0
    base->status.next_switch_s = 1;
    err = ciot_wifi_multi_task(test_multi_wifi);
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, err);
    ciot_wifi_multi_get_status(test_multi_wifi, &status);
    TEST_ASSERT_EQUAL(0, status.active_index);
    TEST_ASSERT_EQUAL(true, status.items[0].selected);

    // Third task: rotate from network 0 to network 1 again
    base->status.next_switch_s = 1;
    err = ciot_wifi_multi_task(test_multi_wifi);
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, err);
    ciot_wifi_multi_get_status(test_multi_wifi, &status);
    TEST_ASSERT_EQUAL(1, status.active_index);
    TEST_ASSERT_EQUAL(true, status.items[1].selected);
}

void test_ciot_wifi_multi_task_retries_invalid_networks_in_rotation(void)
{
    ciot_wifi_multi_cfg_t cfg = {0};
    cfg.items_count = 2;
    cfg.switch_interval_s = 10;
    strncpy((char*)cfg.items[0].ssid, "SSID1", sizeof(cfg.items[0].ssid) - 1);
    strncpy((char*)cfg.items[0].password, "Pass1", sizeof(cfg.items[0].password) - 1);
    cfg.items[0].valid = true;
    strncpy((char*)cfg.items[1].ssid, "SSID2", sizeof(cfg.items[1].ssid) - 1);
    strncpy((char*)cfg.items[1].password, "Pass2", sizeof(cfg.items[1].password) - 1);
    cfg.items[1].valid = true;
    cfg.initial_index = 0;

    ciot_err_t err = start_test_multi_wifi(test_multi_wifi, &cfg);
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, err);

    // Invalidate network 1 while network 0 remains active.
    err = ciot_wifi_multi_mark_invalid(test_multi_wifi, 1, CIOT_ERR_DISCONNECTION);
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, err);

    ciot_wifi_multi_status_t status = {0};
    ciot_wifi_multi_get_status(test_multi_wifi, &status);
    TEST_ASSERT_EQUAL(0, status.active_index);
    TEST_ASSERT_FALSE(status.items[1].valid);

    ciot_wifi_multi_base_t *base = (ciot_wifi_multi_base_t *)test_multi_wifi;
    base->status.next_switch_s = 1;

    // Task rotation must include invalid network again to retest it.
    err = ciot_wifi_multi_task(test_multi_wifi);
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, err);

    ciot_wifi_multi_get_status(test_multi_wifi, &status);
    TEST_ASSERT_EQUAL(1, status.active_index);
    TEST_ASSERT_TRUE(status.items[1].valid);
}

void test_ciot_wifi_multi_task_returns_to_previous_network_when_switch_fails(void)
{
    ciot_wifi_multi_cfg_t cfg = {0};
    cfg.items_count = 2;
    cfg.switch_interval_s = 10;
    strncpy((char*)cfg.items[0].ssid, "SSID1", sizeof(cfg.items[0].ssid) - 1);
    strncpy((char*)cfg.items[0].password, "Pass1", sizeof(cfg.items[0].password) - 1);
    cfg.items[0].valid = true;
    strncpy((char*)cfg.items[1].ssid, "SSID2", sizeof(cfg.items[1].ssid) - 1);
    strncpy((char*)cfg.items[1].password, "Pass2", sizeof(cfg.items[1].password) - 1);
    cfg.items[1].valid = true;
    cfg.initial_index = 0;

    ciot_err_t err = start_test_multi_wifi(test_multi_wifi, &cfg);
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, err);

    ciot_wifi_multi_status_t status = {0};
    ciot_wifi_multi_get_status(test_multi_wifi, &status);
    TEST_ASSERT_EQUAL(0, status.active_index);

    // Force rotation attempt and fail only when trying to switch to SSID2.
    strncpy(mock_wifi_fail_ssid, "SSID2", sizeof(mock_wifi_fail_ssid) - 1);
    ciot_wifi_multi_base_t *base = (ciot_wifi_multi_base_t *)test_multi_wifi;
    base->status.next_switch_s = 1;

    err = ciot_wifi_multi_task(test_multi_wifi);
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, err);

    // Failed switch to SSID2 must rollback to previous SSID1.
    ciot_wifi_multi_get_status(test_multi_wifi, &status);
    TEST_ASSERT_EQUAL(0, status.active_index);
    TEST_ASSERT_TRUE(status.items[0].valid);
    TEST_ASSERT_FALSE(status.items[1].valid);
}

void test_ciot_wifi_multi_task_does_nothing_when_schedule_disabled(void)
{
    ciot_wifi_multi_cfg_t cfg = {0};
    cfg.items_count = 2;
    strncpy((char*)cfg.items[0].ssid, "SSID1", sizeof(cfg.items[0].ssid) - 1);
    strncpy((char*)cfg.items[0].password, "Pass1", sizeof(cfg.items[0].password) - 1);
    cfg.items[0].valid = true;
    strncpy((char*)cfg.items[1].ssid, "SSID2", sizeof(cfg.items[1].ssid) - 1);
    strncpy((char*)cfg.items[1].password, "Pass2", sizeof(cfg.items[1].password) - 1);
    cfg.items[1].valid = true;
    cfg.initial_index = 0;

    ciot_err_t err = start_test_multi_wifi(test_multi_wifi, &cfg);
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, err);

    err = ciot_wifi_multi_task(test_multi_wifi);
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, err);

    ciot_wifi_multi_status_t status = {0};
    ciot_wifi_multi_get_status(test_multi_wifi, &status);
    TEST_ASSERT_EQUAL(0, status.active_index);
    TEST_ASSERT_EQUAL(0, status.next_switch_s);
}

// ============================================================================
// Test: Failover logic - Mark invalid
// ============================================================================

void test_ciot_wifi_multi_mark_invalid_by_index(void)
{
    ciot_wifi_multi_cfg_t cfg = {0};
    cfg.items_count = 2;
    strncpy((char*)cfg.items[0].ssid, "SSID1", sizeof(cfg.items[0].ssid) - 1);
    strncpy((char*)cfg.items[0].password, "Pass1", sizeof(cfg.items[0].password) - 1);
    cfg.items[0].valid = true;
    strncpy((char*)cfg.items[1].ssid, "SSID2", sizeof(cfg.items[1].ssid) - 1);
    strncpy((char*)cfg.items[1].password, "Pass2", sizeof(cfg.items[1].password) - 1);
    cfg.items[1].valid = true;
    cfg.initial_index = 0;
    
    start_test_multi_wifi(test_multi_wifi, &cfg);
    
    // Mark network 0 as invalid
    ciot_err_t err = ciot_wifi_multi_mark_invalid(test_multi_wifi, 0, CIOT_ERR_DISCONNECTION);
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, err);
    
    ciot_wifi_multi_status_t status = {0};
    ciot_wifi_multi_get_status(test_multi_wifi, &status);
    TEST_ASSERT_FALSE(status.items[0].valid);
    // Should auto-switch to network 1
    TEST_ASSERT_EQUAL(1, status.active_index);
    TEST_ASSERT_TRUE(status.items[1].valid);
}

void test_ciot_wifi_multi_mark_active_invalid_triggers_failover(void)
{
    ciot_wifi_multi_cfg_t cfg = {0};
    cfg.items_count = 2;
    strncpy((char*)cfg.items[0].ssid, "SSID1", sizeof(cfg.items[0].ssid) - 1);
    strncpy((char*)cfg.items[0].password, "Pass1", sizeof(cfg.items[0].password) - 1);
    cfg.items[0].valid = true;
    strncpy((char*)cfg.items[1].ssid, "SSID2", sizeof(cfg.items[1].ssid) - 1);
    strncpy((char*)cfg.items[1].password, "Pass2", sizeof(cfg.items[1].password) - 1);
    cfg.items[1].valid = true;
    cfg.initial_index = 0;
    
    start_test_multi_wifi(test_multi_wifi, &cfg);
    
    ciot_wifi_multi_status_t status = {0};
    ciot_wifi_multi_get_status(test_multi_wifi, &status);
    TEST_ASSERT_EQUAL(0, status.active_index);
    
    // Mark active network as invalid - should trigger failover
    ciot_err_t err = ciot_wifi_multi_mark_active_invalid(test_multi_wifi, CIOT_ERR_DISCONNECTION);
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, err);
    
    ciot_wifi_multi_get_status(test_multi_wifi, &status);
    TEST_ASSERT_FALSE(status.items[0].valid);
    TEST_ASSERT_EQUAL(1, status.active_index);  // Should switch to network 1
    TEST_ASSERT_TRUE(status.items[1].valid);
}

void test_ciot_wifi_multi_mark_invalid_no_valid_networks_left(void)
{
    ciot_wifi_multi_cfg_t cfg = {0};
    cfg.items_count = 2;
    strncpy((char*)cfg.items[0].ssid, "SSID1", sizeof(cfg.items[0].ssid) - 1);
    strncpy((char*)cfg.items[0].password, "Pass1", sizeof(cfg.items[0].password) - 1);
    cfg.items[0].valid = true;
    strncpy((char*)cfg.items[1].ssid, "SSID2", sizeof(cfg.items[1].ssid) - 1);
    strncpy((char*)cfg.items[1].password, "Pass2", sizeof(cfg.items[1].password) - 1);
    cfg.items[1].valid = true;
    cfg.initial_index = 0;
    
    start_test_multi_wifi(test_multi_wifi, &cfg);
    
    // Mark network 0 as invalid (should switch to 1)
    ciot_wifi_multi_mark_invalid(test_multi_wifi, 0, CIOT_ERR_DISCONNECTION);
    
    ciot_wifi_multi_status_t status = {0};
    ciot_wifi_multi_get_status(test_multi_wifi, &status);
    TEST_ASSERT_EQUAL(1, status.active_index);
    
    // Mark network 1 as invalid (should have NO_VALID_NETWORK event)
    ciot_err_t err = ciot_wifi_multi_mark_invalid(test_multi_wifi, 1, CIOT_ERR_DISCONNECTION);
    TEST_ASSERT_EQUAL(CIOT_ERR_NOT_FOUND, err);
    
    ciot_wifi_multi_get_status(test_multi_wifi, &status);
    TEST_ASSERT_EQUAL(CIOT_WIFI_MULTI_ACTIVE_INDEX_NONE, status.active_index);
    TEST_ASSERT_EQUAL(0, status.valid_count);
}

// ============================================================================
// Test: Recovery - Reset invalid networks
// ============================================================================

void test_ciot_wifi_multi_reset_invalid_recovers_networks(void)
{
    ciot_wifi_multi_cfg_t cfg = {0};
    cfg.items_count = 2;
    strncpy((char*)cfg.items[0].ssid, "SSID1", sizeof(cfg.items[0].ssid) - 1);
    strncpy((char*)cfg.items[0].password, "Pass1", sizeof(cfg.items[0].password) - 1);
    cfg.items[0].valid = true;
    strncpy((char*)cfg.items[1].ssid, "SSID2", sizeof(cfg.items[1].ssid) - 1);
    strncpy((char*)cfg.items[1].password, "Pass2", sizeof(cfg.items[1].password) - 1);
    cfg.items[1].valid = true;
    cfg.initial_index = 0;
    
    start_test_multi_wifi(test_multi_wifi, &cfg);
    
    // Mark network 0 as invalid
    ciot_wifi_multi_mark_invalid(test_multi_wifi, 0, CIOT_ERR_DISCONNECTION);
    
    ciot_wifi_multi_status_t status = {0};
    ciot_wifi_multi_get_status(test_multi_wifi, &status);
    TEST_ASSERT_FALSE(status.items[0].valid);
    TEST_ASSERT_EQUAL(1, status.valid_count);
    
    // Reset invalid networks
    ciot_err_t err = ciot_wifi_multi_reset_invalid(test_multi_wifi);
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, err);
    
    ciot_wifi_multi_get_status(test_multi_wifi, &status);
    TEST_ASSERT_TRUE(status.items[0].valid);
    TEST_ASSERT_TRUE(status.items[1].valid);
    TEST_ASSERT_EQUAL(2, status.valid_count);
}

void test_ciot_wifi_multi_reset_invalid_when_no_active_network(void)
{
    ciot_wifi_multi_cfg_t cfg = {0};
    cfg.items_count = 2;
    strncpy((char*)cfg.items[0].ssid, "SSID1", sizeof(cfg.items[0].ssid) - 1);
    strncpy((char*)cfg.items[0].password, "Pass1", sizeof(cfg.items[0].password) - 1);
    cfg.items[0].valid = true;
    strncpy((char*)cfg.items[1].ssid, "SSID2", sizeof(cfg.items[1].ssid) - 1);
    strncpy((char*)cfg.items[1].password, "Pass2", sizeof(cfg.items[1].password) - 1);
    cfg.items[1].valid = true;
    cfg.initial_index = 0;
    
    start_test_multi_wifi(test_multi_wifi, &cfg);
    
    // Mark both as invalid
    ciot_wifi_multi_mark_invalid(test_multi_wifi, 0, CIOT_ERR_DISCONNECTION);
    ciot_wifi_multi_mark_invalid(test_multi_wifi, 1, CIOT_ERR_DISCONNECTION);
    
    ciot_wifi_multi_status_t status = {0};
    ciot_wifi_multi_get_status(test_multi_wifi, &status);
    TEST_ASSERT_EQUAL(CIOT_WIFI_MULTI_ACTIVE_INDEX_NONE, status.active_index);
    TEST_ASSERT_EQUAL(0, status.valid_count);
    
    // Reset invalid - should recover and select first valid
    ciot_err_t err = ciot_wifi_multi_reset_invalid(test_multi_wifi);
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, err);
    
    ciot_wifi_multi_get_status(test_multi_wifi, &status);
    TEST_ASSERT_EQUAL(2, status.valid_count);
    TEST_ASSERT_EQUAL(0, status.active_index);  // Should select first valid (network 0)
}

// ============================================================================
// Test: Request processing
// ============================================================================

void test_ciot_wifi_multi_process_req_next(void)
{
    ciot_wifi_multi_cfg_t cfg = {0};
    cfg.items_count = 2;
    strncpy((char*)cfg.items[0].ssid, "SSID1", sizeof(cfg.items[0].ssid) - 1);
    strncpy((char*)cfg.items[0].password, "Pass1", sizeof(cfg.items[0].password) - 1);
    cfg.items[0].valid = true;
    strncpy((char*)cfg.items[1].ssid, "SSID2", sizeof(cfg.items[1].ssid) - 1);
    strncpy((char*)cfg.items[1].password, "Pass2", sizeof(cfg.items[1].password) - 1);
    cfg.items[1].valid = true;
    cfg.initial_index = 0;
    
    start_test_multi_wifi(test_multi_wifi, &cfg);
    
    ciot_wifi_multi_req_t req = {0};
    req.which_type = CIOT_WIFI_MULTI_REQ_NEXT_TAG;
    
    ciot_err_t err = ciot_wifi_multi_process_req(test_multi_wifi, &req);
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, err);
    
    ciot_wifi_multi_status_t status = {0};
    ciot_wifi_multi_get_status(test_multi_wifi, &status);
    TEST_ASSERT_EQUAL(1, status.active_index);
}

void test_ciot_wifi_multi_process_req_set_invalid(void)
{
    ciot_wifi_multi_cfg_t cfg = {0};
    cfg.items_count = 2;
    strncpy((char*)cfg.items[0].ssid, "SSID1", sizeof(cfg.items[0].ssid) - 1);
    strncpy((char*)cfg.items[0].password, "Pass1", sizeof(cfg.items[0].password) - 1);
    cfg.items[0].valid = true;
    strncpy((char*)cfg.items[1].ssid, "SSID2", sizeof(cfg.items[1].ssid) - 1);
    strncpy((char*)cfg.items[1].password, "Pass2", sizeof(cfg.items[1].password) - 1);
    cfg.items[1].valid = true;
    cfg.initial_index = 0;
    
    start_test_multi_wifi(test_multi_wifi, &cfg);
    
    ciot_wifi_multi_req_t req = {0};
    req.which_type = CIOT_WIFI_MULTI_REQ_SET_INVALID_TAG;
    req.set_invalid.index = 0;
    req.set_invalid.reason = CIOT_ERR_DISCONNECTION;
    
    ciot_err_t err = ciot_wifi_multi_process_req(test_multi_wifi, &req);
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, err);
    
    ciot_wifi_multi_status_t status = {0};
    ciot_wifi_multi_get_status(test_multi_wifi, &status);
    TEST_ASSERT_FALSE(status.items[0].valid);
    TEST_ASSERT_EQUAL(1, status.active_index);
}

void test_ciot_wifi_multi_process_req_reset_invalid(void)
{
    ciot_wifi_multi_cfg_t cfg = {0};
    cfg.items_count = 2;
    strncpy((char*)cfg.items[0].ssid, "SSID1", sizeof(cfg.items[0].ssid) - 1);
    strncpy((char*)cfg.items[0].password, "Pass1", sizeof(cfg.items[0].password) - 1);
    cfg.items[0].valid = true;
    strncpy((char*)cfg.items[1].ssid, "SSID2", sizeof(cfg.items[1].ssid) - 1);
    strncpy((char*)cfg.items[1].password, "Pass2", sizeof(cfg.items[1].password) - 1);
    cfg.items[1].valid = true;
    cfg.initial_index = 0;
    
    start_test_multi_wifi(test_multi_wifi, &cfg);
    
    // Invalidate network 0
    ciot_wifi_multi_req_t req = {0};
    req.which_type = CIOT_WIFI_MULTI_REQ_SET_INVALID_TAG;
    req.set_invalid.index = 0;
    req.set_invalid.reason = CIOT_ERR_DISCONNECTION;
    ciot_wifi_multi_process_req(test_multi_wifi, &req);
    
    // Reset invalid
    req.which_type = CIOT_WIFI_MULTI_REQ_RESET_INVALID_TAG;
    ciot_err_t err = ciot_wifi_multi_process_req(test_multi_wifi, &req);
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, err);
    
    ciot_wifi_multi_status_t status = {0};
    ciot_wifi_multi_get_status(test_multi_wifi, &status);
    TEST_ASSERT_TRUE(status.items[0].valid);
    TEST_ASSERT_TRUE(status.items[1].valid);
}

void test_ciot_wifi_multi_process_req_set_item_updates_ram_only(void)
{
    ciot_wifi_multi_req_t req = {0};
    req.which_type = CIOT_WIFI_MULTI_REQ_SET_ITEM_TAG;
    req.set_item.index = 0;
    req.set_item.has_config = true;
    strncpy((char*)req.set_item.config.ssid, "IdleSSID", sizeof(req.set_item.config.ssid) - 1);
    strncpy((char*)req.set_item.config.password, "IdlePass", sizeof(req.set_item.config.password) - 1);
    req.set_item.config.valid = true;

    ciot_err_t err = ciot_wifi_multi_process_req(test_multi_wifi, &req);
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, err);

    ciot_wifi_multi_cfg_t cfg = {0};
    err = ciot_wifi_multi_get_cfg(test_multi_wifi, &cfg);
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, err);
    TEST_ASSERT_EQUAL(0, strncmp((char*)cfg.items[0].ssid, "IdleSSID", 8));
    TEST_ASSERT_EQUAL(0, strncmp((char*)cfg.items[0].password, "IdlePass", 8));

    ciot_wifi_multi_status_t status = {0};
    err = ciot_wifi_multi_get_status(test_multi_wifi, &status);
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, err);
    TEST_ASSERT_EQUAL(CIOT_WIFI_MULTI_ACTIVE_INDEX_NONE, status.active_index);
}

void test_ciot_wifi_multi_process_req_set_item_requires_config(void)
{
    ciot_wifi_multi_req_t req = {0};
    req.which_type = CIOT_WIFI_MULTI_REQ_SET_ITEM_TAG;
    req.set_item.index = 0;
    req.set_item.has_config = false;

    ciot_err_t err = ciot_wifi_multi_process_req(test_multi_wifi, &req);
    TEST_ASSERT_EQUAL(CIOT_ERR_INVALID_ARG, err);
}

// ============================================================================
// Test: Configuration retrieval
// ============================================================================

void test_ciot_wifi_multi_get_cfg(void)
{
    ciot_wifi_multi_cfg_t cfg = {0};
    cfg.items_count = 2;
    strncpy((char*)cfg.items[0].ssid, "SSID1", sizeof(cfg.items[0].ssid) - 1);
    strncpy((char*)cfg.items[0].password, "Pass1", sizeof(cfg.items[0].password) - 1);
    cfg.items[0].valid = true;
    strncpy((char*)cfg.items[1].ssid, "SSID2", sizeof(cfg.items[1].ssid) - 1);
    strncpy((char*)cfg.items[1].password, "Pass2", sizeof(cfg.items[1].password) - 1);
    cfg.items[1].valid = true;
    cfg.initial_index = 0;
    
    start_test_multi_wifi(test_multi_wifi, &cfg);
    
    ciot_wifi_multi_cfg_t retrieved_cfg = {0};
    ciot_err_t err = ciot_wifi_multi_get_cfg(test_multi_wifi, &retrieved_cfg);
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, err);
    TEST_ASSERT_EQUAL(2, retrieved_cfg.items_count);
    TEST_ASSERT_EQUAL(0, strncmp((char*)retrieved_cfg.items[0].ssid, "SSID1", 5));
}

void test_ciot_wifi_multi_get_info(void)
{
    ciot_wifi_multi_cfg_t cfg = {0};
    cfg.items_count = 1;
    strncpy((char*)cfg.items[0].ssid, "SSID1", sizeof(cfg.items[0].ssid) - 1);
    strncpy((char*)cfg.items[0].password, "Pass1", sizeof(cfg.items[0].password) - 1);
    cfg.items[0].valid = true;
    cfg.initial_index = 0;
    
    start_test_multi_wifi(test_multi_wifi, &cfg);
    
    ciot_wifi_multi_info_t info = {0};
    ciot_err_t err = ciot_wifi_multi_get_info(test_multi_wifi, &info);
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, err);
    TEST_ASSERT_EQUAL(0, info.active_index);
}

// ============================================================================
// Test: Empty configuration
// ============================================================================

void test_ciot_wifi_multi_start_with_no_networks(void)
{
    ciot_wifi_multi_cfg_t cfg = {0};
    cfg.items_count = 0;
    
    ciot_err_t err = start_test_multi_wifi(test_multi_wifi, &cfg);
    TEST_ASSERT_EQUAL(CIOT_ERR_INVALID_ARG, err);
}

void test_ciot_wifi_multi_next_with_no_networks(void)
{
    ciot_err_t err = ciot_wifi_multi_next(test_multi_wifi);
    TEST_ASSERT_EQUAL(CIOT_ERR_INVALID_STATE, err);
}

// ============================================================================
// Test: Connection failure - start sends STOPPED event (failover via event)
// ============================================================================

void test_ciot_wifi_multi_start_failover_when_first_network_sends_stopped_event(void)
{
    ciot_wifi_multi_cfg_t cfg = {0};
    cfg.items_count = 2;
    strncpy((char*)cfg.items[0].ssid, "SSID1", sizeof(cfg.items[0].ssid) - 1);
    strncpy((char*)cfg.items[0].password, "Pass1", sizeof(cfg.items[0].password) - 1);
    cfg.items[0].valid = true;
    strncpy((char*)cfg.items[1].ssid, "SSID2", sizeof(cfg.items[1].ssid) - 1);
    strncpy((char*)cfg.items[1].password, "Pass2", sizeof(cfg.items[1].password) - 1);
    cfg.items[1].valid = true;
    cfg.initial_index = 0;

    // Simulate connection failure: start sends STOPPED event
    mock_wifi_fail_on_start = true;

    ciot_err_t err = start_test_multi_wifi(test_multi_wifi, &cfg);
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, err);

    // Network 0 should have been invalidated and failover to network 1 triggered,
    // but network 1 also fails, so no valid network remains
    ciot_wifi_multi_status_t status = {0};
    ciot_wifi_multi_get_status(test_multi_wifi, &status);
    TEST_ASSERT_FALSE(status.items[0].valid);
    TEST_ASSERT_EQUAL(CIOT_WIFI_MULTI_ACTIVE_INDEX_NONE, status.active_index);
}

void test_ciot_wifi_multi_failover_succeeds_when_second_network_connects(void)
{
    ciot_wifi_multi_cfg_t cfg = {0};
    cfg.items_count = 2;
    strncpy((char*)cfg.items[0].ssid, "SSID1", sizeof(cfg.items[0].ssid) - 1);
    strncpy((char*)cfg.items[0].password, "Pass1", sizeof(cfg.items[0].password) - 1);
    cfg.items[0].valid = true;
    strncpy((char*)cfg.items[1].ssid, "SSID2", sizeof(cfg.items[1].ssid) - 1);
    strncpy((char*)cfg.items[1].password, "Pass2", sizeof(cfg.items[1].password) - 1);
    cfg.items[1].valid = true;
    cfg.initial_index = 0;

    // First start fails, then recovery succeeds
    mock_wifi_fail_on_start = true;
    ciot_err_t err = start_test_multi_wifi(test_multi_wifi, &cfg);
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, err);

    // Reset invalid networks and allow connection
    mock_wifi_fail_on_start = false;
    err = ciot_wifi_multi_reset_invalid(test_multi_wifi);
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, err);

    ciot_wifi_multi_status_t status = {0};
    ciot_wifi_multi_get_status(test_multi_wifi, &status);
    TEST_ASSERT_EQUAL(2, status.valid_count);
    TEST_ASSERT_NOT_EQUAL(CIOT_WIFI_MULTI_ACTIVE_INDEX_NONE, status.active_index);
}

void test_ciot_wifi_multi_mark_active_invalid_triggers_failover_via_event(void)
{
    ciot_wifi_multi_cfg_t cfg = {0};
    cfg.items_count = 2;
    strncpy((char*)cfg.items[0].ssid, "SSID1", sizeof(cfg.items[0].ssid) - 1);
    strncpy((char*)cfg.items[0].password, "Pass1", sizeof(cfg.items[0].password) - 1);
    cfg.items[0].valid = true;
    strncpy((char*)cfg.items[1].ssid, "SSID2", sizeof(cfg.items[1].ssid) - 1);
    strncpy((char*)cfg.items[1].password, "Pass2", sizeof(cfg.items[1].password) - 1);
    cfg.items[1].valid = true;
    cfg.initial_index = 0;

    ciot_err_t err = start_test_multi_wifi(test_multi_wifi, &cfg);
    TEST_ASSERT_EQUAL(CIOT_ERR_OK, err);

    ciot_wifi_multi_status_t status = {0};
    ciot_wifi_multi_get_status(test_multi_wifi, &status);
    TEST_ASSERT_EQUAL(0, status.active_index);

    // Simulate disconnection: stop sends STOPPED event, which wifi_multi intercepts
    // and calls mark_active_invalid internally
    ciot_wifi_multi_stop(test_multi_wifi);

    ciot_wifi_multi_get_status(test_multi_wifi, &status);
    TEST_ASSERT_EQUAL(CIOT_WIFI_MULTI_ACTIVE_INDEX_NONE, status.active_index);
}
