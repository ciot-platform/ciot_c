/* Generated by the protocol buffer compiler.  DO NOT EDIT! */
/* Generated from: ciot/proto/v1/http_server.proto */

/* Do not generate deprecated warnings for self */
#ifndef PROTOBUF_C__NO_DEPRECATED
#define PROTOBUF_C__NO_DEPRECATED
#endif

#include "ciot/proto/v1/http_server.pb-c.h"
void   ciot__http_server_cfg__init
                     (Ciot__HttpServerCfg         *message)
{
  static const Ciot__HttpServerCfg init_value = CIOT__HTTP_SERVER_CFG__INIT;
  *message = init_value;
}
size_t ciot__http_server_cfg__get_packed_size
                     (const Ciot__HttpServerCfg *message)
{
  assert(message->base.descriptor == &ciot__http_server_cfg__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t ciot__http_server_cfg__pack
                     (const Ciot__HttpServerCfg *message,
                      uint8_t       *out)
{
  assert(message->base.descriptor == &ciot__http_server_cfg__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t ciot__http_server_cfg__pack_to_buffer
                     (const Ciot__HttpServerCfg *message,
                      ProtobufCBuffer *buffer)
{
  assert(message->base.descriptor == &ciot__http_server_cfg__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
Ciot__HttpServerCfg *
       ciot__http_server_cfg__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (Ciot__HttpServerCfg *)
     protobuf_c_message_unpack (&ciot__http_server_cfg__descriptor,
                                allocator, len, data);
}
void   ciot__http_server_cfg__free_unpacked
                     (Ciot__HttpServerCfg *message,
                      ProtobufCAllocator *allocator)
{
  if(!message)
    return;
  assert(message->base.descriptor == &ciot__http_server_cfg__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
void   ciot__http_server_status__init
                     (Ciot__HttpServerStatus         *message)
{
  static const Ciot__HttpServerStatus init_value = CIOT__HTTP_SERVER_STATUS__INIT;
  *message = init_value;
}
size_t ciot__http_server_status__get_packed_size
                     (const Ciot__HttpServerStatus *message)
{
  assert(message->base.descriptor == &ciot__http_server_status__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t ciot__http_server_status__pack
                     (const Ciot__HttpServerStatus *message,
                      uint8_t       *out)
{
  assert(message->base.descriptor == &ciot__http_server_status__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t ciot__http_server_status__pack_to_buffer
                     (const Ciot__HttpServerStatus *message,
                      ProtobufCBuffer *buffer)
{
  assert(message->base.descriptor == &ciot__http_server_status__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
Ciot__HttpServerStatus *
       ciot__http_server_status__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (Ciot__HttpServerStatus *)
     protobuf_c_message_unpack (&ciot__http_server_status__descriptor,
                                allocator, len, data);
}
void   ciot__http_server_status__free_unpacked
                     (Ciot__HttpServerStatus *message,
                      ProtobufCAllocator *allocator)
{
  if(!message)
    return;
  assert(message->base.descriptor == &ciot__http_server_status__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
void   ciot__http_server_req__init
                     (Ciot__HttpServerReq         *message)
{
  static const Ciot__HttpServerReq init_value = CIOT__HTTP_SERVER_REQ__INIT;
  *message = init_value;
}
size_t ciot__http_server_req__get_packed_size
                     (const Ciot__HttpServerReq *message)
{
  assert(message->base.descriptor == &ciot__http_server_req__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t ciot__http_server_req__pack
                     (const Ciot__HttpServerReq *message,
                      uint8_t       *out)
{
  assert(message->base.descriptor == &ciot__http_server_req__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t ciot__http_server_req__pack_to_buffer
                     (const Ciot__HttpServerReq *message,
                      ProtobufCBuffer *buffer)
{
  assert(message->base.descriptor == &ciot__http_server_req__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
Ciot__HttpServerReq *
       ciot__http_server_req__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (Ciot__HttpServerReq *)
     protobuf_c_message_unpack (&ciot__http_server_req__descriptor,
                                allocator, len, data);
}
void   ciot__http_server_req__free_unpacked
                     (Ciot__HttpServerReq *message,
                      ProtobufCAllocator *allocator)
{
  if(!message)
    return;
  assert(message->base.descriptor == &ciot__http_server_req__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
void   ciot__http_server_data__init
                     (Ciot__HttpServerData         *message)
{
  static const Ciot__HttpServerData init_value = CIOT__HTTP_SERVER_DATA__INIT;
  *message = init_value;
}
size_t ciot__http_server_data__get_packed_size
                     (const Ciot__HttpServerData *message)
{
  assert(message->base.descriptor == &ciot__http_server_data__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t ciot__http_server_data__pack
                     (const Ciot__HttpServerData *message,
                      uint8_t       *out)
{
  assert(message->base.descriptor == &ciot__http_server_data__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t ciot__http_server_data__pack_to_buffer
                     (const Ciot__HttpServerData *message,
                      ProtobufCBuffer *buffer)
{
  assert(message->base.descriptor == &ciot__http_server_data__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
Ciot__HttpServerData *
       ciot__http_server_data__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (Ciot__HttpServerData *)
     protobuf_c_message_unpack (&ciot__http_server_data__descriptor,
                                allocator, len, data);
}
void   ciot__http_server_data__free_unpacked
                     (Ciot__HttpServerData *message,
                      ProtobufCAllocator *allocator)
{
  if(!message)
    return;
  assert(message->base.descriptor == &ciot__http_server_data__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
static const ProtobufCFieldDescriptor ciot__http_server_cfg__field_descriptors[3] =
{
  {
    "address",
    1,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    offsetof(Ciot__HttpServerCfg, address),
    NULL,
    &protobuf_c_empty_string,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "route",
    2,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    offsetof(Ciot__HttpServerCfg, route),
    NULL,
    &protobuf_c_empty_string,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "port",
    3,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_UINT32,
    0,   /* quantifier_offset */
    offsetof(Ciot__HttpServerCfg, port),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned ciot__http_server_cfg__field_indices_by_name[] = {
  0,   /* field[0] = address */
  2,   /* field[2] = port */
  1,   /* field[1] = route */
};
static const ProtobufCIntRange ciot__http_server_cfg__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 3 }
};
const ProtobufCMessageDescriptor ciot__http_server_cfg__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "Ciot.HttpServerCfg",
  "HttpServerCfg",
  "Ciot__HttpServerCfg",
  "Ciot",
  sizeof(Ciot__HttpServerCfg),
  3,
  ciot__http_server_cfg__field_descriptors,
  ciot__http_server_cfg__field_indices_by_name,
  1,  ciot__http_server_cfg__number_ranges,
  (ProtobufCMessageInit) ciot__http_server_cfg__init,
  NULL,NULL,NULL    /* reserved[123] */
};
static const ProtobufCFieldDescriptor ciot__http_server_status__field_descriptors[2] =
{
  {
    "state",
    1,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_ENUM,
    0,   /* quantifier_offset */
    offsetof(Ciot__HttpServerStatus, state),
    &ciot__http_server_state__descriptor,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "error",
    2,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    offsetof(Ciot__HttpServerStatus, error),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned ciot__http_server_status__field_indices_by_name[] = {
  1,   /* field[1] = error */
  0,   /* field[0] = state */
};
static const ProtobufCIntRange ciot__http_server_status__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 2 }
};
const ProtobufCMessageDescriptor ciot__http_server_status__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "Ciot.HttpServerStatus",
  "HttpServerStatus",
  "Ciot__HttpServerStatus",
  "Ciot",
  sizeof(Ciot__HttpServerStatus),
  2,
  ciot__http_server_status__field_descriptors,
  ciot__http_server_status__field_indices_by_name,
  1,  ciot__http_server_status__number_ranges,
  (ProtobufCMessageInit) ciot__http_server_status__init,
  NULL,NULL,NULL    /* reserved[123] */
};
static const ProtobufCFieldDescriptor ciot__http_server_req__field_descriptors[1] =
{
  {
    "type",
    1,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_ENUM,
    0,   /* quantifier_offset */
    offsetof(Ciot__HttpServerReq, type),
    &ciot__http_server_req_type__descriptor,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned ciot__http_server_req__field_indices_by_name[] = {
  0,   /* field[0] = type */
};
static const ProtobufCIntRange ciot__http_server_req__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 1 }
};
const ProtobufCMessageDescriptor ciot__http_server_req__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "Ciot.HttpServerReq",
  "HttpServerReq",
  "Ciot__HttpServerReq",
  "Ciot",
  sizeof(Ciot__HttpServerReq),
  1,
  ciot__http_server_req__field_descriptors,
  ciot__http_server_req__field_indices_by_name,
  1,  ciot__http_server_req__number_ranges,
  (ProtobufCMessageInit) ciot__http_server_req__init,
  NULL,NULL,NULL    /* reserved[123] */
};
static const ProtobufCFieldDescriptor ciot__http_server_data__field_descriptors[3] =
{
  {
    "config",
    1,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_MESSAGE,
    0,   /* quantifier_offset */
    offsetof(Ciot__HttpServerData, config),
    &ciot__http_server_cfg__descriptor,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "status",
    2,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_MESSAGE,
    0,   /* quantifier_offset */
    offsetof(Ciot__HttpServerData, status),
    &ciot__http_server_status__descriptor,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "request",
    3,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_MESSAGE,
    0,   /* quantifier_offset */
    offsetof(Ciot__HttpServerData, request),
    &ciot__http_server_req__descriptor,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned ciot__http_server_data__field_indices_by_name[] = {
  0,   /* field[0] = config */
  2,   /* field[2] = request */
  1,   /* field[1] = status */
};
static const ProtobufCIntRange ciot__http_server_data__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 3 }
};
const ProtobufCMessageDescriptor ciot__http_server_data__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "Ciot.HttpServerData",
  "HttpServerData",
  "Ciot__HttpServerData",
  "Ciot",
  sizeof(Ciot__HttpServerData),
  3,
  ciot__http_server_data__field_descriptors,
  ciot__http_server_data__field_indices_by_name,
  1,  ciot__http_server_data__number_ranges,
  (ProtobufCMessageInit) ciot__http_server_data__init,
  NULL,NULL,NULL    /* reserved[123] */
};
static const ProtobufCEnumValue ciot__http_server_state__enum_values_by_number[3] =
{
  { "HTTP_SERVER_STATE_ERROR", "CIOT__HTTP_SERVER_STATE__HTTP_SERVER_STATE_ERROR", -1 },
  { "HTTP_SERVER_STATE_STOPPED", "CIOT__HTTP_SERVER_STATE__HTTP_SERVER_STATE_STOPPED", 0 },
  { "HTTP_SERVER_STATE_STARTED", "CIOT__HTTP_SERVER_STATE__HTTP_SERVER_STATE_STARTED", 1 },
};
static const ProtobufCIntRange ciot__http_server_state__value_ranges[] = {
{-1, 0},{0, 3}
};
static const ProtobufCEnumValueIndex ciot__http_server_state__enum_values_by_name[3] =
{
  { "HTTP_SERVER_STATE_ERROR", 0 },
  { "HTTP_SERVER_STATE_STARTED", 2 },
  { "HTTP_SERVER_STATE_STOPPED", 1 },
};
const ProtobufCEnumDescriptor ciot__http_server_state__descriptor =
{
  PROTOBUF_C__ENUM_DESCRIPTOR_MAGIC,
  "Ciot.HttpServerState",
  "HttpServerState",
  "Ciot__HttpServerState",
  "Ciot",
  3,
  ciot__http_server_state__enum_values_by_number,
  3,
  ciot__http_server_state__enum_values_by_name,
  1,
  ciot__http_server_state__value_ranges,
  NULL,NULL,NULL,NULL   /* reserved[1234] */
};
static const ProtobufCEnumValue ciot__http_server_req_type__enum_values_by_number[1] =
{
  { "HTTP_SERVER_REQ_TYPE_UNKOWN", "CIOT__HTTP_SERVER_REQ_TYPE__HTTP_SERVER_REQ_TYPE_UNKOWN", 0 },
};
static const ProtobufCIntRange ciot__http_server_req_type__value_ranges[] = {
{0, 0},{0, 1}
};
static const ProtobufCEnumValueIndex ciot__http_server_req_type__enum_values_by_name[1] =
{
  { "HTTP_SERVER_REQ_TYPE_UNKOWN", 0 },
};
const ProtobufCEnumDescriptor ciot__http_server_req_type__descriptor =
{
  PROTOBUF_C__ENUM_DESCRIPTOR_MAGIC,
  "Ciot.HttpServerReqType",
  "HttpServerReqType",
  "Ciot__HttpServerReqType",
  "Ciot",
  1,
  ciot__http_server_req_type__enum_values_by_number,
  1,
  ciot__http_server_req_type__enum_values_by_name,
  1,
  ciot__http_server_req_type__value_ranges,
  NULL,NULL,NULL,NULL   /* reserved[1234] */
};
