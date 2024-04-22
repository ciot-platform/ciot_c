/**
 * @file ciot_nrf_dfu.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-04-18
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef __CIOT_NRF_DFU__H__
#define __CIOT_NRF_DFU__H__

#include "ciot_dfu.h"

#define CIOT_NRF_DFU_TIMEOUT_SEC 2
#define CIOT_NRF_DFU_MAX_DFU_PKT_LEN_UART 64
#define CIOT_NRF_DFU_MAX_DFU_PKT_LEN_BLE 20

typedef enum ciot_nrf_dfu_packet_type
{
    CIOT_NRF_DFU_PACKET_TYPE_INIT = 0x01,
    CIOT_NRF_DFU_PACKET_TYPE_APP_IMAGE,
} ciot_nrf_dfu_packet_type_t;

typedef enum ciot_nrf_dfu_op
{
    CIOT_NRF_DFU_OP_PROTOCOL_VERSION = 0x00,  //!< Retrieve protocol version.
    CIOT_NRF_DFU_OP_OBJECT_CREATE = 0x01,     //!< Create selected object.
    CIOT_NRF_DFU_OP_RECEIPT_NOTIF_SET = 0x02, //!< Set receipt notification.
    CIOT_NRF_DFU_OP_CRC_GET = 0x03,           //!< Request CRC of selected object.
    CIOT_NRF_DFU_OP_OBJECT_EXECUTE = 0x04,    //!< Execute selected object.
    CIOT_NRF_DFU_OP_OBJECT_SELECT = 0x06,     //!< Select object.
    CIOT_NRF_DFU_OP_MTU_GET = 0x07,           //!< Retrieve MTU size.
    CIOT_NRF_DFU_OP_OBJECT_WRITE = 0x08,      //!< Write selected object.
    CIOT_NRF_DFU_OP_PING = 0x09,              //!< Ping.
    CIOT_NRF_DFU_OP_HARDWARE_VERSION = 0x0A,  //!< Retrieve hardware version.
    CIOT_NRF_DFU_OP_FIRMWARE_VERSION = 0x0B,  //!< Retrieve firmware version.
    CIOT_NRF_DFU_OP_ABORT = 0x0C,             //!< Abort the DFU procedure.
    CIOT_NRF_DFU_OP_RESPONSE = 0x60,          //!< Response.
    CIOT_NRF_DFU_OP_SLIP_PACKET_END = 0xc0,   //!< Slip end of packet.
    CIOT_NRF_DFU_OP_INVALID = 0xFF,           //!< Invalid request.
} ciot_nrf_dfu_op_t;

typedef enum ciot_nrf_dfu_result
{
    CIOT_NRF_DFU_RES_CODE_INVALID = 0x00,                 //!< Invalid opcode.
    CIOT_NRF_DFU_RES_CODE_SUCCESS = 0x01,                 //!< Operation successful.
    CIOT_NRF_DFU_RES_CODE_OP_CODE_NOT_SUPPORTED = 0x02,   //!< Opcode not supported.
    CIOT_NRF_DFU_RES_CODE_INVALID_PARAMETER = 0x03,       //!< Missing or invalid parameter value.
    CIOT_NRF_DFU_RES_CODE_INSUFFICIENT_RESOURCES = 0x04,  //!< Not enough memory for the data object.
    CIOT_NRF_DFU_RES_CODE_INVALID_OBJECT = 0x05,          //!< Data object does not match the firmware and hardware requirements, the signature is wrong, or parsing the command failed.
    CIOT_NRF_DFU_RES_CODE_UNSUPPORTED_TYPE = 0x07,        //!< Not a valid object type for a Create request.
    CIOT_NRF_DFU_RES_CODE_OPERATION_NOT_PERMITTED = 0x08, //!< The state of the DFU process does not allow this operation.
    CIOT_NRF_DFU_RES_CODE_OPERATION_FAILED = 0x0A,        //!< Operation failed.
    CIOT_NRF_DFU_RES_CODE_EXT_ERROR = 0x0B,               //!< Extended error. The next byte of the response contains the error code of the extended error (see @ref nrf_dfu_ext_error_code_t.
} ciot_nrf_dfu_result_t;

typedef enum ciot_nrf_dfu_state
{
    CIOT_NRF_DFU_STATE_ERROR = -1,
    CIOT_NRF_DFU_STATE_IDLE,
    CIOT_NRF_DFU_STATE_SEND_PING_RESP,
    CIOT_NRF_DFU_STATE_WAITING_PING_RESP,
    CIOT_NRF_DFU_STATE_CREATE_OBJECT,
    CIOT_NRF_DFU_STATE_WAITING_CREATE_OBJ,
    CIOT_NRF_DFU_STATE_WRITE_DFU_PACKAGE,
    CIOT_NRF_DFU_STATE_REQUEST_CRC,
    CIOT_NRF_DFU_STATE_WAITING_CRC,
    CIOT_NRF_DFU_STATE_REQUEST_EXECUTE,
    CIOT_NRF_DFU_STATE_WAITING_EXECUTE,
    CIOT_NRF_DFU_STATE_COMPLETED,
} ciot_nrf_dfu_state_t;

typedef struct ciot_nrf_dfu_packet
{
    ciot_nrf_dfu_packet_type_t type;
    uint32_t size;
    uint8_t *data;
    bool transferred;
} ciot_nrf_dfu_packet_t;

typedef struct ciot_nrf_dfu_cfg
{
    ciot_dfu_cfg_t dfu;
    ciot_nrf_dfu_packet_t init_packet;
    ciot_nrf_dfu_packet_t app_image;
    uint32_t max_package_len;
} ciot_nrf_dfu_cfg_t;

ciot_dfu_t ciot_nrf_dfu_new(ciot_nrf_dfu_cfg_t *cfg, ciot_iface_t *iface);
ciot_err_t ciot_nrf_dfu_start(ciot_dfu_t self, ciot_dfu_cfg_t *cfg);
ciot_err_t ciot_nrf_uart_stop(ciot_dfu_t self);
ciot_err_t ciot_nrf_dfu_process_req(ciot_dfu_t self, ciot_dfu_req_t *req);
ciot_err_t ciot_nrf_dfu_send_data(ciot_dfu_t self, uint8_t *data, int size);

ciot_err_t ciot_nrf_dfu_task(ciot_dfu_t self);
ciot_err_t ciot_nrf_dfu_send_firmware(ciot_dfu_t self);
ciot_err_t ciot_nrf_dfu_read_file(ciot_nrf_dfu_packet_t *object, const char *name);
ciot_nrf_dfu_state_t ciot_nrf_dfu_state(ciot_dfu_t self);

#endif  //!__CIOT_NRF_DFU__H__