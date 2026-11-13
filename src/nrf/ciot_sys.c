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
// _NEWLIB_VERSION comes from <newlib.h>; included explicitly so the check below does not depend on
// what <stdlib.h> happens to pull in.
#if defined(__has_include)
#if __has_include(<newlib.h>)
#include <newlib.h>
#endif
#endif
#if defined(_NEWLIB_VERSION)
#include <stddef.h>
#include <malloc.h>
#include "app_util.h"
#include "app_util_platform.h"
#endif
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

static uint32_t ciot_sys_free_memory(void);

ciot_sys_t ciot_sys_new(void *handle)
{
    ciot_sys_t self = calloc(1, sizeof(struct ciot_sys));
    if (self == NULL)
    {
        return NULL;
    }
    ciot_sys_init(self);
    // First value; ciot_sys_task refreshes it once per second.
    self->base.status.free_memory = ciot_sys_free_memory();
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

#if defined(_NEWLIB_VERSION)
// newlib; <unistd.h> hides it under -std=c99.
extern void *_sbrk(ptrdiff_t incr);

// What the application can still allocate, as esp_get_free_heap_size() on ESP32: the RAM the heap
// has not taken yet, up to the bottom of the stack reservation (STACK_BASE; newlib's _sbrk from
// libnosys does not stop at __HeapLimit), plus the blocks freed back to malloc.
// Only newlib malloc is covered. The value is too high with an _sbrk that stops at __HeapLimit, and
// it does not see an RTOS heap (e.g. FreeRTOS pvPortMalloc).
// Read in a critical region: newlib-nano has no malloc lock without an RTOS, and an interrupt that
// allocates or frees would change the free list mallinfo() walks, or the heap end, mid-read.
static uint32_t ciot_sys_free_memory(void)
{
    struct mallinfo info;
    uintptr_t heap_end;
    CRITICAL_REGION_ENTER();
    info = mallinfo();
    heap_end = (uintptr_t)_sbrk(0);
    CRITICAL_REGION_EXIT();
    uint32_t free_memory = (uint32_t)info.fordblks;
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
    uint32_t now = (uint32_t)ciot_timer_now();
    // mallinfo() walks the free list, so free_memory is refreshed once per second (it stays at the
    // value read in ciot_sys_new while ciot_timer is not running, as lifetime does).
    if (now != self->base.status.lifetime)
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
