# -*- coding: utf-8 -*-
# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: ciot/proto/v1/usb.proto
# Protobuf Python Version: 5.26.1
"""Generated protocol buffer code."""
from google.protobuf import descriptor as _descriptor
from google.protobuf import descriptor_pool as _descriptor_pool
from google.protobuf import symbol_database as _symbol_database
from google.protobuf.internal import builder as _builder
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()




DESCRIPTOR = _descriptor_pool.Default().AddSerializedFile(b'\n\x17\x63iot/proto/v1/usb.proto\x12\x04\x43iot\"\x1d\n\x06UsbCfg\x12\x13\n\x0b\x62ridge_mode\x18\x01 \x01(\x08\"*\n\tUsbStatus\x12\x1d\n\x05state\x18\x01 \x01(\x0e\x32\x0e.Ciot.UsbState\"6\n\x06UsbReq\x12\x1e\n\x04type\x18\x01 \x01(\x0e\x32\x10.Ciot.UsbReqType\x12\x0c\n\x04\x64\x61ta\x18\x02 \x01(\x0c\"g\n\x07UsbData\x12\x1c\n\x06\x63onfig\x18\x01 \x01(\x0b\x32\x0c.Ciot.UsbCfg\x12\x1f\n\x06status\x18\x02 \x01(\x0b\x32\x0f.Ciot.UsbStatus\x12\x1d\n\x07request\x18\x03 \x01(\x0b\x32\x0c.Ciot.UsbReq*T\n\x08UsbState\x12\x15\n\x11USB_STATE_STOPPED\x10\x00\x12\x15\n\x11USB_STATE_STARTED\x10\x01\x12\x1a\n\x16USB_STATE_CIOT_S_ERROR\x10\x03*\x83\x01\n\nUsbReqType\x12\x17\n\x13USB_REQ_TYPE_UNKOWN\x10\x00\x12\x1a\n\x16USB_REQ_TYPE_SEND_DATA\x10\x01\x12\x1b\n\x17USB_REQ_TYPE_SEND_BYTES\x10\x02\x12#\n\x1fUSB_REQ_TYPE_ENABLE_BRIDGE_MODE\x10\x03\x62\x06proto3')

_globals = globals()
_builder.BuildMessageAndEnumDescriptors(DESCRIPTOR, _globals)
_builder.BuildTopDescriptorsAndMessages(DESCRIPTOR, 'ciot.proto.v1.usb_pb2', _globals)
if not _descriptor._USE_C_DESCRIPTORS:
  DESCRIPTOR._loaded_options = None
  _globals['_USBSTATE']._serialized_start=269
  _globals['_USBSTATE']._serialized_end=353
  _globals['_USBREQTYPE']._serialized_start=356
  _globals['_USBREQTYPE']._serialized_end=487
  _globals['_USBCFG']._serialized_start=33
  _globals['_USBCFG']._serialized_end=62
  _globals['_USBSTATUS']._serialized_start=64
  _globals['_USBSTATUS']._serialized_end=106
  _globals['_USBREQ']._serialized_start=108
  _globals['_USBREQ']._serialized_end=162
  _globals['_USBDATA']._serialized_start=164
  _globals['_USBDATA']._serialized_end=267
# @@protoc_insertion_point(module_scope)
