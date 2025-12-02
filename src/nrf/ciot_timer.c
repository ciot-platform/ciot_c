/**
 * @file ciot_timer.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-16
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "ciot_config.h"

#if CIOT_CONFIG_FEATURE_TIMER == 1 && defined(CIOT_PLATFORM_NRF)

#include "ciot_timer.h"
#include "nrf_error.h"
#include "nrf_drv_rtc.h"
#include "nrf_drv_clock.h"

#define RTC_PRESCALER 4095
#define TIME_CONST (32768 / (RTC_PRESCALER + 1))
#define ONE_SECOND_TIMER (1 * TIME_CONST)
#define QUARTER_TIMER 1

typedef struct ciot_timer
{
    uint64_t now;
    ciot_timer_cfg_t cfg;
} ciot_timer_t;

static void hg_ble_rtc_event_handler(nrf_drv_rtc_int_type_t int_type);

static const nrf_drv_rtc_t rtc = NRF_DRV_RTC_INSTANCE(1);

static ciot_timer_t self;

ciot_err_t ciot_timer_start(ciot_timer_cfg_t *cfg)
{
    uint32_t err_code = NRF_SUCCESS;

    self.cfg = *cfg;

    err_code = nrf_drv_clock_init();
    APP_ERROR_CHECK(err_code);

    nrf_drv_clock_lfclk_request(NULL);

    nrf_drv_rtc_config_t config = NRF_DRV_RTC_DEFAULT_CONFIG;
    config.prescaler = RTC_PRESCALER;
    err_code = nrf_drv_rtc_init(&rtc, &config, hg_ble_rtc_event_handler);
    if (err_code)
    {
        return err_code;
    }

    err_code = nrf_drv_rtc_cc_set(&rtc, 0, ONE_SECOND_TIMER, true);
    if (err_code)
    {
        return err_code;
    }

    nrf_drv_rtc_enable(&rtc);

    return err_code;
}

uint64_t ciot_timer_now(void)
{
    return self.now;
}

static void hg_ble_rtc_event_handler(nrf_drv_rtc_int_type_t int_type)
{
    switch (int_type)
    {
    case NRF_DRV_RTC_INT_COMPARE0:
        nrf_drv_rtc_counter_clear(&rtc);
        nrf_drv_rtc_cc_set(&rtc, 0, ONE_SECOND_TIMER, true); // unused error return value
        self.now++;
        if(self.cfg.event_handler != NULL)
        {
            self.cfg.event_handler(self.cfg.event_args);
        }
        break;
    default:
        break;
    }
}

#endif // CIOT_CONFIG_FEATURE_TIMER == 1
