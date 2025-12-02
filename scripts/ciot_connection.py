from abc import ABC, abstractmethod
from ciot.proto.v1.msg_pb2 import Msg
import requests;
import struct;
import serial;

id = 0

class CiotConnection(ABC):
    def __init__(self, config):
        self.config = config

    @abstractmethod
    def name():
        pass

    def send_msg(self, msg: Msg):
        global id
        msg.id = id
        id = id + 1

    @abstractmethod
    def read(self):
        pass

class CiotHttpClient(CiotConnection):
    def endpoint(self):
        return self.config

    def name(self):
        return self.config

    def send_msg(self, msg: Msg):
        super().send_msg(msg)
        response = requests.post(self.config, msg.SerializeToString(), timeout=5)
        msg.ParseFromString(response.content)
        return msg
    
    def read(sekf):
        pass

class CiotSerialCfg():
    def __init__(self, port, baudrate):
        self.port = port
        self.baudrate = baudrate

class CiotSerial(CiotConnection):
    def __init__(self, config: CiotSerialCfg):
        super().__init__(config)
        self.ser = serial.Serial(config.port, config.baudrate)
        self.ser.timeout = 10000

    def name(self):
        return self.config.port
    
    def send_msg(self, msg: Msg):
        super().send_msg(msg)
        data = msg.SerializeToString()
        size = struct.pack('<H', len(data))
        self.ser.write('{'.encode())
        self.ser.write(size)
        self.ser.write(data)
        self.ser.write('}'.encode())
        if(self.ser.read() == b'{'):
            data = self.ser.read(2)
            size = struct.unpack('<H', data)[0]
            data = self.ser.read(size)
            msg.ParseFromString(data)
            return msg
        
    def read(self):
        try:
            msg = Msg()
            if(self.ser.read() == b'{'):
                data = self.ser.read(2)
                size = struct.unpack('<H', data)[0]
                data = self.ser.read(size)
                if(self.ser.read() == b'}'):
                    msg.ParseFromString(data)
                    return msg
                else:
                    print('ERROR: Terminator is missing')
                    return None
        except KeyboardInterrupt:
            self.ser.close()
            exit(0)
