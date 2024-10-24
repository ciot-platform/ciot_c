/* Generated by the protocol buffer compiler.  DO NOT EDIT! */
/* Generated from: ciot/proto/v1/ble_scn.proto */

#ifndef PROTOBUF_C_ciot_2fproto_2fv1_2fble_5fscn_2eproto__INCLUDED
#define PROTOBUF_C_ciot_2fproto_2fv1_2fble_5fscn_2eproto__INCLUDED

#include <protobuf-c/protobuf-c.h>

PROTOBUF_C__BEGIN_DECLS

#if PROTOBUF_C_VERSION_NUMBER < 1003000
# error This file was generated by a newer version of protoc-c which is incompatible with your libprotobuf-c headers. Please update your headers.
#elif 1003003 < PROTOBUF_C_MIN_COMPILER_VERSION
# error This file was generated by an older version of protoc-c which is incompatible with your libprotobuf-c headers. Please regenerate this file with a newer version of protoc-c.
#endif


typedef struct _Ciot__BleScnCfg Ciot__BleScnCfg;
typedef struct _Ciot__BleScnAdvInfo Ciot__BleScnAdvInfo;
typedef struct _Ciot__BleScnAdv Ciot__BleScnAdv;
typedef struct _Ciot__BleScnStatus Ciot__BleScnStatus;
typedef struct _Ciot__BleScnReq Ciot__BleScnReq;
typedef struct _Ciot__BleScnData Ciot__BleScnData;


/* --- enums --- */

/*
 * Possible ble scanner states
 */
typedef enum _Ciot__BleScnState {
  /*
   * Idle state
   */
  CIOT__BLE_SCN_STATE__BLE_SCN_STATE_IDLE = 0,
  /*
   * Passive scan enabled
   */
  CIOT__BLE_SCN_STATE__BLE_SCN_STATE_PASSIVE = 1,
  /*
   * Active scan enabled
   */
  CIOT__BLE_SCN_STATE__BLE_SCN_STATE_ACTIVE = 2
    PROTOBUF_C__FORCE_ENUM_TO_BE_INT_SIZE(CIOT__BLE_SCN_STATE)
} Ciot__BleScnState;
/*
 * Ble scanner request types
 */
typedef enum _Ciot__BleScnReqType {
  /*
   * Unknown request type
   */
  CIOT__BLE_SCN_REQ_TYPE__BLE_SCN_REQ_TYPE_UNKOWN = 0
    PROTOBUF_C__FORCE_ENUM_TO_BE_INT_SIZE(CIOT__BLE_SCN_REQ_TYPE)
} Ciot__BleScnReqType;

/* --- messages --- */

/*
 * Ble scanner configuration
 */
struct  _Ciot__BleScnCfg
{
  ProtobufCMessage base;
  /*
   * Scanner interval
   */
  uint32_t interval;
  /*
   * Scanner window
   */
  uint32_t window;
  /*
   * Scanner timeout
   */
  uint32_t timeout;
  /*
   * Enable/disable active scan mode
   */
  protobuf_c_boolean active;
  /*
   * Enable/disable bridge. On bridge mode ciot lib dosen't interpret adv
   * messages as an CioT requests and send all of them to the main application
   * as an data message.
   */
  protobuf_c_boolean bridge_mode;
};
#define CIOT__BLE_SCN_CFG__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&ciot__ble_scn_cfg__descriptor) \
    , 0, 0, 0, 0, 0 }


/*
 * Ble scanner advertisement information
 */
struct  _Ciot__BleScnAdvInfo
{
  ProtobufCMessage base;
  /*
   * Device mac
   */
  ProtobufCBinaryData mac;
  /*
   * Signal strenght
   */
  int32_t rssi;
};
#define CIOT__BLE_SCN_ADV_INFO__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&ciot__ble_scn_adv_info__descriptor) \
    , {0,NULL}, 0 }


/*
 * Ble scanner advertisement
 */
struct  _Ciot__BleScnAdv
{
  ProtobufCMessage base;
  /*
   * Advertisement information
   */
  Ciot__BleScnAdvInfo *info;
  /*
   * Advertisement payload
   */
  ProtobufCBinaryData payload;
};
#define CIOT__BLE_SCN_ADV__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&ciot__ble_scn_adv__descriptor) \
    , NULL, {0,NULL} }


/*
 * Ble scanner status
 */
struct  _Ciot__BleScnStatus
{
  ProtobufCMessage base;
  /*
   * Current state
   */
  Ciot__BleScnState state;
  /*
   * Current error code
   */
  int32_t err_code;
  /*
   * ADVs counter
   */
  int32_t advs_count;
  /*
   * ADVs filtered
   */
  int32_t advs_filtered;
  /*
   * Last event received
   */
  int32_t last_event;
  /*
   * Current fifo lenght
   */
  int32_t fifo_len;
  /*
   * Fifo maximum size
   */
  int32_t fifo_max;
};
#define CIOT__BLE_SCN_STATUS__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&ciot__ble_scn_status__descriptor) \
    , CIOT__BLE_SCN_STATE__BLE_SCN_STATE_IDLE, 0, 0, 0, 0, 0, 0 }


/*
 * Ble scanner request
 */
struct  _Ciot__BleScnReq
{
  ProtobufCMessage base;
  /*
   * Request type
   */
  Ciot__BleScnReqType type;
};
#define CIOT__BLE_SCN_REQ__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&ciot__ble_scn_req__descriptor) \
    , CIOT__BLE_SCN_REQ_TYPE__BLE_SCN_REQ_TYPE_UNKOWN }


/*
 * Ble scanner data
 */
struct  _Ciot__BleScnData
{
  ProtobufCMessage base;
  /*
   * Configuration
   */
  Ciot__BleScnCfg *config;
  /*
   * Status
   */
  Ciot__BleScnStatus *status;
  /*
   * Request
   */
  Ciot__BleScnReq *request;
};
#define CIOT__BLE_SCN_DATA__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&ciot__ble_scn_data__descriptor) \
    , NULL, NULL, NULL }


/* Ciot__BleScnCfg methods */
void   ciot__ble_scn_cfg__init
                     (Ciot__BleScnCfg         *message);
size_t ciot__ble_scn_cfg__get_packed_size
                     (const Ciot__BleScnCfg   *message);
size_t ciot__ble_scn_cfg__pack
                     (const Ciot__BleScnCfg   *message,
                      uint8_t             *out);
size_t ciot__ble_scn_cfg__pack_to_buffer
                     (const Ciot__BleScnCfg   *message,
                      ProtobufCBuffer     *buffer);
Ciot__BleScnCfg *
       ciot__ble_scn_cfg__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   ciot__ble_scn_cfg__free_unpacked
                     (Ciot__BleScnCfg *message,
                      ProtobufCAllocator *allocator);
/* Ciot__BleScnAdvInfo methods */
void   ciot__ble_scn_adv_info__init
                     (Ciot__BleScnAdvInfo         *message);
size_t ciot__ble_scn_adv_info__get_packed_size
                     (const Ciot__BleScnAdvInfo   *message);
size_t ciot__ble_scn_adv_info__pack
                     (const Ciot__BleScnAdvInfo   *message,
                      uint8_t             *out);
size_t ciot__ble_scn_adv_info__pack_to_buffer
                     (const Ciot__BleScnAdvInfo   *message,
                      ProtobufCBuffer     *buffer);
Ciot__BleScnAdvInfo *
       ciot__ble_scn_adv_info__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   ciot__ble_scn_adv_info__free_unpacked
                     (Ciot__BleScnAdvInfo *message,
                      ProtobufCAllocator *allocator);
/* Ciot__BleScnAdv methods */
void   ciot__ble_scn_adv__init
                     (Ciot__BleScnAdv         *message);
size_t ciot__ble_scn_adv__get_packed_size
                     (const Ciot__BleScnAdv   *message);
size_t ciot__ble_scn_adv__pack
                     (const Ciot__BleScnAdv   *message,
                      uint8_t             *out);
size_t ciot__ble_scn_adv__pack_to_buffer
                     (const Ciot__BleScnAdv   *message,
                      ProtobufCBuffer     *buffer);
Ciot__BleScnAdv *
       ciot__ble_scn_adv__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   ciot__ble_scn_adv__free_unpacked
                     (Ciot__BleScnAdv *message,
                      ProtobufCAllocator *allocator);
/* Ciot__BleScnStatus methods */
void   ciot__ble_scn_status__init
                     (Ciot__BleScnStatus         *message);
size_t ciot__ble_scn_status__get_packed_size
                     (const Ciot__BleScnStatus   *message);
size_t ciot__ble_scn_status__pack
                     (const Ciot__BleScnStatus   *message,
                      uint8_t             *out);
size_t ciot__ble_scn_status__pack_to_buffer
                     (const Ciot__BleScnStatus   *message,
                      ProtobufCBuffer     *buffer);
Ciot__BleScnStatus *
       ciot__ble_scn_status__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   ciot__ble_scn_status__free_unpacked
                     (Ciot__BleScnStatus *message,
                      ProtobufCAllocator *allocator);
/* Ciot__BleScnReq methods */
void   ciot__ble_scn_req__init
                     (Ciot__BleScnReq         *message);
size_t ciot__ble_scn_req__get_packed_size
                     (const Ciot__BleScnReq   *message);
size_t ciot__ble_scn_req__pack
                     (const Ciot__BleScnReq   *message,
                      uint8_t             *out);
size_t ciot__ble_scn_req__pack_to_buffer
                     (const Ciot__BleScnReq   *message,
                      ProtobufCBuffer     *buffer);
Ciot__BleScnReq *
       ciot__ble_scn_req__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   ciot__ble_scn_req__free_unpacked
                     (Ciot__BleScnReq *message,
                      ProtobufCAllocator *allocator);
/* Ciot__BleScnData methods */
void   ciot__ble_scn_data__init
                     (Ciot__BleScnData         *message);
size_t ciot__ble_scn_data__get_packed_size
                     (const Ciot__BleScnData   *message);
size_t ciot__ble_scn_data__pack
                     (const Ciot__BleScnData   *message,
                      uint8_t             *out);
size_t ciot__ble_scn_data__pack_to_buffer
                     (const Ciot__BleScnData   *message,
                      ProtobufCBuffer     *buffer);
Ciot__BleScnData *
       ciot__ble_scn_data__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   ciot__ble_scn_data__free_unpacked
                     (Ciot__BleScnData *message,
                      ProtobufCAllocator *allocator);
/* --- per-message closures --- */

typedef void (*Ciot__BleScnCfg_Closure)
                 (const Ciot__BleScnCfg *message,
                  void *closure_data);
typedef void (*Ciot__BleScnAdvInfo_Closure)
                 (const Ciot__BleScnAdvInfo *message,
                  void *closure_data);
typedef void (*Ciot__BleScnAdv_Closure)
                 (const Ciot__BleScnAdv *message,
                  void *closure_data);
typedef void (*Ciot__BleScnStatus_Closure)
                 (const Ciot__BleScnStatus *message,
                  void *closure_data);
typedef void (*Ciot__BleScnReq_Closure)
                 (const Ciot__BleScnReq *message,
                  void *closure_data);
typedef void (*Ciot__BleScnData_Closure)
                 (const Ciot__BleScnData *message,
                  void *closure_data);

/* --- services --- */


/* --- descriptors --- */

extern const ProtobufCEnumDescriptor    ciot__ble_scn_state__descriptor;
extern const ProtobufCEnumDescriptor    ciot__ble_scn_req_type__descriptor;
extern const ProtobufCMessageDescriptor ciot__ble_scn_cfg__descriptor;
extern const ProtobufCMessageDescriptor ciot__ble_scn_adv_info__descriptor;
extern const ProtobufCMessageDescriptor ciot__ble_scn_adv__descriptor;
extern const ProtobufCMessageDescriptor ciot__ble_scn_status__descriptor;
extern const ProtobufCMessageDescriptor ciot__ble_scn_req__descriptor;
extern const ProtobufCMessageDescriptor ciot__ble_scn_data__descriptor;

PROTOBUF_C__END_DECLS


#endif  /* PROTOBUF_C_ciot_2fproto_2fv1_2fble_5fscn_2eproto__INCLUDED */
