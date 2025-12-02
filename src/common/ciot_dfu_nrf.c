/**
 * @file ciot_dfu_nrf.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-06-07
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "ciot_config.h"

#if CIOT_CONFIG_FEATURE_DFU == 1

#include <stdio.h>
#include <stdlib.h>
#include "ciot_types.h"
#include "ciot_dfu_nrf.h"
#include "ciot_config.h"
#include "ciot_sys.h"
#include "ciot_utils.h"
#include "ciot_timer.h"
#include "ciot_decoder.h"
#include "ciot.h"

static const char *TAG = "ciot_dfu_nrf";

#define MAX_OBJECT_SIZE 4096
#define PING_ID 0x01
#define TIMEOUT_RESET 2
#define PKT_SET_PRN_PARAM_LEN (32)

typedef struct ciot_dfu_nrf_object
{
    uint32_t size;
    uint32_t remaining;
    ciot_dfu_nrf_packet_t *packet;
} ciot_dfu_nrf_object_t;

typedef struct ciot_dfu_nrf_crc
{
    uint32_t expected;
    uint32_t received;
} ciot_dfu_nrf_crc_t;

struct ciot_dfu_nrf
{
    ciot_dfu_t base;
    ciot_dfu_nrf_cfg_t cfg;

    uint8_t decoder_buf[256];
    ciot_decoder_t decoder;

    ciot_dfu_nrf_state_t state;
    ciot_dfu_nrf_state_t last_state;
    ciot_dfu_nrf_crc_t crc;
    uint8_t data[CIOT_DFU_NRF_MAX_DFU_PKT_LEN_UART];
    uint32_t prn_counter;
    uint32_t data_transferred;
    ciot_dfu_nrf_object_t object;
    bool ping_refused;

    uint64_t timer;
};

static ciot_err_t ciot_dfu_nrf_process_data(ciot_iface_t *iface, ciot_msg_data_t *data);
static ciot_err_t ciot_dfu_nrf_get_data(ciot_iface_t *iface, ciot_msg_data_t *data);
static ciot_err_t ciot_dfu_nrf_send_data(ciot_iface_t *iface, uint8_t *data, int size);

static ciot_err_t ciot_dfu_nrf_set_state(ciot_dfu_nrf_t self, ciot_dfu_state_t state);
static ciot_err_t ciot_dfu_nrf_event_handler(ciot_iface_t *sender, ciot_event_t *event, void *args);
static ciot_err_t ciot_dfu_nrf_write(ciot_dfu_nrf_t self);
static ciot_err_t ciot_dfu_nrf_process_response(ciot_dfu_nrf_t self, uint8_t *data, int size);

ciot_dfu_nrf_t ciot_dfu_nrf_new(ciot_dfu_nrf_cfg_t *cfg)
{
    ciot_dfu_nrf_t self = calloc(1, sizeof(struct ciot_dfu_nrf));
    ciot_dfu_nrf_init(self);
    self->prn_counter = 0;
    self->cfg = *cfg;
    self->cfg.init_packet.type = CIOT_DFU_NRF_PACKET_TYPE_INIT;
    self->cfg.app_image.type = CIOT_DFU_NRF_PACKET_TYPE_APP_IMAGE;
    self->object.packet = &self->cfg.init_packet;
    self->decoder = ciot_decoder_slip_new(self->decoder_buf, sizeof(self->decoder_buf));
    // self->decoder->event_type = CIOT_IFACE_EVENT_DATA;
    return self;
}

ciot_err_t ciot_dfu_nrf_start(ciot_dfu_nrf_t self, ciot_dfu_cfg_t *cfg)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_STATE_CHECK(self->base.status.state, CIOT_DFU_STATE_IDLE);

    if (cfg->type != self->cfg.dfu.type)
    {
        return CIOT_ERR_INVALID_TYPE;
    }

    self->state = CIOT_DFU_NRF_STATE_IDLE;
    ciot_dfu_nrf_set_state(self, CIOT_DFU_STATE_STARTING);
    ciot_iface_set_decoder(self->cfg.iface, self->decoder);
    ciot_iface_set_event_handler(self->cfg.iface, ciot_dfu_nrf_event_handler, self);

    self->base.status.error = self->cfg.iface->process_data(self->cfg.iface, self->cfg.iface_cfg);

    return self->base.status.error;
}

ciot_err_t ciot_dfu_nrf_stop(ciot_dfu_nrf_t self)
{
    CIOT_ERR_NULL_CHECK(self);
    ciot_iface_set_event_handler(self->cfg.iface, NULL, NULL);
    self->cfg.iface->req_status.state = CIOT_IFACE_REQ_STATE_IDLE;
    self->prn_counter = 0;
    self->data_transferred = 0;
    self->crc.expected = 0;
    self->crc.received = 0;
    self->object.packet = &self->cfg.init_packet;
    self->state = CIOT_DFU_NRF_STATE_IDLE;
    self->last_state = CIOT_DFU_NRF_STATE_IDLE;
    self->base.status.state = CIOT_DFU_STATE_IDLE;
    ciot_msg_data_t data = {
        .which_type = self->cfg.iface_cfg->which_type,
        .common.which_type = CIOT_COMMON_STOP_TAG,
        .common.stop = true,
    };
    self->cfg.iface->process_data(self->cfg.iface, &data);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_dfu_nrf_init(ciot_dfu_nrf_t self)
{
    ciot_dfu_t *base = (ciot_dfu_t *)&self->base;
    base->iface.ptr = self;
    base->iface.process_data = ciot_dfu_nrf_process_data;
    base->iface.get_data = ciot_dfu_nrf_get_data;
    base->iface.send_data = ciot_dfu_nrf_send_data;
    base->iface.info.type = CIOT_IFACE_TYPE_IFACE_TYPE_DFU;
    return CIOT_ERR_OK;
}

static ciot_err_t ciot_dfu_nrf_process_data(ciot_iface_t *iface, ciot_msg_data_t *data)
{
    CIOT_ERR_TYPE_CHECK(data->which_type, CIOT_MSG_DATA_DFU_TAG);

    ciot_dfu_nrf_t self = iface->ptr;
    ciot_dfu_data_t *dfu = &data->dfu;

    switch (dfu->which_type)
    {
    case CIOT_DFU_DATA_STOP_TAG:
        return ciot_dfu_nrf_stop(self);
    case CIOT_DFU_DATA_CONFIG_TAG:
        return ciot_dfu_nrf_start(self, &dfu->config);
    case CIOT_DFU_DATA_REQUEST_TAG:
        return ciot_dfu_nrf_process_req(self, &dfu->request);
    default:
        break;
    }

    return CIOT_ERR_NOT_IMPLEMENTED;
}

static ciot_err_t ciot_dfu_nrf_get_data(ciot_iface_t *iface, ciot_msg_data_t *data)
{
    CIOT_ERR_TYPE_CHECK(data->which_type, CIOT_MSG_DATA_DFU_TAG);

    ciot_dfu_nrf_t self = iface->ptr;
    ciot_data_type_t data_type = data->get_data.type;
    data->which_type = CIOT_MSG_DATA_DFU_TAG;

    switch (data_type)
    {
    case CIOT_DATA_TYPE_CONFIG:
        data->dfu.which_type = CIOT_DFU_DATA_CONFIG_TAG;
        data->dfu.config = self->base.cfg;
        break;
    case CIOT_DATA_TYPE_STATUS:
        data->dfu.which_type = CIOT_DFU_DATA_STATUS_TAG;
        data->dfu.status = self->base.status;
        break;
    default:
        return CIOT_ERR_NOT_FOUND;
    }

    return CIOT_ERR_OK;
}

static ciot_err_t ciot_dfu_nrf_send_data(ciot_iface_t *iface, uint8_t *data, int size)
{
    CIOT_ERR_NULL_CHECK(iface);
    CIOT_ERR_NULL_CHECK(data);
    ciot_dfu_nrf_t self = (ciot_dfu_nrf_t)iface;
    return self->decoder->send(self->decoder, self->cfg.iface, data, size);
}

ciot_err_t ciot_dfu_nrf_process_req(ciot_dfu_nrf_t self, ciot_dfu_req_t *req)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(req);
    return CIOT_ERR_NOT_IMPLEMENTED;
}

ciot_err_t ciot_dfu_nrf_get_cfg(ciot_dfu_nrf_t self, ciot_dfu_cfg_t *cfg)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(cfg);
    ciot_dfu_t *base = (ciot_dfu_t *)self;
    *cfg = base->cfg;
    return CIOT_ERR_OK;
}

ciot_err_t ciot_dfu_nrf_get_status(ciot_dfu_nrf_t self, ciot_dfu_status_t *status)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(status);
    ciot_dfu_t *base = (ciot_dfu_t *)self;
    *status = base->status;
    return CIOT_ERR_OK;
}

ciot_err_t ciot_dfu_nrf_task(ciot_dfu_nrf_t self)
{
    CIOT_ERR_NULL_CHECK(self);

    if (self->state == CIOT_DFU_NRF_STATE_IDLE || self->state == CIOT_DFU_NRF_STATE_COMPLETED)
    {
        return CIOT_ERR_OK;
    }

    ciot_err_t err = ciot_dfu_nrf_write(self);
    if (err != CIOT_ERR_OK)
    {
        self->base.status.error = err;
        self->state = CIOT_DFU_NRF_STATE_ERROR;
        ciot_dfu_nrf_set_state(self, CIOT_DFU_STATE_ERROR);
    }

    if (self->last_state == self->state)
    {
        if(ciot_timer_compare(&self->timer, 5))
        {
            CIOT_LOGE(TAG, "TIMEOUT");
            err = CIOT_ERR_TIMEOUT;
            self->base.status.error = err;
            self->state = CIOT_DFU_NRF_STATE_ERROR;
            ciot_dfu_nrf_set_state(self, CIOT_DFU_STATE_ERROR);
        }
    }
    else
    {
        self->last_state = self->state;
        ciot_timer_init(&self->timer, 5);
    }

    return err;
}

ciot_err_t ciot_dfu_nrf_read_file(ciot_dfu_nrf_packet_t *object, const char *name)
{
    FILE *file = fopen(name, "rb");

    if (file == NULL)
    {
        CIOT_LOGE(TAG, "Error opening file %s", name);
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
    if (elements_read != file_size)
    {
        CIOT_LOGE(TAG, "Error reading file");
        return CIOT_ERR_FAIL;
    }

    return 0;
}

ciot_err_t ciot_dfu_nrf_start_bootloader(ciot_dfu_nrf_t self, ciot_iface_t *iface, int sys_id)
{
    CIOT_LOGI(TAG, "Starting bootloader");
    ciot_msg_t req = {
        .has_iface = true,
        .iface.type = CIOT_IFACE_TYPE_SYS,
        .iface.id = sys_id,
        .has_data = true,
        .data.which_type = CIOT_MSG_DATA_SYS_TAG,
        .data.sys.which_type = CIOT_SYS_DATA_REQUEST_TAG,
        .data.sys.request.which_type = CIOT_SYS_REQ_CMD_TAG,
        .data.sys.request.cmd = CIOT_SYS_REQ_CMD_INIT_DFU,
    };
    ciot_err_t err = ciot_iface_send_req(iface, &req);
    if(err != CIOT_ERR_OK)
    {
        self->base.status.error = err;
        self->state = CIOT_DFU_NRF_STATE_ERROR;
        ciot_dfu_nrf_set_state(self, CIOT_DFU_STATE_ERROR);
        CIOT_LOGE(TAG, "Start bootloader error: %s", ciot_err_to_message(err));
    }
    return err;
}

ciot_dfu_nrf_state_t ciot_dfu_nrf_state(ciot_dfu_nrf_t self)
{
    return self->state;
}

static ciot_err_t ciot_dfu_nrf_write(ciot_dfu_nrf_t self)
{
    CIOT_ERR_NULL_CHECK(self);

    ciot_dfu_t *base = &self->base;

    // Send Ping [x09 x01]
    if (self->state == CIOT_DFU_NRF_STATE_SEND_PING)
    {
        CIOT_LOGI(TAG, "Send ping");
        self->data[0] = CIOT_DFU_NRF_OP_PING;
        self->data[1] = PING_ID;
        self->state = CIOT_DFU_NRF_STATE_WAITING_PING_RESP;
        return ciot_dfu_nrf_send_data(&base->iface, self->data, 2);
    }

    // Create command: size [x01 x01 xXXXXXXXX]
    if (self->state == CIOT_DFU_NRF_STATE_CREATE_OBJECT)
    {
        CIOT_LOGI(TAG, "Create command");
        int32_t data_remaining = self->object.packet->size - self->data_transferred;
        self->object.remaining = data_remaining >= MAX_OBJECT_SIZE ? MAX_OBJECT_SIZE : data_remaining;
        self->data[0] = CIOT_DFU_NRF_OP_OBJECT_CREATE;
        self->data[1] = self->object.packet->type;
        self->data[2] = self->object.remaining;
        self->data[3] = self->object.remaining >> 8;
        self->data[4] = self->object.remaining >> 16;
        self->data[5] = self->object.remaining >> 24;
        self->state = CIOT_DFU_NRF_STATE_WAITING_CREATE_OBJ;
        return ciot_dfu_nrf_send_data(&base->iface, self->data, 6);
    }

    // Transfer data up to 64 or 20 bytes [xXX .. xXX]
    if (self->state == CIOT_DFU_NRF_STATE_WRITE_DFU_PACKAGE)
    {
        uint32_t bytes_to_write = self->object.remaining;
        bytes_to_write = bytes_to_write > self->cfg.max_package_len ? self->cfg.max_package_len : bytes_to_write;
        self->crc.expected = crc32_compute(&self->object.packet->data[self->data_transferred], bytes_to_write, &self->crc.expected);
        // self->crc.expected = ciot_crc32_le(self->crc.expected, &self->object.packet->data[self->data_transferred], bytes_to_write);

        if (self->cfg.dfu.type != CIOT_DFU_TYPE_BLE)
        {
            uint8_t write_opcode[] = {CIOT_DFU_NRF_OP_OBJECT_WRITE};
            // ciot_dfu_nrf_send_data(&base->iface, write_opcode, 1);
            self->cfg.iface->send_data(self->cfg.iface, write_opcode, 1);
            self->base.status.image_size = self->object.packet->size,
            self->base.status.image_written = self->data_transferred;
            // CIOT_LOGI(TAG, "Item %d writing %d of object [%ld / %ld]", self->object.packet->type, bytes_to_write, self->data_transferred, self->object.packet->size);
        }

        ciot_err_t err = ciot_dfu_nrf_send_data(&base->iface, &self->object.packet->data[self->data_transferred], bytes_to_write);
        if (err != CIOT_ERR_OK)
        {
            CIOT_LOGE(TAG, "Error sending data (0x%x)", err);
            self->base.status.error = err;
            self->state = CIOT_DFU_NRF_STATE_ERROR;
            ciot_dfu_nrf_set_state(self, CIOT_DFU_STATE_ERROR);
        }

        self->data_transferred += bytes_to_write;
        self->object.remaining -= bytes_to_write;
        self->object.packet->transferred = self->data_transferred == self->object.packet->size;
        self->prn_counter++;

        if (self->prn_counter >= PKT_SET_PRN_PARAM_LEN || self->object.packet->transferred)
        {
            self->prn_counter = 0;
            self->state = CIOT_DFU_NRF_STATE_REQUEST_CRC;
        }

        return err;
    }

    // Calculate CRC [x03]
    if (self->state == CIOT_DFU_NRF_STATE_REQUEST_CRC)
    {
        CIOT_LOGI(TAG, "Request CRC");
        self->data[0] = CIOT_DFU_NRF_OP_CRC_GET;
        self->state = CIOT_DFU_NRF_STATE_WAITING_CRC;
        return ciot_dfu_nrf_send_data(&base->iface, self->data, 1);
    }

    // Execute command [0x04]
    if (self->state == CIOT_DFU_NRF_STATE_REQUEST_EXECUTE)
    {
        CIOT_LOGI(TAG, "Execute command");
        self->data[0] = CIOT_DFU_NRF_OP_OBJECT_EXECUTE;
        self->state = CIOT_DFU_NRF_STATE_WAITING_EXECUTE;
        return ciot_dfu_nrf_send_data(&base->iface, self->data, 1);
    }

    return CIOT_ERR_OK;
}

static ciot_err_t ciot_dfu_nrf_process_response(ciot_dfu_nrf_t self, uint8_t *data, int len)
{
    CIOT_ERR_NULL_CHECK(self);

    ciot_dfu_t *base = &self->base;
    // CIOT_LOG_BUFFER_HEX(TAG, data, len);

    base->status.code = data[2];

    for (size_t i = 0; i < len; i++)
    {
        if (data[i] == CIOT_DFU_NRF_OP_RESPONSE)
        {
            data = &data[i];
            break;
        }
        CIOT_LOGD(TAG, "Invalid initial byte: %0x", data[i]);
    }

    // Invlalid message
    if (data[0] != CIOT_DFU_NRF_OP_RESPONSE)
    {
        CIOT_LOGE(TAG, "Invalid msg");
        return CIOT_ERR_VALIDATION_FAILED;
    }

    // Ping Response Success [x60 x09 x01] or Opcode not supported [x60 x09 x03]
    if (self->state == CIOT_DFU_NRF_STATE_WAITING_PING_RESP &&
        data[0] == CIOT_DFU_NRF_OP_RESPONSE &&
        data[1] == CIOT_DFU_NRF_OP_PING &&
        data[2] == CIOT_DFU_NRF_RES_CODE_SUCCESS)
    {
        CIOT_LOGI(TAG, "Ping response sucess");
        self->state = CIOT_DFU_NRF_STATE_CREATE_OBJECT;
        ciot_dfu_nrf_set_state(self, CIOT_DFU_STATE_IN_PROGRESS);
        return CIOT_ERR_OK;
    }

    if (self->state == CIOT_DFU_NRF_STATE_WAITING_PING_RESP &&
        data[0] == CIOT_DFU_NRF_OP_RESPONSE &&
        data[2] == CIOT_DFU_NRF_RES_CODE_OP_CODE_NOT_SUPPORTED &&
        !self->ping_refused)
    {
        CIOT_LOGI(TAG, "First ping refused. Trying again...");
        self->state = CIOT_DFU_NRF_STATE_SEND_PING;
        self->ping_refused = true;
        return CIOT_ERR_OK;
    }

    // Response Create Success [x60 x01 x01]
    if (self->state == CIOT_DFU_NRF_STATE_WAITING_CREATE_OBJ &&
        data[0] == CIOT_DFU_NRF_OP_RESPONSE &&
        data[1] == CIOT_DFU_NRF_OP_OBJECT_CREATE &&
        data[2] == CIOT_DFU_NRF_RES_CODE_SUCCESS)
    {
        CIOT_LOGI(TAG, "Response create success");
        self->state = CIOT_DFU_NRF_STATE_WRITE_DFU_PACKAGE;
        return CIOT_ERR_OK;
    }

    // Response Calculate CRC Sucess: offset CRC32 [x60 x03 x01 XXXXXXXX XXXXXXXX]
    // Response PRN Success: offset CRC32 [60 03 01 XXXXXXXX XXXXXXXX]
    if (self->state == CIOT_DFU_NRF_STATE_WAITING_CRC &&
        data[0] == CIOT_DFU_NRF_OP_RESPONSE &&
        data[1] == CIOT_DFU_NRF_OP_CRC_GET &&
        data[2] == CIOT_DFU_NRF_RES_CODE_SUCCESS)
    {
        CIOT_LOGI(TAG, "CRC received");
        self->crc.received = data[len - 1] << 24;
        self->crc.received += data[len - 2] << 16;
        self->crc.received += data[len - 3] << 8;
        self->crc.received += data[len - 4];
        if (self->crc.received != self->crc.expected)
        {
            CIOT_LOGI(TAG, "CRC error: expected = %d, received = %d", (int)self->crc.expected, (int)self->crc.received);
            self->state = CIOT_DFU_NRF_STATE_ERROR;
            ciot_dfu_nrf_set_state(self, CIOT_DFU_STATE_ERROR);
            return CIOT_ERR_CHECKSUM;
        }
        if (self->object.remaining == 0)
        {
            CIOT_LOGI(TAG, "Calculate CRC Sucess: offset CRC32");
            self->state = CIOT_DFU_NRF_STATE_REQUEST_EXECUTE;
        }
        else
        {
            CIOT_LOGI(TAG, "Response PRN Sucess: offset CRC32");
            self->state = CIOT_DFU_NRF_STATE_WRITE_DFU_PACKAGE;
        }
        return CIOT_ERR_OK;
    }

    // Response Execute Sucess [x60 x04 x01]
    if (self->state == CIOT_DFU_NRF_STATE_WAITING_EXECUTE &&
        data[0] == CIOT_DFU_NRF_OP_RESPONSE &&
        data[1] == CIOT_DFU_NRF_OP_OBJECT_EXECUTE &&
        data[2] == CIOT_DFU_NRF_RES_CODE_SUCCESS)
    {
        CIOT_LOGI(TAG, "Response Execute Success");
        if (self->object.packet->transferred)
        {
            if (self->object.packet->type == CIOT_DFU_NRF_PACKET_TYPE_INIT)
            {
                CIOT_LOGI(TAG, "Init packet write completed");
                self->object.packet = &self->cfg.app_image;
                self->data_transferred = 0;
                self->crc.expected = 0;
                self->state = CIOT_DFU_NRF_STATE_CREATE_OBJECT;
                return CIOT_ERR_OK;
            }
            if (self->object.packet->type == CIOT_DFU_NRF_PACKET_TYPE_APP_IMAGE)
            {
                CIOT_LOGI(TAG, "App image write completed");
                self->state = CIOT_DFU_NRF_STATE_COMPLETED;
                ciot_dfu_nrf_set_state(self, CIOT_DFU_STATE_COMPLETED);
                ciot_dfu_nrf_stop(self);
                return CIOT_ERR_OK;
            }
        }
        else
        {
            CIOT_LOGI(TAG, "Object write completed");
            self->state = CIOT_DFU_NRF_STATE_CREATE_OBJECT;
            return CIOT_ERR_OK;
        }
    }

    // DFU completed... nothing to do...
    if (self->state == CIOT_DFU_NRF_STATE_COMPLETED)
    {
        return CIOT_ERR_OK;
    }

    if (data[2] != CIOT_DFU_NRF_RES_CODE_SUCCESS)
    {
        CIOT_LOGE(TAG, "DFU failed at state %d with response code 0x%x", self->state, data[2]);
        if (data[2] == CIOT_DFU_NRF_RES_CODE_EXT_ERROR)
        {
            CIOT_LOGE(TAG, "DFU extended error code: 0x%x", data[3]);
            base->status.error = data[3];
        }
        if (self->state != CIOT_DFU_NRF_STATE_ERROR)
        {
            ciot_dfu_nrf_stop(self);
            ciot_dfu_nrf_set_state(self, CIOT_DFU_STATE_ERROR);
        }
        self->state = CIOT_DFU_NRF_STATE_ERROR;

        return CIOT_ERR_FAIL;
    }

    return CIOT_ERR_OK;
}

static ciot_err_t ciot_dfu_nrf_set_state(ciot_dfu_nrf_t self, ciot_dfu_state_t state)
{
    ciot_dfu_t *base = &self->base;
    base->status.state = state;
    if (base->iface.event_handler != NULL)
    {
        ciot_iface_send_event_type(&base->iface, CIOT_EVENT_TYPE_INTERNAL);
    }
    return CIOT_ERR_OK;
}

static ciot_err_t ciot_dfu_nrf_event_handler(ciot_iface_t *sender, ciot_event_t *event, void *args)
{
    CIOT_LOGD(TAG, "Event received %s", ciot_event_to_str(event));

    ciot_dfu_nrf_t self = (ciot_dfu_nrf_t)args;

    if (event->type == CIOT_EVENT_TYPE_STARTED)
    {
        CIOT_LOGI(TAG, "DFU interface started");
        ciot_dfu_nrf_set_state(self, CIOT_DFU_STATE_STARTED);
        self->state = CIOT_DFU_NRF_STATE_SEND_PING;
    }

    if (event->type == CIOT_EVENT_TYPE_REQUEST)
    {
        ciot_err_t err = ciot_dfu_nrf_process_response(self, event->raw.bytes, event->raw.size);
        if (err != CIOT_ERR_OK)
        {
            self->base.status.error = err;
            self->state = CIOT_DFU_NRF_STATE_ERROR;
            ciot_dfu_nrf_set_state(self, CIOT_DFU_STATE_ERROR);
        }
    }

    return CIOT_ERR_OK;
}

#endif // CIOT_CONFIG_FEATURE_DFU == 1
