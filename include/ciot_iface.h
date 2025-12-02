/**
 * @file ciot_iface.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-27
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef __CIOT_IFACE__H__
#define __CIOT_IFACE__H__

#include <stdbool.h>

#include "ciot_types.h"
#include "ciot_err.h"
#include "ciot_event.h"
#include "ciot_serializer.h"

typedef struct ciot_decoder *ciot_decoder_t; // Forward declaration.

typedef struct ciot_iface ciot_iface_t;
typedef ciot_err_t (ciot_iface_process_data_fn)(ciot_iface_t *self, ciot_msg_data_t *req);
typedef ciot_err_t (ciot_iface_send_data_fn)(ciot_iface_t *self, uint8_t *data, int size);
typedef ciot_err_t (ciot_iface_read_data_fn)(ciot_iface_t *self, uint8_t *data, int size);
typedef ciot_err_t (ciot_iface_get_data_fn)(ciot_iface_t *self, ciot_msg_data_t *req);
typedef ciot_err_t (ciot_iface_event_handler_fn)(ciot_iface_t *sender, ciot_event_t *event, void *args);

struct ciot_iface
{
    void *ptr;
    ciot_iface_process_data_fn *process_data;
    ciot_iface_send_data_fn *send_data;
    ciot_iface_read_data_fn *read_data;
    ciot_iface_get_data_fn *get_data;
    ciot_iface_event_handler_fn *event_handler;
    void *event_args;
    ciot_iface_info_t info;
    ciot_decoder_t decoder;
    ciot_serializer_t serializer;
    ciot_iface_req_status_t req_status;
    ciot_iface_state_t state;
};

ciot_err_t ciot_iface_send_msg(ciot_iface_t *self, ciot_msg_t *msg);
ciot_err_t ciot_iface_send_rsp(ciot_iface_t *self, ciot_msg_t *rsp);
ciot_err_t ciot_iface_send_req(ciot_iface_t *self, ciot_msg_t *req);
ciot_err_t ciot_iface_send_error(ciot_iface_t *self, ciot_iface_type_t iface_type, uint16_t iface_id, ciot_msg_t *msg, ciot_err_t err);
ciot_err_t ciot_iface_send_event(ciot_iface_t *self, ciot_event_t *event);
ciot_err_t ciot_iface_send_event_type(ciot_iface_t *self, ciot_event_type_t event_type);
ciot_err_t ciot_iface_process_msg(ciot_iface_t *self, ciot_msg_t *msg, ciot_iface_t *sender);
ciot_err_t ciot_iface_process_data(ciot_iface_t *self, uint8_t *data, int size, ciot_event_type_t event_type);
ciot_err_t ciot_iface_register_req(ciot_iface_t *self, ciot_iface_info_t *iface, ciot_msg_t *msg, ciot_iface_req_state_t state);
ciot_err_t ciot_iface_set_event_handler(ciot_iface_t *self, ciot_iface_event_handler_fn event_handler, void *event_args);
ciot_err_t ciot_iface_set_serializer(ciot_iface_t *self, ciot_serializer_t serializer);
ciot_err_t ciot_iface_set_decoder(ciot_iface_t *self, ciot_decoder_t decoder);
ciot_err_t ciot_iface_get_msg_id(void);

int ciot_iface_send_bytes(ciot_iface_t *self, uint8_t *bytes, int size);
int ciot_iface_read_bytes(ciot_iface_t *self, uint8_t *bytes, int size);

const char *ciot_iface_to_str(ciot_iface_t *iface);
const char *ciot_iface_type_to_str(ciot_iface_type_t iface_type);

bool ciot_iface_is_equal(ciot_iface_info_t *iface, ciot_iface_info_t *other);
bool ciot_iface_event_is_ack(ciot_event_type_t event_type);

#endif  //!__CIOT_IFACE__H__