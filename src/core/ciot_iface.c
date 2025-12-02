/**
 * @file ciot_iface.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-27
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "ciot_types.h"
#include "ciot_iface.h"
#include "ciot_log.h"
#include "ciot_decoder.h"

static const char *TAG = "ciot_iface";

static ciot_err_t ciot_iface_process_data_result(ciot_iface_t *self, ciot_iface_t *sender, ciot_msg_t *msg, ciot_err_t error);
static ciot_err_t ciot_iface_send(ciot_iface_t *self, ciot_msg_t *msg);

static uint32_t msg_id = 0;

ciot_err_t ciot_iface_send_msg(ciot_iface_t *self, ciot_msg_t *msg)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(msg);
    return ciot_iface_send(self, msg);
}

ciot_err_t ciot_iface_send_rsp(ciot_iface_t *self, ciot_msg_t *rsp)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(rsp);

    if (self->req_status.state != CIOT_IFACE_REQ_STATE_IDLE)
    {
        CIOT_LOGE(TAG, "Iface %s (%lu) is busy", ciot_iface_to_str(self), (long unsigned int)self->info.id);
        return CIOT_ERR_BUSY;
    }
    else
    {
        return ciot_iface_send(self, rsp);
    }
}

ciot_err_t ciot_iface_send_req(ciot_iface_t *self, ciot_msg_t *req)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(req);

    if (self->req_status.state != CIOT_IFACE_REQ_STATE_IDLE &&
        req->data.which_type != self->req_status.data_type &&
        !ciot_iface_is_equal(&req->iface, &self->req_status.iface))
    {
        CIOT_LOGW(TAG, "Overwriting last request on iface %s (%lu)", ciot_iface_to_str(self), (long unsigned int)self->info.id);
    }

    req->id = ciot_iface_get_msg_id();
    ciot_iface_register_req(self, &req->iface, req, CIOT_IFACE_REQ_STATE_SENDED);
    
    ciot_err_t err = ciot_iface_send(self, req);
    if(err != CIOT_ERR_OK)
    {
        self->req_status.state = CIOT_IFACE_REQ_STATE_IDLE;
    }
    return err;
}

ciot_err_t ciot_iface_send_error(ciot_iface_t *self, ciot_iface_type_t iface_type, uint16_t iface_id, ciot_msg_t *msg, ciot_err_t err)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(msg);
    msg->iface.type = iface_type;
    msg->iface.id = iface_id;
    msg->error = err;
    self->req_status.state = CIOT_IFACE_REQ_STATE_IDLE;
    return ciot_iface_send_rsp(self, msg);
}

ciot_err_t ciot_iface_send_event(ciot_iface_t *self, ciot_event_t *event)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(event);
    CIOT_ERR_NULL_CHECK(self->event_handler);
    return self->event_handler(self, event, self->event_args);
}

ciot_err_t ciot_iface_send_event_type(ciot_iface_t *self, ciot_event_type_t event_type)
{
    static ciot_event_t event = { 0 };
    event.type = event_type;
    event.which_data = CIOT_EVENT_MSG_TAG;
    event.msg.has_iface = true;
    event.msg.iface = self->info;
    event.msg.has_data = true;
    event.msg.data.which_type = CIOT_MSG_DATA_GET_DATA_TAG;
    event.msg.data.get_data.type = CIOT_DATA_TYPE_STATUS;
    CIOT_ERR_RETURN(self->get_data(self, &event.msg.data));
    CIOT_LOGD(TAG, "Sending event from iface: %s", ciot_iface_to_str(self));
    return ciot_iface_send_event(self, &event);
}

ciot_err_t ciot_iface_process_msg(ciot_iface_t *self, ciot_msg_t *msg, ciot_iface_t *sender)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(msg);

    switch (msg->data.which_type)
    {
    case CIOT_MSG_DATA_GET_DATA_TAG:
        self->req_status.state = CIOT_IFACE_REQ_STATE_IDLE;
        msg->error = self->get_data(self, &msg->data);
        msg->has_data = msg->data.which_type != 0;
        msg->error = ciot_iface_process_data_result(self, sender, msg, msg->error);          
        break;
    // case CIOT_MSG_RAW_TAG:
    //     msg->error = self->send_data(self, msg->raw.bytes, msg->raw.size);
    //     msg->error = ciot_iface_process_data_result(self, sender, msg, msg->error);
    //     break;
    default:
        ciot_iface_register_req(self, &sender->info, msg, CIOT_IFACE_REQ_STATE_RECEIVED);
        msg->error = self->process_data(self, &msg->data);
        msg->error = ciot_iface_process_data_result(self, sender, msg, msg->error);
        break;
    }

    return msg->error;
}

ciot_err_t ciot_iface_process_data(ciot_iface_t *self, uint8_t *data, int size, ciot_event_type_t event_type)
{
    if(self->decoder != NULL)
    {
        for (size_t i = 0; i < size; i++)
        {
            CIOT_ERR_RETURN(self->decoder->decode(self->decoder, data[i]));
            if(self->decoder->state == CIOT_DECODER_STATE_DONE)
            {
                ciot_event_t event = { 0 };
                if(self->decoder->result.size >= sizeof(event.raw)) return CIOT_ERR_SMALL_RAW;
                event.type = event_type;
                event.raw.size = self->decoder->result.size;
                memcpy(event.raw.bytes, self->decoder->result.buf, self->decoder->result.size);
                return ciot_iface_send_event(self, &event);
            }
        }
        return CIOT_ERR_OK;
    }
    else
    {
        ciot_event_t event = { 0 };
        if(size >= sizeof(event.raw)) return CIOT_ERR_SMALL_RAW;
        event.type = event_type;
        event.raw.size = size;
        memcpy(event.raw.bytes, data, size);
        return ciot_iface_send_event(self, &event);
    }
}

ciot_err_t ciot_iface_register_req(ciot_iface_t *self, ciot_iface_info_t *iface, ciot_msg_t *msg, ciot_iface_req_state_t state)
{
    CIOT_ERR_NULL_CHECK(self);
    self->req_status.state = state;
    self->req_status.id = msg->id;
    self->req_status.data_type = msg->data.which_type;
    self->req_status.iface = *iface;
    return CIOT_ERR_OK;
}

ciot_err_t ciot_iface_set_event_handler(ciot_iface_t *self, ciot_iface_event_handler_fn event_handler, void *event_args)
{
    CIOT_ERR_NULL_CHECK(self);
    self->event_handler = event_handler;
    self->event_args = event_args;
    return CIOT_ERR_OK;
}

ciot_err_t ciot_iface_set_serializer(ciot_iface_t *self, ciot_serializer_t serializer)
{
    CIOT_ERR_NULL_CHECK(self);
    self->serializer = serializer;
    return CIOT_ERR_OK;
}

ciot_err_t ciot_iface_set_decoder(ciot_iface_t *self, ciot_decoder_t decoder)
{
    CIOT_ERR_NULL_CHECK(self);
    self->decoder = decoder;
    return CIOT_ERR_OK;
}

ciot_err_t ciot_iface_get_msg_id(void)
{
    return msg_id++;
}

int ciot_iface_send_bytes(ciot_iface_t *self, uint8_t *bytes, int size)
{
    if(self == NULL) return 0;
    if(bytes == NULL) return 0;
    if(self->send_data == NULL) return 0;
    if(self->send_data(self, bytes, size) == CIOT_ERR_OK) return size;
    return 0;
}

int ciot_iface_read_bytes(ciot_iface_t *self, uint8_t *bytes, int size)
{
    if(self == NULL) return 0;
    if(bytes == NULL) return 0;
    if(self->read_data == NULL) return 0;
    if(self->read_data(self, bytes, size) == CIOT_ERR_OK) return size;
    return 0;
}

const char* ciot_iface_to_str(ciot_iface_t *self)
{
    if(self == NULL) return "NULL";
    return ciot_iface_type_to_str(self->info.type);
}

const char* ciot_iface_type_to_str(ciot_iface_type_t iface_type)
{
    switch (iface_type)
    {
        case CIOT_IFACE_TYPE_UNDEFINED:
            return "UNDEFINED";
        case CIOT_IFACE_TYPE_CUSTOM:
            return "CUSTOM";
        case CIOT_IFACE_TYPE_CIOT:
            return "CIOT";
        case CIOT_IFACE_TYPE_STORAGE:
            return "STORAGE";
        case CIOT_IFACE_TYPE_SYS:
            return "SYS";
        case CIOT_IFACE_TYPE_UART:
            return "UART";
        case CIOT_IFACE_TYPE_USB:
            return "USB";
        case CIOT_IFACE_TYPE_TCP:
            return "TCP";
        case CIOT_IFACE_TYPE_ETH:
            return "ETH";
        case CIOT_IFACE_TYPE_WIFI:
            return "WIFI";
        case CIOT_IFACE_TYPE_BLE:
            return "BLE";
        case CIOT_IFACE_TYPE_BLE_SCN:
            return "BLE_SCN";
        case CIOT_IFACE_TYPE_BLE_ADV:
            return "BLE_ADV";
        case CIOT_IFACE_TYPE_GPIO:
            return "GPIO";
        case CIOT_IFACE_TYPE_NTP:
            return "NTP";
        case CIOT_IFACE_TYPE_OTA:
            return "OTA";
        case CIOT_IFACE_TYPE_DFU:
            return "DFU";
        case CIOT_IFACE_TYPE_HTTP_CLIENT:
            return "HTTP_CLIENT";
        case CIOT_IFACE_TYPE_HTTP_SERVER:
            return "HTTP_SERVER";
        case CIOT_IFACE_TYPE_MQTT_CLIENT:
            return "MQTT_CLIENT";
        case CIOT_IFACE_TYPE_SOCKET:
            return "SOCKET";
        case CIOT_IFACE_TYPE_BRIDGE:
            return "BRIDGE";
        case CIOT_IFACE_TYPE_IOTA_CLIENT:
            return "IOTA_CLIENT";
        case CIOT_IFACE_TYPE_IOTA_SERVER:
            return "IOTA_SERVER";
        case CIOT_IFACE_TYPE_LOG:
            return "LOG";
        case CIOT_IFACE_TYPE_MBUS_CLIENT:
            return "MBUS_CLIENT";
        case CIOT_IFACE_TYPE_MBUS_SERVER:
            return "MBUS_SERVER";
        default:
            return "UNKNOWN";
        break;
    }
}

bool ciot_iface_is_equal(ciot_iface_info_t *self, ciot_iface_info_t *other)
{
    if(self == NULL|| other == NULL) return self == other;
    return memcmp(self, other, sizeof(ciot_iface_info_t)) == 0;
}

static ciot_err_t ciot_iface_send(ciot_iface_t *self, ciot_msg_t *msg)
{
    uint32_t size = 0;
    uint8_t buf[CIOT_CONFIG_MSG_SIZE];

    if(self->serializer != NULL)
    {
        size = self->serializer->to_bytes(buf, sizeof(buf), msg);
    }
    else
    {
        size = ciot_serializer_to_bytes(buf, sizeof(buf), msg, CIOT_MSG_FIELDS);
    }

    if(self->decoder != NULL)
    {
        return self->decoder->send(self->decoder, self, buf, size);
    }
    else
    {
        return self->send_data(self, buf, size);
    }
}

static ciot_err_t ciot_iface_process_data_result(ciot_iface_t *self, ciot_iface_t *sender, ciot_msg_t *msg, ciot_err_t error)
{
    if(self->req_status.state == CIOT_IFACE_REQ_STATE_IDLE || msg->error != CIOT_ERR_OK) 
    {
        return ciot_iface_send_rsp(sender, msg);
    }
    return error;
}

bool ciot_iface_event_is_ack(ciot_event_type_t event_type)
{
    return (event_type == CIOT_EVENT_TYPE_STARTED ||
            event_type == CIOT_EVENT_TYPE_STOPPED ||
            event_type == CIOT_EVENT_TYPE_ERROR);
}
