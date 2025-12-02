from ciot_connection import CiotHttpClient
from ciot.proto.v1.msg_pb2 import MsgType, IfaceType, Msg
import sys

cfg = 'http://127.0.0.1:8080/v1/ciot'
conn = CiotHttpClient(cfg)

def check():
    msg = Msg()
    msg.id = 112
    msg.type = MsgType.MSG_TYPE_GET_INFO
    msg.iface.type = IfaceType.IFACE_TYPE_SYS
    msg.iface.id = 1
    try:
        rsp = conn.send_msg(msg)
        if(rsp.id == msg.id and rsp.type == msg.type):
            print('Device ´' + msg.data.sys.info.hw_name + '` detected on ' + conn.name())
    except:
        print('Device not detected')

def main():
    if(sys.argv[1] == 'check'):
        check()

if __name__ == "__main__":
    if(len(sys.argv) == 1):
        sys.argv.append('check')
    main()        