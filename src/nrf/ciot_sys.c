/**
 * @file ciot_sys.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "ciot_config.h"

#if CIOT_CONFIG_FEATURE_SYS == 1 && defined(CIOT_PLATFORM_NRF)

#include <stdlib.h>
#if defined(__GNUC__) && !defined(__ARMCC_VERSION)
#include <stddef.h>
#include <malloc.h>
#endif
#include "app_util.h"
#include "nrf_nvic.h"
#include "nrf_soc.h"
#include "nrf_bootloader_info.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_log_ctrl.h"
#include "ciot_sys.h"
#include "ciot_timer.h"
#include "ciot_err.h"

// static const char *TAG = "ciot_sys";

struct ciot_sys
{
    ciot_sys_base_t base;
};

ciot_sys_t ciot_sys_new(void *handle)
{
    ciot_sys_t self = calloc(1, sizeof(struct ciot_sys));
    ciot_sys_init(self);
    return self;
}

ciot_err_t ciot_sys_start(ciot_sys_t self, ciot_sys_cfg_t *cfg)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(cfg);
    return CIOT_ERR_NOT_IMPLEMENTED;
}

ciot_err_t ciot_sys_stop(ciot_sys_t self)
{
    CIOT_ERR_NULL_CHECK(self);
    return CIOT_ERR_NOT_IMPLEMENTED;
}

#if defined(__GNUC__) && !defined(__ARMCC_VERSION)
// newlib; <unistd.h> hides it under -std=c99.
extern void *_sbrk(ptrdiff_t incr);

// What the application can still allocate, as esp_get_free_heap_size() on ESP32: the RAM the heap
// has not taken yet, up to the bottom of the stack reservation (STACK_BASE; newlib's _sbrk from
// libnosys does not stop at __HeapLimit), plus the blocks freed back to malloc.
static uint32_t ciot_sys_free_memory(void)
{
    struct mallinfo info = mallinfo();
    uint32_t free_memory = (uint32_t)info.fordblks;
    uintptr_t heap_end = (uintptr_t)_sbrk(0);
    uintptr_t stack_base = (uintptr_t)STACK_BASE;
    if (heap_end < stack_base)
    {
        free_memory += (uint32_t)(stack_base - heap_end);
    }
    return free_memory;
}
#else
static uint32_t ciot_sys_free_memory(void)
{
    return 0;
}
#endif

ciot_err_t ciot_sys_task(ciot_sys_t self)
{
    uint32_t now = ciot_timer_now();
    // mallinfo() walks the free list, so free_memory is refreshed once per second.
    if (now != self->base.status.lifetime || self->base.status.free_memory == 0)
    {
        self->base.status.free_memory = ciot_sys_free_memory();
    }
    self->base.status.lifetime = now;
    if (NRF_LOG_PROCESS() == false)
	{
		#if (NRF_SD_BLE_API_VERSION == 2 || NRF_SD_BLE_API_VERSION == 3)	
			ret_code_t err_code = sd_app_evt_wait();
			APP_ERROR_CHECK(err_code);
		#else
			nrf_pwr_mgmt_run();
		#endif
	}
    return CIOT_ERR_OK;
}

ciot_err_t ciot_sys_restart(void)
{
    sd_nvic_SystemReset();
    return CIOT_ERR_OK;
}

ciot_err_t ciot_sys_init_dfu(void)
{
    uint32_t err_code;

    err_code = (sd_power_gpregret_clr(0, 0xffffffff));
    // VERIFY_SUCCESS(err_code);

    err_code = (sd_power_gpregret_set(0, BOOTLOADER_DFU_START));
    // VERIFY_SUCCESS(err_code);

    nrf_pwr_mgmt_shutdown(NRF_PWR_MGMT_SHUTDOWN_GOTO_DFU);
    
    return err_code;
}

#endif // CIOT_CONFIG_FEATURE_SYS == 1
