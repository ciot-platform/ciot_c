/**
 * @file ciot.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-12-09
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <stdlib.h>

#include "ciot.h"
#include "ciot_log.h"
#include "ciot_timer.h"
#include "ciot_serializer_pb.h"

#define CIOT_IFACE_START_TIMEOUT_SECS 5

static const char *TAG = "ciot";

static ciot_err_t ciot_starting_task(ciot_t self);
static ciot_err_t ciot_busy_task(ciot_t self);
static ciot_err_t ciot_set_iface_list(ciot_t self, ciot_iface_t *ifaces[], int count);
static ciot_err_t ciot_bytes_received(ciot_t self, ciot_iface_t *sender, uint8_t *bytes, int size);
static ciot_err_t ciot_iface_event_handler(ciot_iface_t *sender, ciot_event_t *event, void *event_args);

#ifdef CIOT_MG_ENABLED
struct mg_mgr mg;
#endif

ciot_t ciot_new(void)
{
    ciot_t self = calloc(1, sizeof(struct ciot));
    self->iface.ptr = self;
    ciot_init(self);
    #ifdef CIOT_MG_ENABLED
    mg_mgr_init(CIOT_HANDLE);
    #endif
    uint8_t version[] = { CIOT_VER };
    memcpy(self->info.version, version, sizeof(self->info.version));
    return self;
}

ciot_err_t ciot_start(ciot_t self, ciot_cfg_t *cfg)
{
    CIOT_ERR_NULL_CHECK(self)
    CIOT_ERR_NULL_CHECK(cfg);
    if (self->status.state != CIOT_STATE_IDLE &&
        self->status.state != CIOT_STATE_STARTED)
    {
        CIOT_LOGE(TAG, "ciot core incorrect state");
        return CIOT_ERR_INVALID_STATE;
    }
    self->ifaces.cfgs = cfg->ifaces.cfgs;
    self->storage = cfg->storage;
    ciot_set_iface_list(self, cfg->ifaces.list, cfg->ifaces.count);
    self->starter.timer = 0;
    self->starter.iface_id = 0;
    self->starter.waiting_result = false;
    self->status.state = CIOT_STATE_STARTING;
    CIOT_LOGI(TAG, "CIOT_STATE_STARTING");
    return CIOT_ERR_OK;
}

ciot_err_t ciot_stop(ciot_t self)
{
    CIOT_ERR_NULL_CHECK(self);
    self->status.state = CIOT_STATE_IDLE;
    CIOT_LOGV(TAG, "CIOT_STATE_IDLE");
    return CIOT_ERR_OK;
}

ciot_err_t ciot_task(ciot_t self)
{
    #ifdef CIOT_MG_ENABLED
    mg_mgr_poll(CIOT_HANDLE, CIOT_CONFIG_MG_POOL_INTERVAL_MS);
    #endif
    
    CIOT_ERR_NULL_CHECK(self);
    
    switch (self->status.state)
    {
    case CIOT_STATE_STARTING:
        ciot_starting_task(self);
        break;
    case CIOT_STATE_BUSY:
    case CIOT_STATE_PENDING_EVENT:
        ciot_busy_task(self);
        break;
    default:
        break;
    }

    return CIOT_ERR_OK;
}

ciot_err_t ciot_get_cfg(ciot_t self, ciot_cfg_t *cfg)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(cfg);
    *cfg = self->cfg;
    return CIOT_ERR_OK;
}

ciot_err_t ciot_get_status(ciot_t self, ciot_status_t *status)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(status);
    *status = self->status;
    return CIOT_ERR_OK;
}

ciot_err_t ciot_get_info(ciot_t self, ciot_info_t *info)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(info);
    *info = self->info;
    return CIOT_ERR_OK;
}

ciot_err_t ciot_delete_cfg(ciot_t self, ciot_iface_info_t *iface)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(self->storage);
    CIOT_ERR_VALUE_CHECK(iface->type, self->ifaces.list[iface->id]->info.type, CIOT_ERR_INVALID_TYPE);
    char filename[16];
    sprintf(filename, CIOT_IFACE_CFG_FILENAME, (int)iface->id);
    CIOT_LOGI(TAG, "Deleting configuration: %s file: %s", ciot_iface_type_to_str(iface->type), filename);
    return self->storage->remove(self->storage, filename);
}

ciot_err_t ciot_save_cfg(ciot_t self, ciot_iface_info_t *iface)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(self->storage);
    char filename[16];
    sprintf(filename, CIOT_IFACE_CFG_FILENAME, (int)iface->id);
    ciot_iface_t *iface_instance = self->ifaces.list[iface->id];
    ciot_msg_t msg = {
        .has_iface = true,
        .iface = iface_instance->info,
        .has_data = true,
        .data.which_type = CIOT_MSG_DATA_GET_DATA_TAG,
        .data.get_data.type = CIOT_DATA_TYPE_CONFIG,
    };
    CIOT_ERR_RETURN(iface_instance->get_data(iface_instance, &msg.data));
    return ciot_storage_save_data(self->storage, filename, &msg.data);
}

ciot_err_t ciot_load_cfg(ciot_t self, ciot_iface_info_t *iface, ciot_msg_data_t *cfg)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(self->storage);
    char filename[16];
    sprintf(filename, CIOT_IFACE_CFG_FILENAME, (int)iface->id);
    return ciot_storage_load_data(self->storage, filename, cfg);
}

ciot_err_t ciot_get_ifaces_info(ciot_t self, ciot_info_t *info)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(info);
    info->ifaces_count = self->ifaces.count;
    for (size_t i = 0; i < self->ifaces.count; i++)
    {
        if(self->ifaces.list[i] != NULL)
        {
            info->ifaces[i] = self->ifaces.list[i]->info.type;
        }
    }
    return CIOT_ERR_OK;
}

ciot_err_t ciot_delete_all(ciot_t self)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(self->storage);
    for (int i = 0; i < self->ifaces.count; i++)
    {
        char filename[16];
        sprintf(filename, CIOT_IFACE_CFG_FILENAME, i);
        ciot_err_t err = self->storage->remove(self->storage, filename);
        CIOT_LOGI(TAG, "Deleting configuration file: %s", filename);
        if(err != CIOT_ERR_OK) {
            CIOT_LOGI(TAG, "iface %d cfg file not deleted. reason: %s", 
                i,
                ciot_err_to_message(err));
        }
    }
    return CIOT_ERR_OK;
}

bool ciot_cfg_exists(ciot_t self, uint8_t iface_id)
{
    if(self == NULL) return false;
    if(self->storage == NULL) return false;
    if(iface_id >= self->ifaces.count) return false;
    char filename[18];
    int size = 0;
    sprintf(filename, CIOT_IFACE_CFG_FILENAME, iface_id);
    self->storage->read_bytes(self->storage, filename, NULL, &size);
    return size > 0;
}

const char *ciot_event_to_str(ciot_event_t *event)
{
    if(event == NULL) return NULL;

    switch (event->type)
    {
        case CIOT_EVENT_TYPE_UNKNOWN:
            return "EVENT_UNKNOWN";
        case CIOT_EVENT_TYPE_DATA:
            return "EVENT_DATA";
        case CIOT_EVENT_TYPE_STARTED:
            return "EVENT_STARTED";
        case CIOT_EVENT_TYPE_STOPPED:
            return "EVENT_STOPPED";
        case CIOT_EVENT_TYPE_ERROR:
            return "EVENT_ERROR";
        case CIOT_EVENT_TYPE_REQUEST:
            return "EVENT_REQUEST";
        case CIOT_EVENT_TYPE_DONE:
            return "EVENT_DONE";
        case CIOT_EVENT_TYPE_STATE_CHANGED:
            return "EVENT_STATE_CHANGED";
        case CIOT_EVENT_TYPE_INTERNAL:
            return "EVENT_INTERNAL";
        case CIOT_EVENT_TYPE_CUSTOM:
            return "EVENT_CUSTOM";
        default:
            return "DEFAULT";
    }
}

static ciot_err_t ciot_starting_task(ciot_t self)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(self->ifaces.list);

    ciot_starter_t *starter = &self->starter;
    ciot_receiver_t *receiver = &self->receiver;

    if (self->ifaces.count == 1)
    {
        return CIOT_ERR_OK;
    }

    if (starter->waiting_result)
    {
        if (receiver->sender != NULL &&
            ciot_iface_event_is_ack(receiver->event.type) &&
            starter->iface_id == receiver->sender->info.id)
        {
            CIOT_LOGI(TAG, "Interface [%lu]:%s evt %s received", (long unsigned int)receiver->sender->info.id, ciot_iface_to_str(receiver->sender), ciot_event_to_str(&receiver->event));
            if(self->iface.event_handler != NULL)
            {
                self->iface.event_handler(receiver->sender, &receiver->event, self->iface.event_args);
            }
            starter->iface_id++;
            starter->waiting_result = false;
        }
        else if (ciot_timer_compare(&starter->timer, CIOT_IFACE_START_TIMEOUT_SECS))
        {
            CIOT_LOGE(TAG, "Interface [%lu]:%s Timeout", (long unsigned int)starter->iface_id, ciot_iface_to_str(self->ifaces.list[starter->iface_id]));
            starter->iface_id++;
            starter->waiting_result = false;
        }
    }
    else
    {
        uint8_t id = starter->iface_id;
        if(self->iface.info.id == id)
        {
            starter->iface_id++;
            id++;
        }

        ciot_iface_t *iface = self->ifaces.list[id];
        ciot_msg_t msg = { 0 };

        if(iface == NULL)
        {
            CIOT_LOGI(TAG, "Interface %d is null. Skipping...", id);
            starter->iface_id++;
        }
        else
        {
            if(self->storage != NULL)
            {
                ciot_load_cfg(self, &iface->info, &msg.data);
                if(msg.data.which_type != 0)
                {
                    CIOT_LOGI(TAG, "Loading cfg into interface %s", ciot_iface_to_str(iface));
                }
            }

            if(msg.data.which_type == 0 && self->ifaces.cfgs[id] != NULL)
            {
                CIOT_LOGI(TAG, "Loading static configuration into interface %s", ciot_iface_to_str(iface));
                msg.data = *self->ifaces.cfgs[id];
            }

            if(msg.data.which_type != 0)
            {
                ciot_err_t err = iface->process_data(iface, &msg.data);
                if (err == CIOT_ERR_OK)
                {
                    ciot_timer_init(&starter->timer, CIOT_IFACE_START_TIMEOUT_SECS);
                    starter->waiting_result = true;
                }
                else
                {
                    CIOT_LOGE(TAG, "Interface [%u]:%s cannot start. Error: %s[%u]", id, ciot_iface_to_str(iface), ciot_err_to_message(err), err);
                    starter->iface_id++;
                }
            }
            else
            {
                CIOT_LOGI(TAG, "Interface [%u]:%s with null configuration not started.", id, ciot_iface_to_str(iface));
                starter->iface_id++;
            }
        }
    }

    if (starter->iface_id == self->ifaces.count)
    {
        self->status.state = CIOT_STATE_STARTED;
        CIOT_LOGI(TAG, "CIOT_STATE_STARTED");
        ciot_iface_send_event_type(&self->iface, CIOT_EVENT_TYPE_STARTED);
    }

    return CIOT_ERR_OK;
}

static ciot_err_t ciot_busy_task(ciot_t self)
{
    CIOT_LOGI(TAG, "ciot is busy...");

    ciot_receiver_t *receiver = &self->receiver;

    if(self->status.state == CIOT_STATE_PENDING_EVENT && self->receiver.event.msg.iface.id < self->ifaces.count)
    {
        receiver->sender = self->ifaces.list[self->receiver.event.msg.iface.id];
        self->status.state = CIOT_STATE_BUSY;
    }

    if (receiver->sender == NULL)
    {
        CIOT_LOGE(TAG, "Sender is null");
        self->status.state = CIOT_STATE_STARTED;
        return CIOT_ERR_NULL_ARG;
    }

    ciot_event_t *event = &receiver->event;
    ciot_iface_t *sender = receiver->sender;

    if (sender->req_status.state != CIOT_IFACE_REQ_STATE_IDLE && (event->which_data == CIOT_EVENT_MSG_TAG))
    {
        CIOT_LOGI(TAG, "Processing event from %s", ciot_iface_to_str(sender));
        bool iface_is_equal = sender->req_status.state == CIOT_IFACE_REQ_STATE_SENDED 
            ? ciot_iface_is_equal(&sender->req_status.iface, &event->msg.iface)
            : ciot_iface_is_equal(&sender->info, &event->msg.iface);
        if ((ciot_iface_event_is_ack(event->type) || event->type == CIOT_EVENT_TYPE_REQUEST) &&
            iface_is_equal)
        {
            if (sender->req_status.state == CIOT_IFACE_REQ_STATE_SENDED &&
                sender->req_status.id == event->msg.id)
            {
                CIOT_LOGI(TAG, "Request done");
                event->type = CIOT_EVENT_TYPE_DONE;
            }
            else
            {
                CIOT_LOGI(TAG, "Response sended");
                event->msg.id = sender->req_status.id;
                ciot_iface_send_rsp(self->ifaces.list[sender->req_status.iface.id], &event->msg);
            }
            sender->req_status.state = CIOT_IFACE_REQ_STATE_IDLE;
        }
        CIOT_LOGI(TAG, "Event processed");
    }

    if (event->type == CIOT_EVENT_TYPE_REQUEST)
    {
        CIOT_LOGI(TAG, "Processing request from %s", ciot_iface_to_str(sender));
        // CIOT_LOG_MSG_P(TAG, CIOT_LOGV, "RX REQ <- ", sender, event->msg);
        uint8_t id = event->msg.iface.id;
        if (id < self->ifaces.count /*&& event->msg->type <= CIOT__MSG_TYPE__MSG_TYPE_REQUEST*/)
        {
            ciot_iface_t *iface = self->ifaces.list[id];
            if (iface != NULL)
            {
                CIOT_LOGI(TAG, "Processing message");
                ciot_iface_process_msg(iface, &event->msg, sender);
                if (iface->req_status.state == CIOT_IFACE_REQ_STATE_IDLE)
                {
                    event->type = CIOT_EVENT_TYPE_DONE;
                }
            }
            else if(event->msg.iface.type == CIOT_IFACE_TYPE_CUSTOM)
            {
                CIOT_LOGE(TAG, "Error. %d iface is null.", id);
                event->type = CIOT_EVENT_TYPE_ERROR;
                ciot_iface_send_error(sender, CIOT_IFACE_TYPE_UNDEFINED, id, &event->msg, CIOT_ERR_NULL_ARG);
            }
        }
    }

    receiver->sender = NULL;

    self->status.state = self->status.state != CIOT_STATE_PENDING_EVENT
        ? CIOT_STATE_STARTED
        : CIOT_STATE_PENDING_EVENT;

    if(self->iface.event_handler != NULL)
    {
        self->iface.event_handler(sender, event, self->iface.event_args);
    }

    CIOT_LOGI(TAG, "ciot done");

    return CIOT_ERR_OK;
}

static ciot_err_t ciot_set_iface_list(ciot_t self, ciot_iface_t *ifaces[], int count)
{
    ciot_err_t ret = CIOT_ERR_OK;

    self->ifaces.list = ifaces;
    self->ifaces.count = count;
    self->status.ifaces_count = count;

    for (int i = 0; i < count; i++)
    {
        ciot_iface_t *iface = self->ifaces.list[i];
        self->status.ifaces[i].type = iface ? iface->info.type : CIOT_IFACE_TYPE_UNDEFINED;
        if (iface != NULL)
        {
            iface->info.id = i;
            CIOT_LOGI(TAG, "Interface %d %s at %p", i, ciot_iface_to_str(iface), iface);
            if(iface->info.id != self->iface.info.id)
            {
                ciot_err_t err = ciot_iface_set_event_handler(iface, ciot_iface_event_handler, self);
                if (err != CIOT_ERR_OK)
                {
                    CIOT_LOGE(TAG, "Interface id:%d type%d register event error: %d", i, iface->info.type, err);
                    ret = err;
                }
            }
        }
        else
        {
            CIOT_LOGI(TAG, "Interface %d is null.", i);
        }
    }

    return ret;
}

static ciot_err_t ciot_iface_event_handler(ciot_iface_t *sender, ciot_event_t *event, void *event_args)
{
    ciot_t self = (ciot_t)event_args;
    ciot_receiver_t *receiver = &self->receiver;

    CIOT_LOGD(TAG, "evt: %s(%lu): %s", ciot_iface_to_str(sender), (long unsigned int)sender->info.id, ciot_event_to_str(event));

    if(sender->info.id < sizeof(self->status.ifaces))
    {
        if(event->type == CIOT_EVENT_TYPE_STARTED)
        {
            self->status.ifaces[sender->info.id].state = CIOT_IFACE_STATE_STARTED;
            self->ifaces.list[sender->info.id]->state = CIOT_IFACE_STATE_STARTED;
        }
        if(event->type == CIOT_EVENT_TYPE_STOPPED)
        {
            self->status.ifaces[sender->info.id].state = CIOT_IFACE_STATE_STOPPED;
            self->ifaces.list[sender->info.id]->state = CIOT_IFACE_STATE_STOPPED;
        }
        if(event->type == CIOT_EVENT_TYPE_ERROR)
        {
            self->status.ifaces[sender->info.id].state = CIOT_IFACE_STATE_STOPPED;
            self->ifaces.list[sender->info.id]->state = CIOT_IFACE_STATE_STOPPED;
        }
    }

    if ((event->type == CIOT_EVENT_TYPE_DATA || 
         event->type == CIOT_EVENT_TYPE_INTERNAL ||
         event->type == CIOT_EVENT_TYPE_CUSTOM) && 
         self->iface.event_handler != NULL)
    {
        return self->iface.event_handler(sender, event, self->iface.event_args);
    }

    if (self->status.state == CIOT_STATE_BUSY)
    {
        if(ciot_iface_is_equal(&event->msg.iface, &receiver->event.msg.iface))
        {
            CIOT_LOGI(TAG, "ciot busy but event from same iface. processing...");
            self->status.state = CIOT_STATE_PENDING_EVENT;
        }
        else
        {
            CIOT_LOGE(TAG, "ciot busy. %s(%lu) evt:%s ignored", ciot_iface_to_str(sender), (long unsigned int)sender->info.id, ciot_event_to_str(event));
            return CIOT_ERR_BUSY;
        }
    }

    receiver->event = *event;
    receiver->sender = sender;
    
    if (event->type == CIOT_EVENT_TYPE_REQUEST)
    {
        if (self->status.state != CIOT_STATE_STARTED)
        {
            CIOT_LOGE(TAG, "ciot core is not started");
            return CIOT_ERR_BUSY;
        }

        CIOT_ERR_RETURN(ciot_bytes_received(self, sender, event->raw.bytes, event->raw.size));

#if CIOT_CONFIG_FEATURE_LOG == 1
        if(receiver->event.msg.data.which_type == CIOT_MSG_DATA_LOG_TAG)
        {
            ciot_log_data_t *log = &receiver->event.msg.data.log;
            if(log->level == CIOT_LOG_LEVEL_INFO) CIOT_LOGI(TAG, "[%s] %s", log->tag, log->message);
            if(log->level == CIOT_LOG_LEVEL_WARNING) CIOT_LOGW(TAG, "[%s] %s", log->tag, log->message);
            if(log->level == CIOT_LOG_LEVEL_ERROR) CIOT_LOGE(TAG, "[%s] %s", log->tag, log->message);
        }
#endif 

        if(receiver->event.msg.iface.type == CIOT_IFACE_TYPE_CUSTOM)
        {
            if(self->iface.event_handler != NULL)
            {
                ciot_err_t err = self->iface.event_handler(sender, &receiver->event, self->iface.event_args);
                if(err != CIOT_ERR_OK)
                {
                    CIOT_LOGE(TAG, "error from application event handler: %s", ciot_err_to_message(err));
                    ciot_iface_send_error(receiver->sender, receiver->event.msg.iface.type, receiver->event.msg.iface.type, &receiver->event.msg, err);
                }
            }
            else
            {
                CIOT_LOGE(TAG, "ciot event handler is null");
                ciot_iface_send_error(receiver->sender, CIOT_IFACE_TYPE_CIOT, self->iface.info.id, &receiver->event.msg, CIOT_ERR_NULL_EVENT_HANDLER);
            }
        }
        else
        {
            self->status.state = CIOT_STATE_BUSY;
        }
    }
    else if(self->status.state == CIOT_STATE_STARTED)
    {
        CIOT_LOGI(TAG, "Event received from %s", ciot_iface_to_str(sender));
        receiver->sender = sender;
        self->status.state = CIOT_STATE_BUSY;
    }

    return CIOT_ERR_OK;
}

static ciot_err_t ciot_bytes_received(ciot_t self, ciot_iface_t *sender, uint8_t *bytes, int size)
{
    if(sender->serializer != NULL)
    {
        sender->serializer->from_bytes(bytes, size, &self->receiver.event.msg);
        self->receiver.event.which_data = CIOT_EVENT_MSG_TAG;
        return CIOT_ERR_OK;
    }
    else
    {
        ciot_serializer_from_bytes(bytes, size, &self->receiver.event.msg, CIOT_MSG_FIELDS);
        self->receiver.event.which_data = CIOT_EVENT_MSG_TAG;
        return CIOT_ERR_OK;
    }
}
