/* Generated by the protocol buffer compiler.  DO NOT EDIT! */
/* Generated from: ciot/proto/v1/ntp.proto */

#ifndef PROTOBUF_C_ciot_2fproto_2fv1_2fntp_2eproto__INCLUDED
#define PROTOBUF_C_ciot_2fproto_2fv1_2fntp_2eproto__INCLUDED

#include <protobuf-c/protobuf-c.h>

PROTOBUF_C__BEGIN_DECLS

#if PROTOBUF_C_VERSION_NUMBER < 1003000
# error This file was generated by a newer version of protoc-c which is incompatible with your libprotobuf-c headers. Please update your headers.
#elif 1003003 < PROTOBUF_C_MIN_COMPILER_VERSION
# error This file was generated by an older version of protoc-c which is incompatible with your libprotobuf-c headers. Please regenerate this file with a newer version of protoc-c.
#endif


typedef struct _Ciot__NtpCfg Ciot__NtpCfg;
typedef struct _Ciot__NtpStatus Ciot__NtpStatus;
typedef struct _Ciot__NtpReqData Ciot__NtpReqData;
typedef struct _Ciot__NtpReq Ciot__NtpReq;
typedef struct _Ciot__NtpData Ciot__NtpData;


/* --- enums --- */

/*
 * Enum representing the state of the NTP module.
 */
typedef enum _Ciot__NtpState {
  /*
   * NTP state: reset.
   */
  CIOT__NTP_STATE__NTP_STATE_RESET = 0,
  /*
   * NTP state: completed.
   */
  CIOT__NTP_STATE__NTP_STATE_COMPLETED = 1,
  /*
   * NTP state: in progress.
   */
  CIOT__NTP_STATE__NTP_STATE_IN_PROGRESS = 2
    PROTOBUF_C__FORCE_ENUM_TO_BE_INT_SIZE(CIOT__NTP_STATE)
} Ciot__NtpState;
/*
 * Enum representing different types of NTP requests.
 */
typedef enum _Ciot__NtpReqType {
  /*
   * Unknown NTP request type.
   */
  CIOT__NTP_REQ_TYPE__NTP_REQ_TYPE_UNKOWN = 0
    PROTOBUF_C__FORCE_ENUM_TO_BE_INT_SIZE(CIOT__NTP_REQ_TYPE)
} Ciot__NtpReqType;

/* --- messages --- */

/*
 * Message representing configuration for the NTP module.
 */
struct  _Ciot__NtpCfg
{
  ProtobufCMessage base;
  /*
   * Operation mode for NTP.
   */
  int32_t op_mode;
  /*
   * Synchronization mode for NTP.
   */
  int32_t sync_mode;
  /*
   * Synchronization interval for NTP.
   */
  int32_t sync_interval;
  /*
   * Timezone for NTP.
   */
  char *timezone;
  /*
   * NTP server 1.
   */
  char *server1;
  /*
   * NTP server 2.
   */
  char *server2;
  /*
   * NTP server 3.
   */
  char *server3;
};
#define CIOT__NTP_CFG__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&ciot__ntp_cfg__descriptor) \
    , 0, 0, 0, (char *)protobuf_c_empty_string, (char *)protobuf_c_empty_string, (char *)protobuf_c_empty_string, (char *)protobuf_c_empty_string }


/*
 * Message representing status for the NTP module.
 */
struct  _Ciot__NtpStatus
{
  ProtobufCMessage base;
  /*
   * State of the NTP module.
   */
  Ciot__NtpState state;
  /*
   * Timestamp of the last synchronization.
   */
  uint64_t last_sync;
  /*
   * Number of synchronizations.
   */
  uint32_t sync_count;
  /*
   * Initialization status.
   */
  protobuf_c_boolean init;
  /*
   * Synchronization status.
   */
  protobuf_c_boolean sync;
};
#define CIOT__NTP_STATUS__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&ciot__ntp_status__descriptor) \
    , CIOT__NTP_STATE__NTP_STATE_RESET, 0, 0, 0, 0 }


/*
 * Message representing data for an NTP request.
 */
struct  _Ciot__NtpReqData
{
  ProtobufCMessage base;
};
#define CIOT__NTP_REQ_DATA__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&ciot__ntp_req_data__descriptor) \
     }


/*
 * Message representing an NTP request.
 */
struct  _Ciot__NtpReq
{
  ProtobufCMessage base;
  /*
   * Type of the NTP request.
   */
  Ciot__NtpReqType type;
  /*
   * Data associated with the request.
   */
  Ciot__NtpReqData *data;
};
#define CIOT__NTP_REQ__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&ciot__ntp_req__descriptor) \
    , CIOT__NTP_REQ_TYPE__NTP_REQ_TYPE_UNKOWN, NULL }


/*
 * Message representing data for the NTP module.
 */
struct  _Ciot__NtpData
{
  ProtobufCMessage base;
  /*
   * Configuration for the NTP module.
   */
  Ciot__NtpCfg *config;
  /*
   * Status of the NTP module.
   */
  Ciot__NtpStatus *status;
  /*
   * NTP request data.
   */
  Ciot__NtpReq *request;
};
#define CIOT__NTP_DATA__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&ciot__ntp_data__descriptor) \
    , NULL, NULL, NULL }


/* Ciot__NtpCfg methods */
void   ciot__ntp_cfg__init
                     (Ciot__NtpCfg         *message);
size_t ciot__ntp_cfg__get_packed_size
                     (const Ciot__NtpCfg   *message);
size_t ciot__ntp_cfg__pack
                     (const Ciot__NtpCfg   *message,
                      uint8_t             *out);
size_t ciot__ntp_cfg__pack_to_buffer
                     (const Ciot__NtpCfg   *message,
                      ProtobufCBuffer     *buffer);
Ciot__NtpCfg *
       ciot__ntp_cfg__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   ciot__ntp_cfg__free_unpacked
                     (Ciot__NtpCfg *message,
                      ProtobufCAllocator *allocator);
/* Ciot__NtpStatus methods */
void   ciot__ntp_status__init
                     (Ciot__NtpStatus         *message);
size_t ciot__ntp_status__get_packed_size
                     (const Ciot__NtpStatus   *message);
size_t ciot__ntp_status__pack
                     (const Ciot__NtpStatus   *message,
                      uint8_t             *out);
size_t ciot__ntp_status__pack_to_buffer
                     (const Ciot__NtpStatus   *message,
                      ProtobufCBuffer     *buffer);
Ciot__NtpStatus *
       ciot__ntp_status__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   ciot__ntp_status__free_unpacked
                     (Ciot__NtpStatus *message,
                      ProtobufCAllocator *allocator);
/* Ciot__NtpReqData methods */
void   ciot__ntp_req_data__init
                     (Ciot__NtpReqData         *message);
size_t ciot__ntp_req_data__get_packed_size
                     (const Ciot__NtpReqData   *message);
size_t ciot__ntp_req_data__pack
                     (const Ciot__NtpReqData   *message,
                      uint8_t             *out);
size_t ciot__ntp_req_data__pack_to_buffer
                     (const Ciot__NtpReqData   *message,
                      ProtobufCBuffer     *buffer);
Ciot__NtpReqData *
       ciot__ntp_req_data__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   ciot__ntp_req_data__free_unpacked
                     (Ciot__NtpReqData *message,
                      ProtobufCAllocator *allocator);
/* Ciot__NtpReq methods */
void   ciot__ntp_req__init
                     (Ciot__NtpReq         *message);
size_t ciot__ntp_req__get_packed_size
                     (const Ciot__NtpReq   *message);
size_t ciot__ntp_req__pack
                     (const Ciot__NtpReq   *message,
                      uint8_t             *out);
size_t ciot__ntp_req__pack_to_buffer
                     (const Ciot__NtpReq   *message,
                      ProtobufCBuffer     *buffer);
Ciot__NtpReq *
       ciot__ntp_req__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   ciot__ntp_req__free_unpacked
                     (Ciot__NtpReq *message,
                      ProtobufCAllocator *allocator);
/* Ciot__NtpData methods */
void   ciot__ntp_data__init
                     (Ciot__NtpData         *message);
size_t ciot__ntp_data__get_packed_size
                     (const Ciot__NtpData   *message);
size_t ciot__ntp_data__pack
                     (const Ciot__NtpData   *message,
                      uint8_t             *out);
size_t ciot__ntp_data__pack_to_buffer
                     (const Ciot__NtpData   *message,
                      ProtobufCBuffer     *buffer);
Ciot__NtpData *
       ciot__ntp_data__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   ciot__ntp_data__free_unpacked
                     (Ciot__NtpData *message,
                      ProtobufCAllocator *allocator);
/* --- per-message closures --- */

typedef void (*Ciot__NtpCfg_Closure)
                 (const Ciot__NtpCfg *message,
                  void *closure_data);
typedef void (*Ciot__NtpStatus_Closure)
                 (const Ciot__NtpStatus *message,
                  void *closure_data);
typedef void (*Ciot__NtpReqData_Closure)
                 (const Ciot__NtpReqData *message,
                  void *closure_data);
typedef void (*Ciot__NtpReq_Closure)
                 (const Ciot__NtpReq *message,
                  void *closure_data);
typedef void (*Ciot__NtpData_Closure)
                 (const Ciot__NtpData *message,
                  void *closure_data);

/* --- services --- */


/* --- descriptors --- */

extern const ProtobufCEnumDescriptor    ciot__ntp_state__descriptor;
extern const ProtobufCEnumDescriptor    ciot__ntp_req_type__descriptor;
extern const ProtobufCMessageDescriptor ciot__ntp_cfg__descriptor;
extern const ProtobufCMessageDescriptor ciot__ntp_status__descriptor;
extern const ProtobufCMessageDescriptor ciot__ntp_req_data__descriptor;
extern const ProtobufCMessageDescriptor ciot__ntp_req__descriptor;
extern const ProtobufCMessageDescriptor ciot__ntp_data__descriptor;

PROTOBUF_C__END_DECLS


#endif  /* PROTOBUF_C_ciot_2fproto_2fv1_2fntp_2eproto__INCLUDED */