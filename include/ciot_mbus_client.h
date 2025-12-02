/**
 * @file ciot_mbus_client.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-02-04
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef __CIOT_MBUS_CLIENT__H__
#define __CIOT_MBUS_CLIENT__H__

#ifdef __cplusplus
extern "C" {
#endif

#include "ciot_config.h"
#include "ciot_types.h"
#include "ciot_iface.h"
#include "ciot_mbus.h"
#include "nanomodbus.h"

typedef struct ciot_mbus_client *ciot_mbus_client_t;

typedef struct ciot_mbus_client_base
{
    ciot_iface_t iface;
    ciot_mbus_client_cfg_t cfg;
    ciot_mbus_client_status_t status;
} ciot_mbus_client_base_t;

ciot_mbus_client_t ciot_mbus_client_new(void *handle, ciot_iface_t *iface);
ciot_err_t ciot_mbus_client_init(ciot_mbus_client_t self);
ciot_err_t ciot_mbus_client_start(ciot_mbus_client_t self, ciot_mbus_client_cfg_t *cfg);
ciot_err_t ciot_mbus_client_stop(ciot_mbus_client_t self);
ciot_err_t ciot_mbus_client_process_req(ciot_mbus_client_t self, ciot_mbus_client_req_t *req);
ciot_err_t ciot_mbus_client_get_cfg(ciot_mbus_client_t self, ciot_mbus_client_cfg_t *cfg);
ciot_err_t ciot_mbus_client_get_status(ciot_mbus_client_t self, ciot_mbus_client_status_t *status);
ciot_err_t ciot_mbus_client_read_coils(ciot_mbus_client_t self, uint16_t address, uint16_t quantity, nmbs_bitfield coils_out);
ciot_err_t ciot_mbus_client_read_discrete_inputs(ciot_mbus_client_t self, uint16_t address, uint16_t quantity, nmbs_bitfield inputs_out);
ciot_err_t ciot_mbus_client_read_holding_registers(ciot_mbus_client_t self, uint16_t address, uint16_t quantity, uint16_t* registers_out);
ciot_err_t ciot_mbus_client_read_input_registers(ciot_mbus_client_t self, uint16_t address, uint16_t quantity, uint16_t* registers_out);
ciot_err_t ciot_mbus_client_write_single_coil(ciot_mbus_client_t self, uint16_t address, bool value);
ciot_err_t ciot_mbus_client_write_single_register(ciot_mbus_client_t self, uint16_t address, uint16_t value);
ciot_err_t ciot_mbus_client_write_multiple_coils(ciot_mbus_client_t self, uint16_t address, uint16_t quantity, const nmbs_bitfield coils);
ciot_err_t ciot_mbus_client_write_multiple_registers(ciot_mbus_client_t self, uint16_t address, uint16_t quantity, const uint16_t* registers);

#ifdef __cplusplus
}
#endif

#endif  //!__CIOT_MBUS_CLIENT__H__