/* Generated by the protocol buffer compiler.  DO NOT EDIT! */
/* Generated from: ciot/proto/v1/ciot.proto */

#ifndef PROTOBUF_C_ciot_2fproto_2fv1_2fciot_2eproto__INCLUDED
#define PROTOBUF_C_ciot_2fproto_2fv1_2fciot_2eproto__INCLUDED

#include <protobuf-c/protobuf-c.h>

PROTOBUF_C__BEGIN_DECLS

#if PROTOBUF_C_VERSION_NUMBER < 1003000
# error This file was generated by a newer version of protoc-c which is incompatible with your libprotobuf-c headers. Please update your headers.
#elif 1003003 < PROTOBUF_C_MIN_COMPILER_VERSION
# error This file was generated by an older version of protoc-c which is incompatible with your libprotobuf-c headers. Please regenerate this file with a newer version of protoc-c.
#endif


typedef struct _Ciot__CiotCfg Ciot__CiotCfg;
typedef struct _Ciot__CiotInfo Ciot__CiotInfo;
typedef struct _Ciot__CiotStatus Ciot__CiotStatus;
typedef struct _Ciot__CiotReqData Ciot__CiotReqData;
typedef struct _Ciot__CiotReq Ciot__CiotReq;
typedef struct _Ciot__CiotData Ciot__CiotData;


/* --- enums --- */

/*
 * Enum representing the state of CIOT.
 */
typedef enum _Ciot__CiotState {
  /*
   * CIOT core is in an idle state.
   */
  CIOT__CIOT_STATE__CIOT_STATE_IDLE = 0,
  /*
   * CIOT core is starting.
   */
  CIOT__CIOT_STATE__CIOT_STATE_STARTING = 1,
  /*
   * CIOT core is started.
   */
  CIOT__CIOT_STATE__CIOT_STATE_STARTED = 2,
  /*
   * CIOT core is busy.
   */
  CIOT__CIOT_STATE__CIOT_STATE_BUSY = 3,
  /*
   * CIOT core encountered an error.
   */
  CIOT__CIOT_STATE__CIOT_STATE_ERROR = -1
    PROTOBUF_C__FORCE_ENUM_TO_BE_INT_SIZE(CIOT__CIOT_STATE)
} Ciot__CiotState;
/*
 * Enum representing different types of CIOT requests.
 */
typedef enum _Ciot__CiotReqType {
  /*
   * Unknown request type.
   */
  CIOT__CIOT_REQ_TYPE__CIOT_REQ_TYPE_UNKOWN = 0,
  /*
   * Request to save interface configuration.
   */
  CIOT__CIOT_REQ_TYPE__CIOT_REQ_TYPE_SAVE_IFACE_CFG = 1,
  /*
   * Request to delete interface configuration.
   */
  CIOT__CIOT_REQ_TYPE__CIOT_REQ_TYPE_DELETE_IFACE_CFG = 2,
  /*
   * Request to proxy a message.
   */
  CIOT__CIOT_REQ_TYPE__CIOT_REQ_TYPE_PROXY_MSG = 3
    PROTOBUF_C__FORCE_ENUM_TO_BE_INT_SIZE(CIOT__CIOT_REQ_TYPE)
} Ciot__CiotReqType;
/*
 * Enum representing different serialization types for CIOT.
 */
typedef enum _Ciot__CiotSerializationType {
  /*
   * Protocol Buffers serialization type.
   */
  CIOT__CIOT_SERIALIZATION_TYPE__CIOT_SERIALIZATION_PROTOBUF = 0
    PROTOBUF_C__FORCE_ENUM_TO_BE_INT_SIZE(CIOT__CIOT_SERIALIZATION_TYPE)
} Ciot__CiotSerializationType;

/* --- messages --- */

/*
 * Message representing CIOT configuration.
 */
struct  _Ciot__CiotCfg
{
  ProtobufCMessage base;
};
#define CIOT__CIOT_CFG__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&ciot__ciot_cfg__descriptor) \
     }


/*
 * Message representing CIOT information.
 */
struct  _Ciot__CiotInfo
{
  ProtobufCMessage base;
  /*
   * CIOT library version
   */
  ProtobufCBinaryData version;
};
#define CIOT__CIOT_INFO__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&ciot__ciot_info__descriptor) \
    , {0,NULL} }


/*
 * Message representing CIOT status.
 */
struct  _Ciot__CiotStatus
{
  ProtobufCMessage base;
  /*
   * State of the CIOT device.
   */
  Ciot__CiotState state;
};
#define CIOT__CIOT_STATUS__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&ciot__ciot_status__descriptor) \
    , CIOT__CIOT_STATE__CIOT_STATE_IDLE }


/*
 * Message representing data for a CIOT request.
 */
struct  _Ciot__CiotReqData
{
  ProtobufCMessage base;
  /*
   * Interface ID for the request.
   */
  uint32_t iface_id;
  /*
   * Payload data for the request.
   */
  ProtobufCBinaryData payload;
};
#define CIOT__CIOT_REQ_DATA__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&ciot__ciot_req_data__descriptor) \
    , 0, {0,NULL} }


/*
 * Message representing a CIOT request.
 */
struct  _Ciot__CiotReq
{
  ProtobufCMessage base;
  /*
   * Type of the CIOT request.
   */
  Ciot__CiotReqType type;
  /*
   * Data associated with the request.
   */
  Ciot__CiotReqData *data;
};
#define CIOT__CIOT_REQ__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&ciot__ciot_req__descriptor) \
    , CIOT__CIOT_REQ_TYPE__CIOT_REQ_TYPE_UNKOWN, NULL }


/*
 * Message representing CIOT data.
 */
struct  _Ciot__CiotData
{
  ProtobufCMessage base;
  /*
   * Configuration data.
   */
  Ciot__CiotCfg *config;
  /*
   * Status of the CIOT device.
   */
  Ciot__CiotStatus *status;
  /*
   * CIOT request data.
   */
  Ciot__CiotReq *request;
  /*
   * Additional information about the CIOT device.
   */
  Ciot__CiotInfo *info;
};
#define CIOT__CIOT_DATA__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&ciot__ciot_data__descriptor) \
    , NULL, NULL, NULL, NULL }


/* Ciot__CiotCfg methods */
void   ciot__ciot_cfg__init
                     (Ciot__CiotCfg         *message);
size_t ciot__ciot_cfg__get_packed_size
                     (const Ciot__CiotCfg   *message);
size_t ciot__ciot_cfg__pack
                     (const Ciot__CiotCfg   *message,
                      uint8_t             *out);
size_t ciot__ciot_cfg__pack_to_buffer
                     (const Ciot__CiotCfg   *message,
                      ProtobufCBuffer     *buffer);
Ciot__CiotCfg *
       ciot__ciot_cfg__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   ciot__ciot_cfg__free_unpacked
                     (Ciot__CiotCfg *message,
                      ProtobufCAllocator *allocator);
/* Ciot__CiotInfo methods */
void   ciot__ciot_info__init
                     (Ciot__CiotInfo         *message);
size_t ciot__ciot_info__get_packed_size
                     (const Ciot__CiotInfo   *message);
size_t ciot__ciot_info__pack
                     (const Ciot__CiotInfo   *message,
                      uint8_t             *out);
size_t ciot__ciot_info__pack_to_buffer
                     (const Ciot__CiotInfo   *message,
                      ProtobufCBuffer     *buffer);
Ciot__CiotInfo *
       ciot__ciot_info__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   ciot__ciot_info__free_unpacked
                     (Ciot__CiotInfo *message,
                      ProtobufCAllocator *allocator);
/* Ciot__CiotStatus methods */
void   ciot__ciot_status__init
                     (Ciot__CiotStatus         *message);
size_t ciot__ciot_status__get_packed_size
                     (const Ciot__CiotStatus   *message);
size_t ciot__ciot_status__pack
                     (const Ciot__CiotStatus   *message,
                      uint8_t             *out);
size_t ciot__ciot_status__pack_to_buffer
                     (const Ciot__CiotStatus   *message,
                      ProtobufCBuffer     *buffer);
Ciot__CiotStatus *
       ciot__ciot_status__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   ciot__ciot_status__free_unpacked
                     (Ciot__CiotStatus *message,
                      ProtobufCAllocator *allocator);
/* Ciot__CiotReqData methods */
void   ciot__ciot_req_data__init
                     (Ciot__CiotReqData         *message);
size_t ciot__ciot_req_data__get_packed_size
                     (const Ciot__CiotReqData   *message);
size_t ciot__ciot_req_data__pack
                     (const Ciot__CiotReqData   *message,
                      uint8_t             *out);
size_t ciot__ciot_req_data__pack_to_buffer
                     (const Ciot__CiotReqData   *message,
                      ProtobufCBuffer     *buffer);
Ciot__CiotReqData *
       ciot__ciot_req_data__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   ciot__ciot_req_data__free_unpacked
                     (Ciot__CiotReqData *message,
                      ProtobufCAllocator *allocator);
/* Ciot__CiotReq methods */
void   ciot__ciot_req__init
                     (Ciot__CiotReq         *message);
size_t ciot__ciot_req__get_packed_size
                     (const Ciot__CiotReq   *message);
size_t ciot__ciot_req__pack
                     (const Ciot__CiotReq   *message,
                      uint8_t             *out);
size_t ciot__ciot_req__pack_to_buffer
                     (const Ciot__CiotReq   *message,
                      ProtobufCBuffer     *buffer);
Ciot__CiotReq *
       ciot__ciot_req__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   ciot__ciot_req__free_unpacked
                     (Ciot__CiotReq *message,
                      ProtobufCAllocator *allocator);
/* Ciot__CiotData methods */
void   ciot__ciot_data__init
                     (Ciot__CiotData         *message);
size_t ciot__ciot_data__get_packed_size
                     (const Ciot__CiotData   *message);
size_t ciot__ciot_data__pack
                     (const Ciot__CiotData   *message,
                      uint8_t             *out);
size_t ciot__ciot_data__pack_to_buffer
                     (const Ciot__CiotData   *message,
                      ProtobufCBuffer     *buffer);
Ciot__CiotData *
       ciot__ciot_data__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   ciot__ciot_data__free_unpacked
                     (Ciot__CiotData *message,
                      ProtobufCAllocator *allocator);
/* --- per-message closures --- */

typedef void (*Ciot__CiotCfg_Closure)
                 (const Ciot__CiotCfg *message,
                  void *closure_data);
typedef void (*Ciot__CiotInfo_Closure)
                 (const Ciot__CiotInfo *message,
                  void *closure_data);
typedef void (*Ciot__CiotStatus_Closure)
                 (const Ciot__CiotStatus *message,
                  void *closure_data);
typedef void (*Ciot__CiotReqData_Closure)
                 (const Ciot__CiotReqData *message,
                  void *closure_data);
typedef void (*Ciot__CiotReq_Closure)
                 (const Ciot__CiotReq *message,
                  void *closure_data);
typedef void (*Ciot__CiotData_Closure)
                 (const Ciot__CiotData *message,
                  void *closure_data);

/* --- services --- */


/* --- descriptors --- */

extern const ProtobufCEnumDescriptor    ciot__ciot_state__descriptor;
extern const ProtobufCEnumDescriptor    ciot__ciot_req_type__descriptor;
extern const ProtobufCEnumDescriptor    ciot__ciot_serialization_type__descriptor;
extern const ProtobufCMessageDescriptor ciot__ciot_cfg__descriptor;
extern const ProtobufCMessageDescriptor ciot__ciot_info__descriptor;
extern const ProtobufCMessageDescriptor ciot__ciot_status__descriptor;
extern const ProtobufCMessageDescriptor ciot__ciot_req_data__descriptor;
extern const ProtobufCMessageDescriptor ciot__ciot_req__descriptor;
extern const ProtobufCMessageDescriptor ciot__ciot_data__descriptor;

PROTOBUF_C__END_DECLS


#endif  /* PROTOBUF_C_ciot_2fproto_2fv1_2fciot_2eproto__INCLUDED */
