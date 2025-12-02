/**
 * @file ciot_dfu_nrf.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-12
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef __CIOT_DFU_NRF__H__
#define __CIOT_DFU_NRF__H__

#include "ciot_dfu.h"
#include "ciot_decoder_slip.h"

#define CIOT_DFU_NRF_TIMEOUT_SEC 2
#define CIOT_DFU_NRF_MAX_DFU_PKT_LEN_UART 64
#define CIOT_DFU_NRF_MAX_DFU_PKT_LEN_BLE 20

typedef enum ciot_dfu_nrf_op
{
    CIOT_DFU_NRF_OP_PROTOCOL_VERSION = 0x00,
    CIOT_DFU_NRF_OP_OBJECT_CREATE = 0x01,
    CIOT_DFU_NRF_OP_RECEIPT_NOTIF_SET = 0x02,
    CIOT_DFU_NRF_OP_CRC_GET = 0x03,
    CIOT_DFU_NRF_OP_OBJECT_EXECUTE = 0x04,
    CIOT_DFU_NRF_OP_OBJECT_SELECT = 0x06,
    CIOT_DFU_NRF_OP_MTU_GET = 0x07,
    CIOT_DFU_NRF_OP_OBJECT_WRITE = 0x08,
    CIOT_DFU_NRF_OP_PING = 0x09,
    CIOT_DFU_NRF_OP_HARDWARE_VERSION = 0x0A,
    CIOT_DFU_NRF_OP_FIRMWARE_VERSION = 0x0B,
    CIOT_DFU_NRF_OP_ABORT = 0x0C,
    CIOT_DFU_NRF_OP_RESPONSE = 0x60,
    CIOT_DFU_NRF_OP_SLIP_PACKET_END = 0xC0,
    CIOT_DFU_NRF_OP_INVALID = 0xFF,
} ciot_dfu_nrf_op_t;

typedef enum ciot_dfu_nrf_result
{
    CIOT_DFU_NRF_RES_CODE_INVALID = 0x00,
    CIOT_DFU_NRF_RES_CODE_SUCCESS = 0x01,
    CIOT_DFU_NRF_RES_CODE_OP_CODE_NOT_SUPPORTED = 0x02,
    CIOT_DFU_NRF_RES_CODE_INVALID_PARAMETER = 0x03,
    CIOT_DFU_NRF_RES_CODE_INSUFFICIENT_RESOURCES = 0x04,
    CIOT_DFU_NRF_RES_CODE_INVALID_OBJECT = 0x05,
    CIOT_DFU_NRF_RES_CODE_UNSUPPORTED_TYPE = 0x07,
    CIOT_DFU_NRF_RES_CODE_OPERATION_NOT_PERMITTED = 0x08,
    CIOT_DFU_NRF_RES_CODE_OPERATION_FAILED = 0x0A,
    CIOT_DFU_NRF_RES_CODE_EXT_ERROR = 0x0B,
} ciot_dfu_nrf_result_t;


typedef enum ciot_dfu_nrf_state
{
    CIOT_DFU_NRF_STATE_ERROR = -1,
    CIOT_DFU_NRF_STATE_IDLE,
    CIOT_DFU_NRF_STATE_SEND_PING,
    CIOT_DFU_NRF_STATE_WAITING_PING_RESP,
    CIOT_DFU_NRF_STATE_CREATE_OBJECT,
    CIOT_DFU_NRF_STATE_WAITING_CREATE_OBJ,
    CIOT_DFU_NRF_STATE_WRITE_DFU_PACKAGE,
    CIOT_DFU_NRF_STATE_REQUEST_CRC,
    CIOT_DFU_NRF_STATE_WAITING_CRC,
    CIOT_DFU_NRF_STATE_REQUEST_EXECUTE,
    CIOT_DFU_NRF_STATE_WAITING_EXECUTE,
    CIOT_DFU_NRF_STATE_COMPLETED,
} ciot_dfu_nrf_state_t;

typedef enum ciot_dfu_nrf_packet_type
{
    CIOT_DFU_NRF_PACKET_TYPE_INIT = 0x01,
    CIOT_DFU_NRF_PACKET_TYPE_APP_IMAGE,
} ciot_dfu_nrf_packet_type_t;

typedef struct ciot_dfu_nrf_packet
{
    ciot_dfu_nrf_packet_type_t type;
    uint32_t size;
    uint8_t *data;
    bool transferred;
} ciot_dfu_nrf_packet_t;

typedef struct ciot_dfu_nrf_cfg
{
    ciot_dfu_cfg_t dfu;
    ciot_dfu_nrf_packet_t init_packet;
    ciot_dfu_nrf_packet_t app_image;
    uint32_t max_package_len;
    int iface_sys_id;
    ciot_iface_t *iface;
    ciot_msg_data_t *iface_cfg;
} ciot_dfu_nrf_cfg_t;

typedef struct ciot_dfu_nrf *ciot_dfu_nrf_t;

ciot_dfu_nrf_t ciot_dfu_nrf_new(ciot_dfu_nrf_cfg_t *cfg);
ciot_err_t ciot_dfu_nrf_start(ciot_dfu_nrf_t self, ciot_dfu_cfg_t *cfg);
ciot_err_t ciot_dfu_nrf_stop(ciot_dfu_nrf_t self);
ciot_err_t ciot_dfu_nrf_init(ciot_dfu_nrf_t self);
ciot_err_t ciot_dfu_nrf_process_req(ciot_dfu_nrf_t self, ciot_dfu_req_t *req);
ciot_err_t ciot_dfu_nrf_get_cfg(ciot_dfu_nrf_t self, ciot_dfu_cfg_t *cfg);
ciot_err_t ciot_dfu_nrf_get_status(ciot_dfu_nrf_t self, ciot_dfu_status_t *status);
ciot_err_t ciot_dfu_nrf_task(ciot_dfu_nrf_t self);
ciot_err_t ciot_dfu_nrf_read_file(ciot_dfu_nrf_packet_t *object, const char *name);
ciot_err_t ciot_dfu_nrf_start_bootloader(ciot_dfu_nrf_t self, ciot_iface_t *iface, int sys_id);
ciot_dfu_nrf_state_t ciot_dfu_nrf_state(ciot_dfu_nrf_t self);

#endif  //!__CIOT_DFU_NRF__H__
