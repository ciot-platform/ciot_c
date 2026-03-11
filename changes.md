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

### TCP configuration fixes:

* In `ciot_tcp_set_ip_cfg` for ESP32 (`ciot_tcp.c`), corrected the assignment so that the DNS address (`dns`) is used instead of the netmask (`mask`) when configuring `dns_info`.
* In `ciot_tcp_set_ip_cfg` for ESP8266 (`ciot_tcp.c`), fixed the DNS configuration to use the DNS address (`dns`) rather than the netmask (`mask`).

### MQTT client bugfix:

* Updated `ciot_mqtt_client_subtopic_publish` in `ciot_mqtt_client_base.c` to set `base->status.last_msg_time` using `ciot_timer_now()`, ensuring the timestamp is refreshed on each publish.

### WiFi reconnection and event management:

* Added `skip_disconnect_event` flag to the `ciot_wifi` struct to control whether disconnect events are sent during reconnections, preventing redundant events when switching APs (`src/esp32/ciot_wifi.c`).
* Updated the WiFi station start and stop logic to set TCP state to `DISCONNECTING` and manage the `skip_disconnect_event` flag, ensuring proper event flow and state consistency (`src/esp32/ciot_wifi.c`). [[1]](diffhunk://#diff-1e5fcda8059ac0f18c5557188647b0a89a590410665a42881e045675369a6279R121-R122) [[2]](diffhunk://#diff-1e5fcda8059ac0f18c5557188647b0a89a590410665a42881e045675369a6279R132) [[3]](diffhunk://#diff-1e5fcda8059ac0f18c5557188647b0a89a590410665a42881e045675369a6279R226-R227)
* Modified the WiFi station event handler to conditionally send the `STOPPED` event based on the `skip_disconnect_event` flag, resetting the flag after use (`src/esp32/ciot_wifi.c`).

* Added logic for tracking connection attempts and switching networks in `ciot_wifi.c` for both ESP32 and ESP8266, including new fields (`connect_attempts`, `switching_network`) and a retry limit (`CIOT_WIFI_CONNECTION_ATTEMPTS`). This prevents infinite reconnection loops and properly handles network switching scenarios. [[1]](diffhunk://#diff-1e5fcda8059ac0f18c5557188647b0a89a590410665a42881e045675369a6279R26-R33) [[2]](diffhunk://#diff-1e5fcda8059ac0f18c5557188647b0a89a590410665a42881e045675369a6279R222-R229) [[3]](diffhunk://#diff-1e5fcda8059ac0f18c5557188647b0a89a590410665a42881e045675369a6279R241-R243) [[4]](diffhunk://#diff-1e5fcda8059ac0f18c5557188647b0a89a590410665a42881e045675369a6279R397-R402) [[5]](diffhunk://#diff-1e5fcda8059ac0f18c5557188647b0a89a590410665a42881e045675369a6279R430) [[6]](diffhunk://#diff-1e5fcda8059ac0f18c5557188647b0a89a590410665a42881e045675369a6279L428-R472) [[7]](diffhunk://#diff-3178431e9d6c268319f8e759b5ad1f5805a9c1a44db140add61f0b66bcb3580dR26-R33) [[8]](diffhunk://#diff-3178431e9d6c268319f8e759b5ad1f5805a9c1a44db140add61f0b66bcb3580dR222-R229) [[9]](diffhunk://#diff-3178431e9d6c268319f8e759b5ad1f5805a9c1a44db140add61f0b66bcb3580dR241-R243) [[10]](diffhunk://#diff-3178431e9d6c268319f8e759b5ad1f5805a9c1a44db140add61f0b66bcb3580dR410-R415) [[11]](diffhunk://#diff-3178431e9d6c268319f8e759b5ad1f5805a9c1a44db140add61f0b66bcb3580dR438-R444) [[12]](diffhunk://#diff-3178431e9d6c268319f8e759b5ad1f5805a9c1a44db140add61f0b66bcb3580dL422-R485)
* Improved AP scanning memory management in ESP8266 Wi-Fi by allocating and freeing memory for AP records, ensuring accurate results and preventing memory leaks.
* Added a new API function `ciot_wifi_is_connected` to check Wi-Fi connection status, with its implementation for the base Wi-Fi module. [[1]](diffhunk://#diff-ca06c9e501fbf825c6fd9ca3cd0cce0ce9db4ddb5c48c00c94d2c023ccb800d0R62) [[2]](diffhunk://#diff-eef672241b635d4c1209cc3ad8db038d86150871db5b6aaf036ec69d255125d5R199-R205)

### Storage configuration updates:

* Defined `FS_BASE_PATH` and `FS_PARTITION_LABLE` in `ciot_storage_fs.h` and removed their duplicate definition from `ciot_storage_spiffs.c`, ensuring consistent configuration across platforms. [[1]](diffhunk://#diff-3135185a38c22f2caca3534f9b2949c9071934105f8dfe3cdff8661d43d477bcR17-R19) [[2]](diffhunk://#diff-113eac970ce552f3a69530075cca67f6ea170dcd52b35c54163e989008476528L22-L24)
* Fixed an include bug in `ciot_storage_auto.c` by replacing the `.c` file include with `.h`, improving build reliability.

### Version update:

* Changed the `CIOT_VER` macro in `ciot.h` from `0,19,1,0` to `0,19,0,4` to reflect the new version.