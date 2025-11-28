# ciot_c Release

### Changes

### MQTT Session Configuration Support

* Added a new `ciot_mqtt_client_session_cfg_t` struct to represent session parameters (`clean_session` and `keep_alive`) in `mqtt_client.pb.h`, along with related protocol buffer bindings and macros. Session configuration is now included in `ciot_mqtt_client_cfg_t` and integrated into protocol buffer field lists, tags, and default/zero initializers. [[1]](diffhunk://#diff-16c54cb01e6ebd89e2429fd7641138fae68d9ad6f60175561c0c1af809df4436R68-R72) [[2]](diffhunk://#diff-16c54cb01e6ebd89e2429fd7641138fae68d9ad6f60175561c0c1af809df4436R85-R86) [[3]](diffhunk://#diff-16c54cb01e6ebd89e2429fd7641138fae68d9ad6f60175561c0c1af809df4436L174-R183) [[4]](diffhunk://#diff-16c54cb01e6ebd89e2429fd7641138fae68d9ad6f60175561c0c1af809df4436L184-R194) [[5]](diffhunk://#diff-16c54cb01e6ebd89e2429fd7641138fae68d9ad6f60175561c0c1af809df4436R214-R215) [[6]](diffhunk://#diff-16c54cb01e6ebd89e2429fd7641138fae68d9ad6f60175561c0c1af809df4436R224) [[7]](diffhunk://#diff-16c54cb01e6ebd89e2429fd7641138fae68d9ad6f60175561c0c1af809df4436R272-R277) [[8]](diffhunk://#diff-16c54cb01e6ebd89e2429fd7641138fae68d9ad6f60175561c0c1af809df4436L267-R292) [[9]](diffhunk://#diff-16c54cb01e6ebd89e2429fd7641138fae68d9ad6f60175561c0c1af809df4436R341) [[10]](diffhunk://#diff-16c54cb01e6ebd89e2429fd7641138fae68d9ad6f60175561c0c1af809df4436R354) [[11]](diffhunk://#diff-16c54cb01e6ebd89e2429fd7641138fae68d9ad6f60175561c0c1af809df4436L341-R371) [[12]](diffhunk://#diff-ebc5b4503bbc84f1d7b3134b255d936f9b43209eceb18dd789c19680853be48dR21-R23)

* MQTT client start logic on ESP32, ESP8266, and MG platforms now checks for missing session and last will configuration, inheriting values from the base configuration if available. The session parameters are used to set client options such as keep alive, clean session, and last will properties during client initialization. [[1]](diffhunk://#diff-f8eaf168fd7e70a563ff15acc977930908baa391c7a476f3d605cf857af68974L47-R60) [[2]](diffhunk://#diff-f8eaf168fd7e70a563ff15acc977930908baa391c7a476f3d605cf857af68974R73-R79) [[3]](diffhunk://#diff-b22fe51bd53f8103fe2b667c21cdf608a57f4dd05d97e316f7097f0c507c88e4L47-R60) [[4]](diffhunk://#diff-b22fe51bd53f8103fe2b667c21cdf608a57f4dd05d97e316f7097f0c507c88e4R73-R78) [[5]](diffhunk://#diff-59b877ffdc42d6bb6f888a4453d6945185e29ba43d05b8aa92a94ead8634d21eR51-R62) [[6]](diffhunk://#diff-59b877ffdc42d6bb6f888a4453d6945185e29ba43d05b8aa92a94ead8634d21eR71-R94)

### Protocol Buffer and Message Size Updates

* Increased maximum encoded message sizes for MQTT client configuration and data messages, and added size definitions for the new session configuration struct. Also updated the maximum size for message data in `msg_data.pb.h`. [[1]](diffhunk://#diff-16c54cb01e6ebd89e2429fd7641138fae68d9ad6f60175561c0c1af809df4436L341-R371) [[2]](diffhunk://#diff-6cfe3e7642bf0fe28c190044de12d8a62fd1315a9ce319dab0ab8cafa453050dL152-R152)

### Configuration and Build System Improvements

* Updated include paths in `.vscode/c_cpp_properties.json` to use the `CIOT_SDK_ROOT` variable instead of `CIOT_PATH`, reflecting changes in project structure.
* Removed the `mg` source directory from the `COMPONENT_SRCDIRS` list in `component.mk`, likely because it is no longer needed for the ESP8266 example.

### Miscellaneous Improvements

* Increased the buffer size for configuration file names in `ciot_delete_all` from 16 to 32 characters to prevent potential buffer overflows.
* Added maximum size constraints for MQTT last will topic and payload in `global.options`.