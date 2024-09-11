# -*- coding: utf-8 -*-
# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: ciot/proto/v1/http_server.proto

from google.protobuf.internal import enum_type_wrapper
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from google.protobuf import reflection as _reflection
from google.protobuf import symbol_database as _symbol_database
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()




DESCRIPTOR = _descriptor.FileDescriptor(
  name='ciot/proto/v1/http_server.proto',
  package='Ciot',
  syntax='proto3',
  serialized_options=None,
  create_key=_descriptor._internal_create_key,
  serialized_pb=b'\n\x1f\x63iot/proto/v1/http_server.proto\x12\x04\x43iot\"=\n\rHttpServerCfg\x12\x0f\n\x07\x61\x64\x64ress\x18\x01 \x01(\t\x12\r\n\x05route\x18\x02 \x01(\t\x12\x0c\n\x04port\x18\x03 \x01(\r\"G\n\x10HttpServerStatus\x12$\n\x05state\x18\x01 \x01(\x0e\x32\x15.Ciot.HttpServerState\x12\r\n\x05\x65rror\x18\x02 \x01(\x05\"6\n\rHttpServerReq\x12%\n\x04type\x18\x01 \x01(\x0e\x32\x17.Ciot.HttpServerReqType\"\x83\x01\n\x0eHttpServerData\x12#\n\x06\x63onfig\x18\x01 \x01(\x0b\x32\x13.Ciot.HttpServerCfg\x12&\n\x06status\x18\x02 \x01(\x0b\x32\x16.Ciot.HttpServerStatus\x12$\n\x07request\x18\x03 \x01(\x0b\x32\x13.Ciot.HttpServerReq*u\n\x0fHttpServerState\x12\x1d\n\x19HTTP_SERVER_STATE_STOPPED\x10\x00\x12\x1d\n\x19HTTP_SERVER_STATE_STARTED\x10\x01\x12$\n\x17HTTP_SERVER_STATE_ERROR\x10\xff\xff\xff\xff\xff\xff\xff\xff\xff\x01*4\n\x11HttpServerReqType\x12\x1f\n\x1bHTTP_SERVER_REQ_TYPE_UNKOWN\x10\x00\x62\x06proto3'
)

_HTTPSERVERSTATE = _descriptor.EnumDescriptor(
  name='HttpServerState',
  full_name='Ciot.HttpServerState',
  filename=None,
  file=DESCRIPTOR,
  create_key=_descriptor._internal_create_key,
  values=[
    _descriptor.EnumValueDescriptor(
      name='HTTP_SERVER_STATE_STOPPED', index=0, number=0,
      serialized_options=None,
      type=None,
      create_key=_descriptor._internal_create_key),
    _descriptor.EnumValueDescriptor(
      name='HTTP_SERVER_STATE_STARTED', index=1, number=1,
      serialized_options=None,
      type=None,
      create_key=_descriptor._internal_create_key),
    _descriptor.EnumValueDescriptor(
      name='HTTP_SERVER_STATE_ERROR', index=2, number=-1,
      serialized_options=None,
      type=None,
      create_key=_descriptor._internal_create_key),
  ],
  containing_type=None,
  serialized_options=None,
  serialized_start=367,
  serialized_end=484,
)
_sym_db.RegisterEnumDescriptor(_HTTPSERVERSTATE)

HttpServerState = enum_type_wrapper.EnumTypeWrapper(_HTTPSERVERSTATE)
_HTTPSERVERREQTYPE = _descriptor.EnumDescriptor(
  name='HttpServerReqType',
  full_name='Ciot.HttpServerReqType',
  filename=None,
  file=DESCRIPTOR,
  create_key=_descriptor._internal_create_key,
  values=[
    _descriptor.EnumValueDescriptor(
      name='HTTP_SERVER_REQ_TYPE_UNKOWN', index=0, number=0,
      serialized_options=None,
      type=None,
      create_key=_descriptor._internal_create_key),
  ],
  containing_type=None,
  serialized_options=None,
  serialized_start=486,
  serialized_end=538,
)
_sym_db.RegisterEnumDescriptor(_HTTPSERVERREQTYPE)

HttpServerReqType = enum_type_wrapper.EnumTypeWrapper(_HTTPSERVERREQTYPE)
HTTP_SERVER_STATE_STOPPED = 0
HTTP_SERVER_STATE_STARTED = 1
HTTP_SERVER_STATE_ERROR = -1
HTTP_SERVER_REQ_TYPE_UNKOWN = 0



_HTTPSERVERCFG = _descriptor.Descriptor(
  name='HttpServerCfg',
  full_name='Ciot.HttpServerCfg',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  create_key=_descriptor._internal_create_key,
  fields=[
    _descriptor.FieldDescriptor(
      name='address', full_name='Ciot.HttpServerCfg.address', index=0,
      number=1, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=b"".decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='route', full_name='Ciot.HttpServerCfg.route', index=1,
      number=2, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=b"".decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='port', full_name='Ciot.HttpServerCfg.port', index=2,
      number=3, type=13, cpp_type=3, label=1,
      has_default_value=False, default_value=0,
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
  serialized_start=41,
  serialized_end=102,
)


_HTTPSERVERSTATUS = _descriptor.Descriptor(
  name='HttpServerStatus',
  full_name='Ciot.HttpServerStatus',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  create_key=_descriptor._internal_create_key,
  fields=[
    _descriptor.FieldDescriptor(
      name='state', full_name='Ciot.HttpServerStatus.state', index=0,
      number=1, type=14, cpp_type=8, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='error', full_name='Ciot.HttpServerStatus.error', index=1,
      number=2, type=5, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
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
  serialized_start=104,
  serialized_end=175,
)


_HTTPSERVERREQ = _descriptor.Descriptor(
  name='HttpServerReq',
  full_name='Ciot.HttpServerReq',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  create_key=_descriptor._internal_create_key,
  fields=[
    _descriptor.FieldDescriptor(
      name='type', full_name='Ciot.HttpServerReq.type', index=0,
      number=1, type=14, cpp_type=8, label=1,
      has_default_value=False, default_value=0,
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
  serialized_start=177,
  serialized_end=231,
)


_HTTPSERVERDATA = _descriptor.Descriptor(
  name='HttpServerData',
  full_name='Ciot.HttpServerData',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  create_key=_descriptor._internal_create_key,
  fields=[
    _descriptor.FieldDescriptor(
      name='config', full_name='Ciot.HttpServerData.config', index=0,
      number=1, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='status', full_name='Ciot.HttpServerData.status', index=1,
      number=2, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='request', full_name='Ciot.HttpServerData.request', index=2,
      number=3, type=11, cpp_type=10, label=1,
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
  serialized_start=234,
  serialized_end=365,
)

_HTTPSERVERSTATUS.fields_by_name['state'].enum_type = _HTTPSERVERSTATE
_HTTPSERVERREQ.fields_by_name['type'].enum_type = _HTTPSERVERREQTYPE
_HTTPSERVERDATA.fields_by_name['config'].message_type = _HTTPSERVERCFG
_HTTPSERVERDATA.fields_by_name['status'].message_type = _HTTPSERVERSTATUS
_HTTPSERVERDATA.fields_by_name['request'].message_type = _HTTPSERVERREQ
DESCRIPTOR.message_types_by_name['HttpServerCfg'] = _HTTPSERVERCFG
DESCRIPTOR.message_types_by_name['HttpServerStatus'] = _HTTPSERVERSTATUS
DESCRIPTOR.message_types_by_name['HttpServerReq'] = _HTTPSERVERREQ
DESCRIPTOR.message_types_by_name['HttpServerData'] = _HTTPSERVERDATA
DESCRIPTOR.enum_types_by_name['HttpServerState'] = _HTTPSERVERSTATE
DESCRIPTOR.enum_types_by_name['HttpServerReqType'] = _HTTPSERVERREQTYPE
_sym_db.RegisterFileDescriptor(DESCRIPTOR)

HttpServerCfg = _reflection.GeneratedProtocolMessageType('HttpServerCfg', (_message.Message,), {
  'DESCRIPTOR' : _HTTPSERVERCFG,
  '__module__' : 'ciot.proto.v1.http_server_pb2'
  # @@protoc_insertion_point(class_scope:Ciot.HttpServerCfg)
  })
_sym_db.RegisterMessage(HttpServerCfg)

HttpServerStatus = _reflection.GeneratedProtocolMessageType('HttpServerStatus', (_message.Message,), {
  'DESCRIPTOR' : _HTTPSERVERSTATUS,
  '__module__' : 'ciot.proto.v1.http_server_pb2'
  # @@protoc_insertion_point(class_scope:Ciot.HttpServerStatus)
  })
_sym_db.RegisterMessage(HttpServerStatus)

HttpServerReq = _reflection.GeneratedProtocolMessageType('HttpServerReq', (_message.Message,), {
  'DESCRIPTOR' : _HTTPSERVERREQ,
  '__module__' : 'ciot.proto.v1.http_server_pb2'
  # @@protoc_insertion_point(class_scope:Ciot.HttpServerReq)
  })
_sym_db.RegisterMessage(HttpServerReq)

HttpServerData = _reflection.GeneratedProtocolMessageType('HttpServerData', (_message.Message,), {
  'DESCRIPTOR' : _HTTPSERVERDATA,
  '__module__' : 'ciot.proto.v1.http_server_pb2'
  # @@protoc_insertion_point(class_scope:Ciot.HttpServerData)
  })
_sym_db.RegisterMessage(HttpServerData)


# @@protoc_insertion_point(module_scope)
