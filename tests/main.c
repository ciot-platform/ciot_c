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

void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

int main(void)
{
    UNITY_BEGIN();
    test_ciot_decoder_s();
    test_ciot_decoder_slip();
    test_ciot_crypt();
    return UNITY_END();
}
