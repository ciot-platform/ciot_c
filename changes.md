# ciot_c Release

### Changes

MQTT client event handling improvements:
* Set the `base->status.state` to `CIOT_MQTT_CLIENT_STATE_DISCONNECTED` when an MQTT disconnection event occurs in both `ciot_mqtt_client.c` for ESP32 and ESP8266. This ensures the client state accurately reflects the disconnected status. [[1]](diffhunk://#diff-f8eaf168fd7e70a563ff15acc977930908baa391c7a476f3d605cf857af68974R144) [[2]](diffhunk://#diff-b22fe51bd53f8103fe2b667c21cdf608a57f4dd05d97e316f7097f0c507c88e4R133)

Version update:
* Updated the `CIOT_VER` macro in `ciot.h` from `0,21,2,0` to `0,21,2,1` to reflect the new version.