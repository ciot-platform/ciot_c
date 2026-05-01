/**
 * @file main.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-04-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <stdio.h>
#include "unity.h"
#include "ciot_custom_config.h"

void test_ciot_decoder_s();
void test_ciot_decoder_slip();
void test_ciot_crypt();

// Queue regression tests
void test_ciot_queue_starting_discards_unexpected_event_and_continues(void);
void test_ciot_queue_custom_msg_handler_error_does_not_leave_dead_item(void);

// WiFi Multi tests
void test_ciot_wifi_multi_new_with_injected_wifi(void);
void test_ciot_wifi_multi_new_creates_internal_wifi(void);
void test_ciot_wifi_multi_init_null_check(void);
void test_ciot_wifi_multi_start_null_check(void);
void test_ciot_wifi_multi_stop_null_check(void);
void test_ciot_wifi_multi_task_in_idle_does_nothing(void);
void test_ciot_wifi_multi_start_requires_preconfigured_items(void);
void test_ciot_wifi_multi_start_with_single_network(void);
void test_ciot_wifi_multi_start_keeps_current_connection_when_ssid_matches(void);
void test_ciot_wifi_multi_start_with_two_networks(void);
void test_ciot_wifi_multi_start_with_invalid_initial_index(void);
void test_ciot_wifi_multi_stop(void);
void test_ciot_wifi_multi_next_rotates_to_next_network(void);
void test_ciot_wifi_multi_next_wraps_around(void);
void test_ciot_wifi_multi_next_skips_invalid_networks(void);
void test_ciot_wifi_multi_start_schedules_next_switch_when_enabled(void);
void test_ciot_wifi_multi_task_rotates_when_switch_is_due(void);
void test_ciot_wifi_multi_task_rotates_and_returns_to_first_network(void);
void test_ciot_wifi_multi_task_retries_invalid_networks_in_rotation(void);
void test_ciot_wifi_multi_task_returns_to_previous_network_when_switch_fails(void);
void test_ciot_wifi_multi_task_does_nothing_when_schedule_disabled(void);
void test_ciot_wifi_multi_mark_invalid_by_index(void);
void test_ciot_wifi_multi_mark_active_invalid_triggers_failover(void);
void test_ciot_wifi_multi_mark_invalid_no_valid_networks_left(void);
void test_ciot_wifi_multi_reset_invalid_recovers_networks(void);
void test_ciot_wifi_multi_reset_invalid_when_no_active_network(void);
void test_ciot_wifi_multi_process_req_next(void);
void test_ciot_wifi_multi_process_req_set_invalid(void);
void test_ciot_wifi_multi_process_req_reset_invalid(void);
void test_ciot_wifi_multi_process_req_set_item_updates_ram_only(void);
void test_ciot_wifi_multi_process_req_set_item_requires_config(void);
void test_ciot_wifi_multi_get_cfg(void);
void test_ciot_wifi_multi_get_info(void);
void test_ciot_wifi_multi_start_with_no_networks(void);
void test_ciot_wifi_multi_next_with_no_networks(void);
void test_ciot_wifi_multi_start_failover_when_first_network_sends_stopped_event(void);
void test_ciot_wifi_multi_failover_succeeds_when_second_network_connects(void);
void test_ciot_wifi_multi_mark_active_invalid_triggers_failover_via_event(void);

int main(void)
{
    UNITY_BEGIN();
    
    // Original tests
    test_ciot_decoder_s();
    test_ciot_decoder_slip();
    test_ciot_crypt();
    
    // Queue regression tests
    RUN_TEST(test_ciot_queue_starting_discards_unexpected_event_and_continues);
    RUN_TEST(test_ciot_queue_custom_msg_handler_error_does_not_leave_dead_item);

    // WiFi Multi tests
    RUN_TEST(test_ciot_wifi_multi_new_with_injected_wifi);
    RUN_TEST(test_ciot_wifi_multi_new_creates_internal_wifi);
    RUN_TEST(test_ciot_wifi_multi_init_null_check);
    RUN_TEST(test_ciot_wifi_multi_start_null_check);
    RUN_TEST(test_ciot_wifi_multi_stop_null_check);
    RUN_TEST(test_ciot_wifi_multi_task_in_idle_does_nothing);
    RUN_TEST(test_ciot_wifi_multi_start_requires_preconfigured_items);
    RUN_TEST(test_ciot_wifi_multi_start_with_single_network);
    RUN_TEST(test_ciot_wifi_multi_start_keeps_current_connection_when_ssid_matches);
    RUN_TEST(test_ciot_wifi_multi_start_with_two_networks);
    RUN_TEST(test_ciot_wifi_multi_start_with_invalid_initial_index);
    RUN_TEST(test_ciot_wifi_multi_stop);
    RUN_TEST(test_ciot_wifi_multi_next_rotates_to_next_network);
    RUN_TEST(test_ciot_wifi_multi_next_wraps_around);
    RUN_TEST(test_ciot_wifi_multi_next_skips_invalid_networks);
    RUN_TEST(test_ciot_wifi_multi_start_schedules_next_switch_when_enabled);
    RUN_TEST(test_ciot_wifi_multi_task_rotates_when_switch_is_due);
    RUN_TEST(test_ciot_wifi_multi_task_rotates_and_returns_to_first_network);
    RUN_TEST(test_ciot_wifi_multi_task_retries_invalid_networks_in_rotation);
    RUN_TEST(test_ciot_wifi_multi_task_returns_to_previous_network_when_switch_fails);
    RUN_TEST(test_ciot_wifi_multi_task_does_nothing_when_schedule_disabled);
    RUN_TEST(test_ciot_wifi_multi_mark_invalid_by_index);
    RUN_TEST(test_ciot_wifi_multi_mark_active_invalid_triggers_failover);
    RUN_TEST(test_ciot_wifi_multi_mark_invalid_no_valid_networks_left);
    RUN_TEST(test_ciot_wifi_multi_reset_invalid_recovers_networks);
    RUN_TEST(test_ciot_wifi_multi_reset_invalid_when_no_active_network);
    RUN_TEST(test_ciot_wifi_multi_process_req_next);
    RUN_TEST(test_ciot_wifi_multi_process_req_set_invalid);
    RUN_TEST(test_ciot_wifi_multi_process_req_reset_invalid);
    RUN_TEST(test_ciot_wifi_multi_process_req_set_item_updates_ram_only);
    RUN_TEST(test_ciot_wifi_multi_process_req_set_item_requires_config);
    RUN_TEST(test_ciot_wifi_multi_get_cfg);
    RUN_TEST(test_ciot_wifi_multi_get_info);
    RUN_TEST(test_ciot_wifi_multi_start_with_no_networks);
    RUN_TEST(test_ciot_wifi_multi_next_with_no_networks);
    RUN_TEST(test_ciot_wifi_multi_start_failover_when_first_network_sends_stopped_event);
    RUN_TEST(test_ciot_wifi_multi_failover_succeeds_when_second_network_connects);
    RUN_TEST(test_ciot_wifi_multi_mark_active_invalid_triggers_failover_via_event);
    
    return UNITY_END();
}

