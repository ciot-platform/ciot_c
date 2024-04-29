/**
 * @file ciot_nrf_dfu.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-20
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>

#include "ciot_config.h"
#include "ciot_nrf_dfu.h"
#include "ciot_sys.h"
#include "ciot_slip.h"
#include "ciot_utils.h"
#include "ciot_crc.h"
#include "ciot_uart.h"

#define MAX_OBJECT_SIZE 4096
#define PING_ID 0x01
#define TIMEOUT_RESET 0
#define PKT_SET_PRN_PARAM_LEN (1) /**< Length (in bytes) of the parameters for Set Packet Receipt Notification request. */

typedef enum ciot_nrf_dfu_obj
{
    CIOT_NRF_DFU_OBJ_PACKET = 0x01,
    CIOT_NRF_DFU_OBJ_APP_IMG,
} ciot_nrf_dfu_obj_t;

typedef struct ciot_dfu_crc
{
    uint32_t expected;
    uint32_t received;
} ciot_dfu_crc_t;

typedef struct ciot_nrf_dfu_object
{
    uint16_t size;
    uint16_t remaining;
    ciot_nrf_dfu_packet_t *packet;
} ciot_nrf_dfu_object_t;

typedef struct ciot_dfu_msg
{
    uint32_t size;
    uint8_t data[32];
    ciot_dfu_crc_t crc;
} ciot_dfu_msg_t;

typedef struct ciot_nrf_dfu_crc
{
    uint32_t expected;
    uint32_t received;
} ciot_nrf_dfu_crc_t;

typedef struct ciot_nrf_dfu_slip
{
    ciot_slip_t handle;
    uint8_t buffer[256];
} ciot_nrf_dfu_slip_t;

struct ciot_dfu
{
    ciot_iface_t iface;
    ciot_nrf_dfu_cfg_t cfg;
    ciot_dfu_status_t status;

    ciot_nrf_dfu_slip_t slip;
    ciot_nrf_dfu_state_t state;
    ciot_nrf_dfu_crc_t crc;
    uint8_t data[CIOT_NRF_DFU_MAX_DFU_PKT_LEN_UART];
    uint16_t prn_counter;
    uint16_t data_transferred;
    ciot_nrf_dfu_object_t object;

    bool cache_bridge_mode;
    void *cache_event_args;
    ciot_iface_event_handler_t *cache_event_handler;
};

static ciot_err_t ciot_nrf_dfu_set_state(ciot_dfu_t self, ciot_dfu_state_t state);
static ciot_err_t ciot_nrf_dfu_event_handler(ciot_iface_t *sender, ciot_iface_event_t *event, void *args);
static ciot_err_t ciot_nrf_dfu_start_bootloader(ciot_dfu_t self);
static ciot_err_t ciot_nrf_dfu_write(ciot_dfu_t self);
static ciot_err_t ciot_nrf_dfu_process_data(ciot_dfu_t self, uint8_t *data, int32_t len);
static ciot_err_t ciot_nrf_dfu_slip_encode_and_send(ciot_dfu_t self, uint8_t *data, uint32_t len);
static uint32_t ciot_nrf_dfu_timeout_check(uint32_t timeout);

static const char *TAG = "ciot_nrf_dfu";

ciot_dfu_t ciot_nrf_dfu_new(ciot_nrf_dfu_cfg_t *cfg)
{
    ciot_dfu_t self = calloc(1, sizeof(struct ciot_dfu));
    self->cfg = *cfg;
    self->iface.base.ptr = self;
    self->iface.base.start = (ciot_iface_start_fn *)ciot_nrf_dfu_start;
    self->iface.base.stop = (ciot_iface_stop_fn *)ciot_nrf_dfu_stop;
    self->iface.base.process_req = (ciot_iface_process_req_fn *)ciot_nrf_dfu_process_req;
    self->iface.base.send_data = (ciot_iface_send_data_fn *)ciot_nrf_dfu_send_data;
    self->iface.base.cfg.ptr = &self->cfg.dfu;
    self->iface.base.cfg.size = sizeof(self->cfg.dfu);
    self->iface.base.status.ptr = &self->status;
    self->iface.base.status.size = sizeof(self->status);
    self->iface.info.type = CIOT_IFACE_TYPE_DFU;
    return self;
}

ciot_err_t ciot_nrf_dfu_start(ciot_dfu_t self, ciot_dfu_cfg_t *cfg)
{
    CIOT_NULL_CHECK(self);
    CIOT_NULL_CHECK(cfg);

    if(cfg->type != self->cfg.dfu.type)
    {
        return CIOT_ERR_INVALID_TYPE;
    }
    
    self->slip.handle.p_buffer = self->slip.buffer;
    self->slip.handle.buffer_len = sizeof(self->slip.buffer);

    self->prn_counter = 0;
    self->cfg.init_packet.type = CIOT_NRF_DFU_PACKET_TYPE_INIT;
    self->cfg.app_image.type = CIOT_NRF_DFU_PACKET_TYPE_APP_IMAGE;
    self->object.packet = &self->cfg.init_packet;

    return CIOT_OK;
}

ciot_err_t ciot_nrf_dfu_stop(ciot_dfu_t self)
{
    if(self->cfg.iface->info.type == CIOT_IFACE_TYPE_UART)
    {
        ciot_uart_set_bridge_mode((ciot_uart_t)self->cfg.iface, false);
    }
    self->cfg.iface->event_args = self->cache_event_args;
    self->cfg.iface->event_handler = self->cache_event_handler;
    return CIOT_OK;
}

ciot_err_t ciot_nrf_dfu_process_req(ciot_dfu_t self, ciot_dfu_req_t *req)
{
    return CIOT_ERR_NOT_IMPLEMENTED;
}

ciot_err_t ciot_nrf_dfu_send_data(ciot_dfu_t self, uint8_t *data, int size)
{
    if(self->cfg.iface->info.type == CIOT_IFACE_TYPE_UART)
    {
        return ciot_nrf_dfu_slip_encode_and_send(self, data, size);
    }
    else
    {
        return ciot_iface_send_data(self->cfg.iface, data, size);
    }
}

ciot_err_t ciot_nrf_dfu_task(ciot_dfu_t self)
{
    CIOT_NULL_CHECK(self);

    if(self->state == CIOT_NRF_DFU_STATE_IDLE || self->state == CIOT_NRF_DFU_STATE_COMPLETED)
    {
        return CIOT_OK;
    }

    ciot_err_t err = ciot_nrf_dfu_write(self);
    if(err != CIOT_OK)
    {
        self->state = CIOT_NRF_DFU_STATE_ERROR;
    }

    return err;
}

ciot_err_t ciot_nrf_dfu_send_firmware(ciot_dfu_t self)
{
    ciot_nrf_dfu_start_bootloader(self);
    if(self->cfg.iface->info.type == CIOT_IFACE_TYPE_UART)
    {
        ciot_uart_set_bridge_mode((ciot_uart_t)self->cfg.iface, true);
    }
    self->cache_event_args = self->cfg.iface->event_args;
    self->cache_event_handler = self->cfg.iface->event_handler;
    ciot_iface_register_event(self->cfg.iface, ciot_nrf_dfu_event_handler, self);
    self->state = CIOT_NRF_DFU_STATE_SEND_PING_RESP;
    return CIOT_OK;
}

ciot_err_t ciot_nrf_dfu_read_file(ciot_nrf_dfu_packet_t *object, const char *name)
{
    FILE *file = fopen(name, "rb");

    if(file == NULL)
    {
        CIOT_LOGE(TAG, "Error opening file");
        return CIOT_ERR_NOT_FOUND;
    }

    if (object == NULL || object->data != NULL)
    {
        CIOT_LOGE(TAG, "Invalid arg");
        return CIOT_ERR_INVALID_ARG;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    object->data = (uint8_t *)malloc(file_size * sizeof(uint8_t));
    object->size = file_size;
    if (object->data == NULL)
    {
        CIOT_LOGE(TAG, "Error allocating memory");
        return CIOT_ERR_NO_MEMORY;
    }

    size_t elements_read = fread(object->data, sizeof(uint8_t), file_size, file);
    if (elements_read != file_size) {
        CIOT_LOGE(TAG, "Error reading file");
        return CIOT_FAIL;
    }

    return 0;
}

ciot_nrf_dfu_state_t ciot_nrf_dfu_state(ciot_dfu_t self)
{
    return self->state;
}

static ciot_err_t ciot_nrf_dfu_start_bootloader(ciot_dfu_t self)
{
    ciot_msg_t msg = {
        .type = CIOT_MSG_TYPE_REQUEST,
        .iface.id = self->cfg.target_sys_iface_id,
        .iface.type = CIOT_IFACE_TYPE_SYSTEM,
        .data.system.request.type = CIOT_SYS_REQ_INIT_DFU
    };
    return ciot_iface_send_req(self->cfg.iface, &msg, CIOT_MSG_HEADER_SIZE + 1);
}

static ciot_err_t ciot_nrf_dfu_write(ciot_dfu_t self)
{
    CIOT_NULL_CHECK(self);

    // Send Ping [x09 x01]
    if(self->state == CIOT_NRF_DFU_STATE_SEND_PING_RESP)
    {
        CIOT_LOGI(TAG, "Send ping");
        self->data[0] = CIOT_NRF_DFU_OP_PING;
        self->data[1] = PING_ID;
        self->state = CIOT_NRF_DFU_STATE_WAITING_PING_RESP;
        ciot_nrf_dfu_timeout_check(TIMEOUT_RESET);
        return ciot_nrf_dfu_send_data(self, self->data, 2);
    }

    // Create command: size [x01 x01 xXXXXXXXX]
    if(self->state == CIOT_NRF_DFU_STATE_CREATE_OBJECT)
    {
        CIOT_LOGD(TAG, "Create command");
        int32_t data_remaining = self->object.packet->size - self->data_transferred;
        self->object.remaining = data_remaining >= MAX_OBJECT_SIZE ? MAX_OBJECT_SIZE : data_remaining;
        self->data[0] = CIOT_NRF_DFU_OP_OBJECT_CREATE;
        self->data[1] = self->object.packet->type;
        self->data[2] = self->object.remaining;
        self->data[3] = self->object.remaining >> 8;
        self->data[4] = self->object.remaining >> 16;
        self->data[5] = self->object.remaining >> 24;
        self->state = CIOT_NRF_DFU_STATE_WAITING_CREATE_OBJ;
        ciot_nrf_dfu_timeout_check(TIMEOUT_RESET);
        return ciot_nrf_dfu_send_data(self, self->data, 6);
    }

    // Transfer data up to 64 or 20 bytes [xXX .. xXX]
    if(self->state == CIOT_NRF_DFU_STATE_WRITE_DFU_PACKAGE)
    {
        uint16_t bytes_to_write = self->object.remaining;
        bytes_to_write = bytes_to_write > self->cfg.max_package_len ? self->cfg.max_package_len : bytes_to_write;
        self->crc.expected =  crc32_compute(&self->object.packet->data[self->data_transferred], bytes_to_write, &self->crc.expected);
        // self->crc.expected = ciot_crc32_le(self->crc.expected, &self->object.packet->data[self->data_transferred], bytes_to_write);

        if(self->cfg.dfu.type != CIOT_DFU_TYPE_BLE)
        {
            uint8_t write_opcode[] = { CIOT_NRF_DFU_OP_OBJECT_WRITE };
            ciot_iface_send_data(self->cfg.iface, &write_opcode, 1);
            CIOT_LOGI(TAG, "Writing %d object [%d / %ld]", self->object.packet->type, self->data_transferred, self->object.packet->size);
        }

        ciot_err_t err = ciot_nrf_dfu_send_data(self, &self->object.packet->data[self->data_transferred], bytes_to_write);
        if(err != CIOT_OK)
        {
            self->state = CIOT_NRF_DFU_STATE_ERROR;
            return err;
        }

        self->data_transferred += bytes_to_write;
        self->object.remaining -= bytes_to_write;
        self->object.packet->transferred = self->data_transferred == self->object.packet->size;
        self->prn_counter++;

        if(self->prn_counter >= PKT_SET_PRN_PARAM_LEN || self->object.packet->transferred)
        {
            self->prn_counter = 0;
            self->state = CIOT_NRF_DFU_STATE_REQUEST_CRC;
            ciot_nrf_dfu_timeout_check(TIMEOUT_RESET);
        }

        return err;
    }

    // Calculate CRC [x03]
    if(self->state == CIOT_NRF_DFU_STATE_REQUEST_CRC)
    {
        CIOT_LOGD(TAG, "Calculate CRC");
        self->data[0] = CIOT_NRF_DFU_OP_CRC_GET;
        self->state = CIOT_NRF_DFU_STATE_WAITING_CRC;
        ciot_nrf_dfu_timeout_check(TIMEOUT_RESET);
        return ciot_nrf_dfu_send_data(self, self->data, 1);
    }

    // Execute command [0x04]
    if(self->state == CIOT_NRF_DFU_STATE_REQUEST_EXECUTE)
    {
        CIOT_LOGD(TAG, "Execute command");
        self->data[0] = CIOT_NRF_DFU_OP_OBJECT_EXECUTE;
        self->state = CIOT_NRF_DFU_STATE_WAITING_EXECUTE;
        ciot_nrf_dfu_timeout_check(TIMEOUT_RESET);
        return ciot_nrf_dfu_send_data(self, self->data, 1);
    }

    return ciot_nrf_dfu_timeout_check(CIOT_NRF_DFU_TIMEOUT_SEC);
}

static ciot_err_t ciot_nrf_dfu_process_data(ciot_dfu_t self, uint8_t *data, int32_t len)
{
    CIOT_NULL_CHECK(self);

    CIOT_LOG_HEX(TAG, data, len);

    self->status.code = data[2];

    // Ping Response Success [x60 x09 x01] or Opcode not supported [x60 x09 x03]
    if(self->state == CIOT_NRF_DFU_STATE_WAITING_PING_RESP &&
       data[0] == CIOT_NRF_DFU_OP_RESPONSE &&
       data[1] == CIOT_NRF_DFU_OP_PING && 
       data[2] == CIOT_NRF_DFU_RES_CODE_SUCCESS)
    {
        CIOT_LOGD(TAG, "Ping response sucess");
        self->state = CIOT_NRF_DFU_STATE_CREATE_OBJECT;
        ciot_nrf_dfu_set_state(self, CIOT_DFU_STATE_IN_PROGRESS);
        return CIOT_OK;
    }

    // Response Create Success [x60 x01 x01]
    if(self->state == CIOT_NRF_DFU_STATE_WAITING_CREATE_OBJ && 
       data[0] == CIOT_NRF_DFU_OP_RESPONSE &&
       data[1] == CIOT_NRF_DFU_OP_OBJECT_CREATE &&
       data[2] == CIOT_NRF_DFU_RES_CODE_SUCCESS)
    {
        CIOT_LOGD(TAG, "Response create success");
        self->state = CIOT_NRF_DFU_STATE_WRITE_DFU_PACKAGE;
        return CIOT_OK;
    }

    // Response Calculate CRC Sucess: offset CRC32 [x60 x03 x01 XXXXXXXX XXXXXXXX]
    // Response PRN Success: offset CRC32 [60 03 01 XXXXXXXX XXXXXXXX]
    if(self->state == CIOT_NRF_DFU_STATE_WAITING_CRC &&
       data[0] == CIOT_NRF_DFU_OP_RESPONSE &&
       data[1] == CIOT_NRF_DFU_OP_CRC_GET &&
       data[2] == CIOT_NRF_DFU_RES_CODE_SUCCESS)
    {
        self->crc.received = data[len-1] << 24;
        self->crc.received += data[len-2] << 16;
        self->crc.received += data[len-3] << 8;
        self->crc.received += data[len-4];
        if(self->crc.received != self->crc.expected)
        {
            CIOT_LOGE(TAG, "CRC error: expected = %lu, received = %lu", self->crc.expected, self->crc.received);
            self->state = CIOT_NRF_DFU_STATE_ERROR;
            ciot_nrf_dfu_set_state(self, CIOT_DFU_STATE_ERROR);
            return CIOT_ERR_CHECKSUM;
        }
        if(self->object.remaining == 0)
        {
            CIOT_LOGD(TAG, "Calculate CRC Sucess: offset CRC32");
            self->state = CIOT_NRF_DFU_STATE_REQUEST_EXECUTE;
        }
        else
        {
            CIOT_LOGD(TAG, "Response PRN Sucess: offset CRC32");
            self->state = CIOT_NRF_DFU_STATE_WRITE_DFU_PACKAGE;
        }
        return CIOT_OK;
    }

    // Response Execute Sucess [x60 x04 x01]
    if(self->state == CIOT_NRF_DFU_STATE_WAITING_EXECUTE &&
       data[0] == CIOT_NRF_DFU_OP_RESPONSE &&
       data[1] == CIOT_NRF_DFU_OP_OBJECT_EXECUTE && 
       data[2] == CIOT_NRF_DFU_RES_CODE_SUCCESS)
    {
        CIOT_LOGD(TAG, "Response Execute Success");
        if(self->object.packet->transferred)
        {
            if(self->object.packet->type == CIOT_NRF_DFU_PACKET_TYPE_INIT)
            {
                CIOT_LOGI(TAG, "Init packet write completed");
                self->object.packet = &self->cfg.app_image;
                self->data_transferred = 0;
                self->crc.expected = 0;
                self->state = CIOT_NRF_DFU_STATE_CREATE_OBJECT;
                return CIOT_OK;
            }
            if(self->object.packet->type == CIOT_NRF_DFU_PACKET_TYPE_APP_IMAGE)
            {
                CIOT_LOGI(TAG, "App image write completed");
                self->state = CIOT_NRF_DFU_STATE_COMPLETED;
                ciot_nrf_dfu_set_state(self, CIOT_DFU_STATE_COMPLETED);
                ciot_nrf_dfu_stop(self);
                return CIOT_OK;
            }
        }
        else
        {
            CIOT_LOGI(TAG, "Object write completed");
            self->state = CIOT_NRF_DFU_STATE_CREATE_OBJECT;
            return CIOT_OK;
        }
    }

    // DFU completed... nothing to do...
    if(self->state == CIOT_NRF_DFU_STATE_COMPLETED)
    {
        return CIOT_OK;
    }

    CIOT_LOGE(TAG, "DFU failed at state %d with response code 0x%x", self->state, data[2]);
    if(data[2] == CIOT_NRF_DFU_RES_CODE_EXT_ERROR)
    {
        CIOT_LOGE(TAG, "DFU extended error code: 0x%x", data[3]);
        self->status.error = data[3];
    }
    if(self->state != CIOT_NRF_DFU_STATE_ERROR)
    {
        ciot_nrf_dfu_set_state(self, CIOT_DFU_STATE_ERROR);
    }
    self->state = CIOT_NRF_DFU_STATE_ERROR;

    return CIOT_FAIL;
}

static ciot_err_t ciot_nrf_dfu_set_state(ciot_dfu_t self, ciot_dfu_state_t state)
{
    self->status.state = state;
    if(state == CIOT_DFU_STATE_ERROR)
    {
        ciot_nrf_dfu_stop(self);
    }
    if(self->iface.event_handler != NULL)
    {
        ciot_iface_event_t status_event = { 0 };
        ciot_dfu_status_msg_t status_msg = { 0 };
        status_msg.status = self->status;
        status_msg.header.iface = self->iface.info;
        status_msg.header.type = CIOT_MSG_TYPE_GET_STATUS;
        status_event.type = CIOT_DFU_EVENT_STATE_CHANGED;
        status_event.data = (ciot_iface_event_data_u*)&status_msg;
        status_event.size = sizeof(status_msg);
        return self->iface.event_handler(&self->iface, &status_event, self->iface.event_args);
    }
    return CIOT_OK;
}

static ciot_err_t ciot_nrf_dfu_event_handler(ciot_iface_t *sender, ciot_iface_event_t *event, void *args)
{
    ciot_dfu_t self = (ciot_dfu_t)args;

    if(event->type == CIOT_IFACE_EVENT_DATA)
    {
        if(sender->info.type == CIOT_IFACE_TYPE_UART)
        {
            for (size_t i = 0; i < event->size; i++)
            {
                if(ciot_slip_decode_add_byte(&self->slip.handle, event->data->payload[i]) == CIOT_OK)
                {
                    ciot_nrf_dfu_process_data(self, self->slip.buffer, self->slip.handle.current_index);
                    self->slip.handle.current_index = 0;
                }
            }
        }
        else
        {
            ciot_nrf_dfu_process_data(self, event->data->payload, event->size);
        }
    }

    return CIOT_OK;
}

static ciot_err_t ciot_nrf_dfu_slip_encode_and_send(ciot_dfu_t self, uint8_t *data, uint32_t len)
{
    uint8_t encoded_slip_packet[CIOT_NRF_DFU_MAX_DFU_PKT_LEN_UART] = {0};
    uint32_t encoded_slip_packet_len;
    ciot_slip_encode(encoded_slip_packet, data, len, &encoded_slip_packet_len);
    return ciot_iface_send_data(self->cfg.iface, encoded_slip_packet, encoded_slip_packet_len);
}

static uint32_t ciot_nrf_dfu_timeout_check(uint32_t timeout)
{
    return 0;
}
