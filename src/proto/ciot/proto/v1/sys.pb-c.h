/* Generated by the protocol buffer compiler.  DO NOT EDIT! */
/* Generated from: ciot/proto/v1/sys.proto */

#ifndef PROTOBUF_C_ciot_2fproto_2fv1_2fsys_2eproto__INCLUDED
#define PROTOBUF_C_ciot_2fproto_2fv1_2fsys_2eproto__INCLUDED

#include <protobuf-c/protobuf-c.h>

PROTOBUF_C__BEGIN_DECLS

#if PROTOBUF_C_VERSION_NUMBER < 1003000
# error This file was generated by a newer version of protoc-c which is incompatible with your libprotobuf-c headers. Please update your headers.
#elif 1003003 < PROTOBUF_C_MIN_COMPILER_VERSION
# error This file was generated by an older version of protoc-c which is incompatible with your libprotobuf-c headers. Please regenerate this file with a newer version of protoc-c.
#endif


typedef struct _Ciot__SysCfg Ciot__SysCfg;
typedef struct _Ciot__SysHwFeatures Ciot__SysHwFeatures;
typedef struct _Ciot__SysSwFeatures Ciot__SysSwFeatures;
typedef struct _Ciot__SysFeatures Ciot__SysFeatures;
typedef struct _Ciot__SysInfo Ciot__SysInfo;
typedef struct _Ciot__SysStatus Ciot__SysStatus;
typedef struct _Ciot__SysReq Ciot__SysReq;
typedef struct _Ciot__SysData Ciot__SysData;


/* --- enums --- */

/*
 * Enum representing different types of system requests.
 */
typedef enum _Ciot__SysReqType {
  /*
   * Unknown system request type.
   */
  CIOT__SYS_REQ_TYPE__SYS_REQ_TYPE_UNKOWN = 0,
  /*
   * System restart request.
   */
  CIOT__SYS_REQ_TYPE__SYS_REQ_TYPE_RESTART = 1,
  /*
   * System init DFU bootloader
   */
  CIOT__SYS_REQ_TYPE__SYS_REQ_TYPE_INIT_DFU = 2
    PROTOBUF_C__FORCE_ENUM_TO_BE_INT_SIZE(CIOT__SYS_REQ_TYPE)
} Ciot__SysReqType;
/*
 * Enum representing hardware version
 */
typedef enum _Ciot__SysHw {
  /*
   * Unknown hardware
   */
  CIOT__SYS_HW__SYS_HW_UNKNOWN = 0,
  /*
   * ESP8266
   */
  CIOT__SYS_HW__SYS_HW_ESP8266 = 1,
  /*
   * Arduino Board
   */
  CIOT__SYS_HW__SYS_HW_ARDUINO = 2,
  /*
   * ESP32
   */
  CIOT__SYS_HW__SYS_HW_ESP32 = 3,
  /*
   * NRF51
   */
  CIOT__SYS_HW__SYS_HW_NRF51 = 4,
  /*
   * NRF52
   */
  CIOT__SYS_HW__SYS_HW_NRF52 = 5,
  /*
   * Linux device
   */
  CIOT__SYS_HW__SYS_HW_LINUX = 6,
  /*
   * Windows device
   */
  CIOT__SYS_HW__SYS_HW_WIN32 = 7
    PROTOBUF_C__FORCE_ENUM_TO_BE_INT_SIZE(CIOT__SYS_HW)
} Ciot__SysHw;

/* --- messages --- */

/*
 * Message representing configuration for the system.
 */
struct  _Ciot__SysCfg
{
  ProtobufCMessage base;
};
#define CIOT__SYS_CFG__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&ciot__sys_cfg__descriptor) \
     }


/*
 * Enum representing hardware features of the system.
 */
struct  _Ciot__SysHwFeatures
{
  ProtobufCMessage base;
  /*
   * Storage feature.
   */
  protobuf_c_boolean storage;
  /*
   * System feature.
   */
  protobuf_c_boolean sys;
  /*
   * UART feature.
   */
  protobuf_c_boolean uart;
  /*
   * USB feature.
   */
  protobuf_c_boolean usb;
  /*
   * Ethernet feature.
   */
  protobuf_c_boolean ethernet;
  /*
   * WiFi feature.
   */
  protobuf_c_boolean wifi;
  /*
   * Bluetooth Low Energy (BLE) Scanner feature.
   */
  protobuf_c_boolean ble_scn;
  /*
   * GPIO feature
   */
  protobuf_c_boolean gpio;
};
#define CIOT__SYS_HW_FEATURES__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&ciot__sys_hw_features__descriptor) \
    , 0, 0, 0, 0, 0, 0, 0, 0 }


/*
 * Enum representing software features of the system.
 */
struct  _Ciot__SysSwFeatures
{
  ProtobufCMessage base;
  /*
   * NTP feature.
   */
  protobuf_c_boolean ntp;
  /*
   * DFU feature
   */
  protobuf_c_boolean dfu;
  /*
   * OTA (Over-the-Air update) feature.
   */
  protobuf_c_boolean ota;
  /*
   * HTTP client feature.
   */
  protobuf_c_boolean http_client;
  /*
   * HTTP server feature.
   */
  protobuf_c_boolean http_server;
  /*
   * MQTT client feature.
   */
  protobuf_c_boolean mqtt_client;
  /*
   * Timer feature.
   */
  protobuf_c_boolean timer;
};
#define CIOT__SYS_SW_FEATURES__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&ciot__sys_sw_features__descriptor) \
    , 0, 0, 0, 0, 0, 0, 0 }


/*
 * Message representing system features.
 */
struct  _Ciot__SysFeatures
{
  ProtobufCMessage base;
  /*
   * Hardware features bitmask.
   */
  Ciot__SysHwFeatures *hw;
  /*
   * Software features bitmask.
   */
  Ciot__SysSwFeatures *sw;
};
#define CIOT__SYS_FEATURES__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&ciot__sys_features__descriptor) \
    , NULL, NULL }


/*
 * Message representing system information.
 */
struct  _Ciot__SysInfo
{
  ProtobufCMessage base;
  /*
   * Hardware name.
   */
  char *hw_name;
  /*
   * Application version.
   */
  ProtobufCBinaryData app_ver;
  /*
   * Hardware type.
   */
  Ciot__SysHw hardware;
  /*
   * System features.
   */
  Ciot__SysFeatures *features;
};
#define CIOT__SYS_INFO__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&ciot__sys_info__descriptor) \
    , (char *)protobuf_c_empty_string, {0,NULL}, CIOT__SYS_HW__SYS_HW_UNKNOWN, NULL }


/*
 * Message representing system status.
 */
struct  _Ciot__SysStatus
{
  ProtobufCMessage base;
  /*
   * Reason for the last reset.
   */
  uint32_t reset_reason;
  /*
   * Number of resets.
   */
  uint32_t reset_count;
  /*
   * Amount of free memory.
   */
  uint32_t free_memory;
  /*
   * Lifetime of the system.
   */
  uint32_t lifetime;
};
#define CIOT__SYS_STATUS__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&ciot__sys_status__descriptor) \
    , 0, 0, 0, 0 }


/*
 * Message representing a system request.
 */
struct  _Ciot__SysReq
{
  ProtobufCMessage base;
  /*
   * Type of the system request.
   */
  Ciot__SysReqType type;
};
#define CIOT__SYS_REQ__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&ciot__sys_req__descriptor) \
    , CIOT__SYS_REQ_TYPE__SYS_REQ_TYPE_UNKOWN }


/*
 * Message representing data for the system.
 */
struct  _Ciot__SysData
{
  ProtobufCMessage base;
  /*
   * Configuration for the system.
   */
  Ciot__SysCfg *config;
  /*
   * Status of the system.
   */
  Ciot__SysStatus *status;
  /*
   * System request data.
   */
  Ciot__SysReq *request;
  /*
   * System info.
   */
  Ciot__SysInfo *info;
};
#define CIOT__SYS_DATA__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&ciot__sys_data__descriptor) \
    , NULL, NULL, NULL, NULL }


/* Ciot__SysCfg methods */
void   ciot__sys_cfg__init
                     (Ciot__SysCfg         *message);
size_t ciot__sys_cfg__get_packed_size
                     (const Ciot__SysCfg   *message);
size_t ciot__sys_cfg__pack
                     (const Ciot__SysCfg   *message,
                      uint8_t             *out);
size_t ciot__sys_cfg__pack_to_buffer
                     (const Ciot__SysCfg   *message,
                      ProtobufCBuffer     *buffer);
Ciot__SysCfg *
       ciot__sys_cfg__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   ciot__sys_cfg__free_unpacked
                     (Ciot__SysCfg *message,
                      ProtobufCAllocator *allocator);
/* Ciot__SysHwFeatures methods */
void   ciot__sys_hw_features__init
                     (Ciot__SysHwFeatures         *message);
size_t ciot__sys_hw_features__get_packed_size
                     (const Ciot__SysHwFeatures   *message);
size_t ciot__sys_hw_features__pack
                     (const Ciot__SysHwFeatures   *message,
                      uint8_t             *out);
size_t ciot__sys_hw_features__pack_to_buffer
                     (const Ciot__SysHwFeatures   *message,
                      ProtobufCBuffer     *buffer);
Ciot__SysHwFeatures *
       ciot__sys_hw_features__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   ciot__sys_hw_features__free_unpacked
                     (Ciot__SysHwFeatures *message,
                      ProtobufCAllocator *allocator);
/* Ciot__SysSwFeatures methods */
void   ciot__sys_sw_features__init
                     (Ciot__SysSwFeatures         *message);
size_t ciot__sys_sw_features__get_packed_size
                     (const Ciot__SysSwFeatures   *message);
size_t ciot__sys_sw_features__pack
                     (const Ciot__SysSwFeatures   *message,
                      uint8_t             *out);
size_t ciot__sys_sw_features__pack_to_buffer
                     (const Ciot__SysSwFeatures   *message,
                      ProtobufCBuffer     *buffer);
Ciot__SysSwFeatures *
       ciot__sys_sw_features__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   ciot__sys_sw_features__free_unpacked
                     (Ciot__SysSwFeatures *message,
                      ProtobufCAllocator *allocator);
/* Ciot__SysFeatures methods */
void   ciot__sys_features__init
                     (Ciot__SysFeatures         *message);
size_t ciot__sys_features__get_packed_size
                     (const Ciot__SysFeatures   *message);
size_t ciot__sys_features__pack
                     (const Ciot__SysFeatures   *message,
                      uint8_t             *out);
size_t ciot__sys_features__pack_to_buffer
                     (const Ciot__SysFeatures   *message,
                      ProtobufCBuffer     *buffer);
Ciot__SysFeatures *
       ciot__sys_features__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   ciot__sys_features__free_unpacked
                     (Ciot__SysFeatures *message,
                      ProtobufCAllocator *allocator);
/* Ciot__SysInfo methods */
void   ciot__sys_info__init
                     (Ciot__SysInfo         *message);
size_t ciot__sys_info__get_packed_size
                     (const Ciot__SysInfo   *message);
size_t ciot__sys_info__pack
                     (const Ciot__SysInfo   *message,
                      uint8_t             *out);
size_t ciot__sys_info__pack_to_buffer
                     (const Ciot__SysInfo   *message,
                      ProtobufCBuffer     *buffer);
Ciot__SysInfo *
       ciot__sys_info__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   ciot__sys_info__free_unpacked
                     (Ciot__SysInfo *message,
                      ProtobufCAllocator *allocator);
/* Ciot__SysStatus methods */
void   ciot__sys_status__init
                     (Ciot__SysStatus         *message);
size_t ciot__sys_status__get_packed_size
                     (const Ciot__SysStatus   *message);
size_t ciot__sys_status__pack
                     (const Ciot__SysStatus   *message,
                      uint8_t             *out);
size_t ciot__sys_status__pack_to_buffer
                     (const Ciot__SysStatus   *message,
                      ProtobufCBuffer     *buffer);
Ciot__SysStatus *
       ciot__sys_status__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   ciot__sys_status__free_unpacked
                     (Ciot__SysStatus *message,
                      ProtobufCAllocator *allocator);
/* Ciot__SysReq methods */
void   ciot__sys_req__init
                     (Ciot__SysReq         *message);
size_t ciot__sys_req__get_packed_size
                     (const Ciot__SysReq   *message);
size_t ciot__sys_req__pack
                     (const Ciot__SysReq   *message,
                      uint8_t             *out);
size_t ciot__sys_req__pack_to_buffer
                     (const Ciot__SysReq   *message,
                      ProtobufCBuffer     *buffer);
Ciot__SysReq *
       ciot__sys_req__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   ciot__sys_req__free_unpacked
                     (Ciot__SysReq *message,
                      ProtobufCAllocator *allocator);
/* Ciot__SysData methods */
void   ciot__sys_data__init
                     (Ciot__SysData         *message);
size_t ciot__sys_data__get_packed_size
                     (const Ciot__SysData   *message);
size_t ciot__sys_data__pack
                     (const Ciot__SysData   *message,
                      uint8_t             *out);
size_t ciot__sys_data__pack_to_buffer
                     (const Ciot__SysData   *message,
                      ProtobufCBuffer     *buffer);
Ciot__SysData *
       ciot__sys_data__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   ciot__sys_data__free_unpacked
                     (Ciot__SysData *message,
                      ProtobufCAllocator *allocator);
/* --- per-message closures --- */

typedef void (*Ciot__SysCfg_Closure)
                 (const Ciot__SysCfg *message,
                  void *closure_data);
typedef void (*Ciot__SysHwFeatures_Closure)
                 (const Ciot__SysHwFeatures *message,
                  void *closure_data);
typedef void (*Ciot__SysSwFeatures_Closure)
                 (const Ciot__SysSwFeatures *message,
                  void *closure_data);
typedef void (*Ciot__SysFeatures_Closure)
                 (const Ciot__SysFeatures *message,
                  void *closure_data);
typedef void (*Ciot__SysInfo_Closure)
                 (const Ciot__SysInfo *message,
                  void *closure_data);
typedef void (*Ciot__SysStatus_Closure)
                 (const Ciot__SysStatus *message,
                  void *closure_data);
typedef void (*Ciot__SysReq_Closure)
                 (const Ciot__SysReq *message,
                  void *closure_data);
typedef void (*Ciot__SysData_Closure)
                 (const Ciot__SysData *message,
                  void *closure_data);

/* --- services --- */


/* --- descriptors --- */

extern const ProtobufCEnumDescriptor    ciot__sys_req_type__descriptor;
extern const ProtobufCEnumDescriptor    ciot__sys_hw__descriptor;
extern const ProtobufCMessageDescriptor ciot__sys_cfg__descriptor;
extern const ProtobufCMessageDescriptor ciot__sys_hw_features__descriptor;
extern const ProtobufCMessageDescriptor ciot__sys_sw_features__descriptor;
extern const ProtobufCMessageDescriptor ciot__sys_features__descriptor;
extern const ProtobufCMessageDescriptor ciot__sys_info__descriptor;
extern const ProtobufCMessageDescriptor ciot__sys_status__descriptor;
extern const ProtobufCMessageDescriptor ciot__sys_req__descriptor;
extern const ProtobufCMessageDescriptor ciot__sys_data__descriptor;

PROTOBUF_C__END_DECLS


#endif  /* PROTOBUF_C_ciot_2fproto_2fv1_2fsys_2eproto__INCLUDED */
