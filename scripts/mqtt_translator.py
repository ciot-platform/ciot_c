import paho.mqtt.client as mqtt
import threading
import argparse
import time
import json

from ciot.proto.v2 import msg_pb2

MAX_FILE_SIZE_BYTES = 1024 * 1024
topic_file_states = {}
topic_file_lock = threading.Lock()

def protobuf_to_jsonable(message):
    result = {}
    for field, value in message.ListFields():
        if field.is_repeated:
            if field.type == field.TYPE_MESSAGE:
                result[field.name] = [protobuf_to_jsonable(item) for item in value]
            elif field.type == field.TYPE_BYTES:
                result[field.name] = [get_bytes_from_pb_field(item) for item in value]
            elif field.type == field.TYPE_ENUM:
                result[field.name] = [field.enum_type.values_by_number[int(item)].name for item in value]
            else:
                result[field.name] = list(value)
            continue

        if field.type == field.TYPE_MESSAGE:
            result[field.name] = protobuf_to_jsonable(value)
        elif field.type == field.TYPE_BYTES:
            result[field.name] = get_bytes_from_pb_field(field, value)
        elif field.type == field.TYPE_ENUM:
            result[field.name] = field.enum_type.values_by_number[int(value)].name
        else:
            result[field.name] = value

    return result

def get_bytes_from_pb_field(field, value):
    if(field.name == "ip" or field.name == "app_ver"):
        return '.'.join(str(b) for b in value)
    return value.hex()

def on_connect(client, userdata, flags, rc):
    client.subscribe(userdata['topic'])
    print(f"Connected to MQTT broker with code {rc}, subscribed to topic {userdata['topic']}")

def on_disconnect(client, userdata, rc):
    print(f"Disconnected from MQTT broker with code {rc}")

def on_message(client, userdata, msg):
    print(f"Received message on topic {msg.topic} with payload size {len(msg.payload)} bytes")
    pbMsg = msg_pb2.Msg()
    pbMsg.ParseFromString(msg.payload)
    json_data = protobuf_to_jsonable(pbMsg)
    topic = msg.topic
    new_topic = 'json/' + topic
    client.publish(new_topic, json.dumps(json_data))

def main():
    print("Starting MQTT translator...")

    parser = argparse.ArgumentParser(description="MQTT Client Script")
    parser.add_argument('--host', required=True, help='MQTT broker host')
    parser.add_argument('--port', type=int, default=1883, help='MQTT broker port')
    parser.add_argument('--username', required=True, help='MQTT username')
    parser.add_argument('--password', required=True, help='MQTT password')
    parser.add_argument('--topic', default='#', help='MQTT topic to subscribe')
    args = parser.parse_args()

    print(f"Connecting to MQTT broker at {args.host}:{args.port} with username '{args.username}' password '{args.password}' and topic '{args.topic}'...")

    client = mqtt.Client(userdata={'topic': args.topic }, client_id=f"deploy-script-{int(time.time())}")
    client.username_pw_set(args.username, args.password)

    client.on_connect = on_connect
    client.on_disconnect = on_disconnect
    client.on_message = on_message

    client.connect(args.host, args.port, 60)

    # Run the MQTT loop in a separate thread
    def mqtt_loop():
        client.loop_forever()

    thread = threading.Thread(target=mqtt_loop)
    thread.start()

if __name__ == "__main__":
    main()