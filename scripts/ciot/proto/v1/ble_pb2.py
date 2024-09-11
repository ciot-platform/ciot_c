# -*- coding: utf-8 -*-
# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: ciot/proto/v1/ble.proto

from google.protobuf.internal import enum_type_wrapper
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from google.protobuf import reflection as _reflection
from google.protobuf import symbol_database as _symbol_database
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()




DESCRIPTOR = _descriptor.FileDescriptor(
  name='ciot/proto/v1/ble.proto',
  package='Ciot',
  syntax='proto3',
  serialized_options=None,
  create_key=_descriptor._internal_create_key,
  serialized_pb=b'\n\x17\x63iot/proto/v1/ble.proto\x12\x04\x43iot\"\x15\n\x06\x42leCfg\x12\x0b\n\x03mac\x18\x01 \x01(\x0c\")\n\x07\x42leInfo\x12\x0e\n\x06hw_mac\x18\x01 \x01(\x0c\x12\x0e\n\x06sw_mac\x18\x02 \x01(\x0c\"R\n\tBleStatus\x12\x1d\n\x05state\x18\x01 \x01(\x0e\x32\x0e.Ciot.BleState\x12\x10\n\x08\x65rr_code\x18\x02 \x01(\x05\x12\x14\n\x0cusing_sw_mac\x18\x03 \x01(\x08\"9\n\x06\x42leReq\x12\x1e\n\x04type\x18\x01 \x01(\x0e\x32\x10.Ciot.BleReqType\x12\x0f\n\x07set_mac\x18\x02 \x01(\x0c\"\x84\x01\n\x07\x42leData\x12\x1c\n\x06\x63onfig\x18\x01 \x01(\x0b\x32\x0c.Ciot.BleCfg\x12\x1f\n\x06status\x18\x02 \x01(\x0b\x32\x0f.Ciot.BleStatus\x12\x1d\n\x07request\x18\x03 \x01(\x0b\x32\x0c.Ciot.BleReq\x12\x1b\n\x04info\x18\x04 \x01(\x0b\x32\r.Ciot.BleInfo*5\n\x08\x42leState\x12\x12\n\x0e\x42LE_STATE_IDLE\x10\x00\x12\x15\n\x11\x42LE_STATE_STARTED\x10\x01*?\n\nBleReqType\x12\x17\n\x13\x42LE_REQ_TYPE_UNKOWN\x10\x00\x12\x18\n\x14\x42LE_REQ_TYPE_SET_MAC\x10\x01\x62\x06proto3'
)

_BLESTATE = _descriptor.EnumDescriptor(
  name='BleState',
  full_name='Ciot.BleState',
  filename=None,
  file=DESCRIPTOR,
  create_key=_descriptor._internal_create_key,
  values=[
    _descriptor.EnumValueDescriptor(
      name='BLE_STATE_IDLE', index=0, number=0,
      serialized_options=None,
      type=None,
      create_key=_descriptor._internal_create_key),
    _descriptor.EnumValueDescriptor(
      name='BLE_STATE_STARTED', index=1, number=1,
      serialized_options=None,
      type=None,
      create_key=_descriptor._internal_create_key),
  ],
  containing_type=None,
  serialized_options=None,
  serialized_start=377,
  serialized_end=430,
)
_sym_db.RegisterEnumDescriptor(_BLESTATE)

BleState = enum_type_wrapper.EnumTypeWrapper(_BLESTATE)
_BLEREQTYPE = _descriptor.EnumDescriptor(
  name='BleReqType',
  full_name='Ciot.BleReqType',
  filename=None,
  file=DESCRIPTOR,
  create_key=_descriptor._internal_create_key,
  values=[
    _descriptor.EnumValueDescriptor(
      name='BLE_REQ_TYPE_UNKOWN', index=0, number=0,
      serialized_options=None,
      type=None,
      create_key=_descriptor._internal_create_key),
    _descriptor.EnumValueDescriptor(
      name='BLE_REQ_TYPE_SET_MAC', index=1, number=1,
      serialized_options=None,
      type=None,
      create_key=_descriptor._internal_create_key),
  ],
  containing_type=None,
  serialized_options=None,
  serialized_start=432,
  serialized_end=495,
)
_sym_db.RegisterEnumDescriptor(_BLEREQTYPE)

BleReqType = enum_type_wrapper.EnumTypeWrapper(_BLEREQTYPE)
BLE_STATE_IDLE = 0
BLE_STATE_STARTED = 1
BLE_REQ_TYPE_UNKOWN = 0
BLE_REQ_TYPE_SET_MAC = 1



_BLECFG = _descriptor.Descriptor(
  name='BleCfg',
  full_name='Ciot.BleCfg',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  create_key=_descriptor._internal_create_key,
  fields=[
    _descriptor.FieldDescriptor(
      name='mac', full_name='Ciot.BleCfg.mac', index=0,
      number=1, type=12, cpp_type=9, label=1,
      has_default_value=False, default_value=b"",
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=33,
  serialized_end=54,
)


_BLEINFO = _descriptor.Descriptor(
  name='BleInfo',
  full_name='Ciot.BleInfo',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  create_key=_descriptor._internal_create_key,
  fields=[
    _descriptor.FieldDescriptor(
      name='hw_mac', full_name='Ciot.BleInfo.hw_mac', index=0,
      number=1, type=12, cpp_type=9, label=1,
      has_default_value=False, default_value=b"",
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='sw_mac', full_name='Ciot.BleInfo.sw_mac', index=1,
      number=2, type=12, cpp_type=9, label=1,
      has_default_value=False, default_value=b"",
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=56,
  serialized_end=97,
)


_BLESTATUS = _descriptor.Descriptor(
  name='BleStatus',
  full_name='Ciot.BleStatus',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  create_key=_descriptor._internal_create_key,
  fields=[
    _descriptor.FieldDescriptor(
      name='state', full_name='Ciot.BleStatus.state', index=0,
      number=1, type=14, cpp_type=8, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='err_code', full_name='Ciot.BleStatus.err_code', index=1,
      number=2, type=5, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='using_sw_mac', full_name='Ciot.BleStatus.using_sw_mac', index=2,
      number=3, type=8, cpp_type=7, label=1,
      has_default_value=False, default_value=False,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=99,
  serialized_end=181,
)


_BLEREQ = _descriptor.Descriptor(
  name='BleReq',
  full_name='Ciot.BleReq',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  create_key=_descriptor._internal_create_key,
  fields=[
    _descriptor.FieldDescriptor(
      name='type', full_name='Ciot.BleReq.type', index=0,
      number=1, type=14, cpp_type=8, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='set_mac', full_name='Ciot.BleReq.set_mac', index=1,
      number=2, type=12, cpp_type=9, label=1,
      has_default_value=False, default_value=b"",
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=183,
  serialized_end=240,
)


_BLEDATA = _descriptor.Descriptor(
  name='BleData',
  full_name='Ciot.BleData',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  create_key=_descriptor._internal_create_key,
  fields=[
    _descriptor.FieldDescriptor(
      name='config', full_name='Ciot.BleData.config', index=0,
      number=1, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='status', full_name='Ciot.BleData.status', index=1,
      number=2, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='request', full_name='Ciot.BleData.request', index=2,
      number=3, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='info', full_name='Ciot.BleData.info', index=3,
      number=4, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=243,
  serialized_end=375,
)

_BLESTATUS.fields_by_name['state'].enum_type = _BLESTATE
_BLEREQ.fields_by_name['type'].enum_type = _BLEREQTYPE
_BLEDATA.fields_by_name['config'].message_type = _BLECFG
_BLEDATA.fields_by_name['status'].message_type = _BLESTATUS
_BLEDATA.fields_by_name['request'].message_type = _BLEREQ
_BLEDATA.fields_by_name['info'].message_type = _BLEINFO
DESCRIPTOR.message_types_by_name['BleCfg'] = _BLECFG
DESCRIPTOR.message_types_by_name['BleInfo'] = _BLEINFO
DESCRIPTOR.message_types_by_name['BleStatus'] = _BLESTATUS
DESCRIPTOR.message_types_by_name['BleReq'] = _BLEREQ
DESCRIPTOR.message_types_by_name['BleData'] = _BLEDATA
DESCRIPTOR.enum_types_by_name['BleState'] = _BLESTATE
DESCRIPTOR.enum_types_by_name['BleReqType'] = _BLEREQTYPE
_sym_db.RegisterFileDescriptor(DESCRIPTOR)

BleCfg = _reflection.GeneratedProtocolMessageType('BleCfg', (_message.Message,), {
  'DESCRIPTOR' : _BLECFG,
  '__module__' : 'ciot.proto.v1.ble_pb2'
  # @@protoc_insertion_point(class_scope:Ciot.BleCfg)
  })
_sym_db.RegisterMessage(BleCfg)

BleInfo = _reflection.GeneratedProtocolMessageType('BleInfo', (_message.Message,), {
  'DESCRIPTOR' : _BLEINFO,
  '__module__' : 'ciot.proto.v1.ble_pb2'
  # @@protoc_insertion_point(class_scope:Ciot.BleInfo)
  })
_sym_db.RegisterMessage(BleInfo)

BleStatus = _reflection.GeneratedProtocolMessageType('BleStatus', (_message.Message,), {
  'DESCRIPTOR' : _BLESTATUS,
  '__module__' : 'ciot.proto.v1.ble_pb2'
  # @@protoc_insertion_point(class_scope:Ciot.BleStatus)
  })
_sym_db.RegisterMessage(BleStatus)

BleReq = _reflection.GeneratedProtocolMessageType('BleReq', (_message.Message,), {
  'DESCRIPTOR' : _BLEREQ,
  '__module__' : 'ciot.proto.v1.ble_pb2'
  # @@protoc_insertion_point(class_scope:Ciot.BleReq)
  })
_sym_db.RegisterMessage(BleReq)

BleData = _reflection.GeneratedProtocolMessageType('BleData', (_message.Message,), {
  'DESCRIPTOR' : _BLEDATA,
  '__module__' : 'ciot.proto.v1.ble_pb2'
  # @@protoc_insertion_point(class_scope:Ciot.BleData)
  })
_sym_db.RegisterMessage(BleData)


# @@protoc_insertion_point(module_scope)
