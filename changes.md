# ciot_c Release

### Changes

### BLE Scanner API Improvements

* Refactored BLE scanner event handling by introducing a new `ciot_ble_scn_event_adv_report_t` struct and updating the filter function signature to use this type, making event handling more explicit and extensible. Also added a utility function `ciot_ble_scn_copy_mac` for MAC address manipulation. [[1]](diffhunk://#diff-9cc3ff9f4c60d80c2855302798490b054d719bbcb6706bb5179002e28506ccbaL23-R39) [[2]](diffhunk://#diff-9cc3ff9f4c60d80c2855302798490b054d719bbcb6706bb5179002e28506ccbaR79) [[3]](diffhunk://#diff-1d56b9dfd453aa5d1584b74b104467528a195ea593194e1b200838f769b93ea2R215-R222)
* Added a new `ciot_ble_scn_continue` function to allow continuation of BLE scanning, providing finer control over scanner operation.

### MQTT Client Enhancements

* Introduced new publish APIs (`ciot_mqtt_client_publish` and `ciot_mqtt_client_subtopic_publish`) with support for the retain flag, and deprecated the old publish functions for clarity. Also added a function to set the MQTT last will and a base start function to handle configuration inheritance. [[1]](diffhunk://#diff-a638c8162c9fd3c46a613972d55fb3e86f31fb500834c9975f267f2e88874652R55-R74) [[2]](diffhunk://#diff-3c29355ff0857480c7a57573c88ad3ec552a012ff62443b7bcc7fd8537b3efb9R124-R151) [[3]](diffhunk://#diff-3c29355ff0857480c7a57573c88ad3ec552a012ff62443b7bcc7fd8537b3efb9R232-R241) [[4]](diffhunk://#diff-3c9b48cb353d0c1c8a55926bdd4c32238bebed24f11fced21a6168dc52a16d7cR64-R70) [[5]](diffhunk://#diff-f8eaf168fd7e70a563ff15acc977930908baa391c7a476f3d605cf857af68974L48-R48)
* Improved topic buffer management by defining `CIOT_MQTT_CLIENT_TOPIC_SIZE` based on the topics config struct, ensuring buffer sizes remain consistent with configuration. [[1]](diffhunk://#diff-a638c8162c9fd3c46a613972d55fb3e86f31fb500834c9975f267f2e88874652L25-R29) [[2]](diffhunk://#diff-a638c8162c9fd3c46a613972d55fb3e86f31fb500834c9975f267f2e88874652L41-R40)

### System Interface Updates

* Added a function to retrieve system lifetime (`ciot_sys_get_lifetime`), allowing clients to query uptime or similar metrics. [[1]](diffhunk://#diff-cede3fd5e0673989c303211b2c499187cf13932ea5c130e71efd982aae0406faR48) [[2]](diffhunk://#diff-d21b94ecb4a1873e3ebabb7dbd36ee4bf59a6fc588e621c3665fd6a373245dc8R203-R212)

### Core and Interface Refactoring

* Added a new `ciot_iface_send_internal_event` function for sending internal events, improving event handling flexibility. Also made the event variable static for proper reuse. [[1]](diffhunk://#diff-d81cd7438a00c22a4f450ac673da1c4398d739083d28a53d8b1a6bfde11f7558R53) [[2]](diffhunk://#diff-defced0a0a7679a338faf83db3f262138a323d33221f3139c5f179e4da9fc330R94-L96) [[3]](diffhunk://#diff-defced0a0a7679a338faf83db3f262138a323d33221f3139c5f179e4da9fc330R110-R118)

### Build and Versioning

* Updated the project version to 0.19.0.0 and improved the startup log message to include the version. Also refactored the SDK path handling and include paths in `.vscode/c_cpp_properties.json` for better portability and maintainability. [[1]](diffhunk://#diff-e3a87c483eaad2e3f831ea32fa5bb0c2538d10b268422ac8bf3e6da43110f554L36-R36) [[2]](diffhunk://#diff-ac3505c8e7850d45dfcfdd990afb140a041683f93b640fc2b17a8e05bdd30c10L341-R341) [[3]](diffhunk://#diff-8f4e8cf66ff6479868eeeb084ef19fbb8bea6102cf86f8adec2180dcf38dc47dL5-R8) [[4]](diffhunk://#diff-8f4e8cf66ff6479868eeeb084ef19fbb8bea6102cf86f8adec2180dcf38dc47dL102-R122)

Configuration improvements:

* Added `CIOT_CONFIG_HTTP_SERVER_TIMEOUT_MS` and `CIOT_CONFIG_HTTP_SERVER_MAX_RESP_SIZE` macros to allow customization of HTTP server timeout and maximum response size, replacing hardcoded values in `src/esp32/ciot_http_server.c`.
* Updated usage of timeout macros in `ciot_http_server_api_handler` and `ciot_http_server_custom_api_handler` to use the new configurable macro. [[1]](diffhunk://#diff-fa8c3f4021f791ed328fcccd28ee915b3f4373dabba5ef1756aca492c007b692L165-R177) [[2]](diffhunk://#diff-fa8c3f4021f791ed328fcccd28ee915b3f4373dabba5ef1756aca492c007b692L339-R351)

### Buffer safety:

* Updated the response buffer in the `ciot_http_server` struct to use the configurable maximum response size, and added logic in `ciot_http_server_send_bytes` to truncate responses that exceed the buffer size, logging a warning when truncation occurs. [[1]](diffhunk://#diff-fa8c3f4021f791ed328fcccd28ee915b3f4373dabba5ef1756aca492c007b692L20-R35) [[2]](diffhunk://#diff-fa8c3f4021f791ed328fcccd28ee915b3f4373dabba5ef1756aca492c007b692R93-R97)

### Version update:

* Bumped the version macro `CIOT_VER` in `include/ciot.h` to `0,19,0,1` to reflect these changes.