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

ciot_err_t ciot_sys_task(ciot_sys_t self)
{
    self->base.status.lifetime = ciot_timer_now();
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
