# ciot_c Release

### Changes

**HTTP Server Customization:**

* Added support for registering custom API endpoints in the HTTP server, including a new handler type (`ciot_http_server_custom_api_handler_fn`), associated data structures, and the ability to enable/disable and handle custom routes at runtime. This allows users to define and process their own HTTP API endpoints through the CIOT HTTP server. [[1]](diffhunk://#diff-30be0adb0cad3dca9597199fc8e81a0da02c60e787a149fc2f378b9ea209fc1cR24-R25) [[2]](diffhunk://#diff-30be0adb0cad3dca9597199fc8e81a0da02c60e787a149fc2f378b9ea209fc1cR43-R57) [[3]](diffhunk://#diff-30be0adb0cad3dca9597199fc8e81a0da02c60e787a149fc2f378b9ea209fc1cR69) [[4]](diffhunk://#diff-c177ce0c72f538dbaee642d82ed4a16a68060a6fc05da2b6932c167920cea1f7L26-R31) [[5]](diffhunk://#diff-c177ce0c72f538dbaee642d82ed4a16a68060a6fc05da2b6932c167920cea1f7R49-R57) [[6]](diffhunk://#diff-fa8c3f4021f791ed328fcccd28ee915b3f4373dabba5ef1756aca492c007b692L36-R39) [[7]](diffhunk://#diff-fa8c3f4021f791ed328fcccd28ee915b3f4373dabba5ef1756aca492c007b692L59-R60) [[8]](diffhunk://#diff-fa8c3f4021f791ed328fcccd28ee915b3f4373dabba5ef1756aca492c007b692L96-R97) [[9]](diffhunk://#diff-fa8c3f4021f791ed328fcccd28ee915b3f4373dabba5ef1756aca492c007b692R108-R129) [[10]](diffhunk://#diff-fa8c3f4021f791ed328fcccd28ee915b3f4373dabba5ef1756aca492c007b692L121-R140) [[11]](diffhunk://#diff-fa8c3f4021f791ed328fcccd28ee915b3f4373dabba5ef1756aca492c007b692L169-R188) [[12]](diffhunk://#diff-fa8c3f4021f791ed328fcccd28ee915b3f4373dabba5ef1756aca492c007b692R293-R361)

**Network Interface Enhancements:**

* Added new APIs to retrieve the current IP address for both Ethernet (`ciot_eth_get_ip`) and WiFi (`ciot_wifi_get_ip`) interfaces, as well as to query the WiFi signal strength (`ciot_wifi_get_rssi`). These changes are implemented in both the interface headers and their respective source files. [[1]](diffhunk://#diff-fa4523ff179274410ba9796738b1f504e59afa76563a8ee1b0afabb9a6ce640eR37) [[2]](diffhunk://#diff-10c27a04ab9edf24529f28f38b31250b422c7699ec0d9c9a1a1c7fae81747629R140-R149) [[3]](diffhunk://#diff-ca06c9e501fbf825c6fd9ca3cd0cce0ce9db4ddb5c48c00c94d2c023ccb800d0R60-R61) [[4]](diffhunk://#diff-eef672241b635d4c1209cc3ad8db038d86150871db5b6aaf036ec69d255125d5R158-R166) [[5]](diffhunk://#diff-1e5fcda8059ac0f18c5557188647b0a89a590410665a42881e045675369a6279R171-R180)
* The WiFi status retrieval now includes the latest RSSI value, and WiFi connection events update the RSSI and access point information. [[1]](diffhunk://#diff-eef672241b635d4c1209cc3ad8db038d86150871db5b6aaf036ec69d255125d5R135) [[2]](diffhunk://#diff-1e5fcda8059ac0f18c5557188647b0a89a590410665a42881e045675369a6279R408-R413)

**DFU (Device Firmware Update) Improvements:**

* Added a "test mode" to the Nordic DFU implementation, including new struct members, API (`ciot_dfu_nrf_set_test_mode`), and logic to immediately complete ping responses and state transitions when in test mode. [[1]](diffhunk://#diff-74a286d93b9147cda2e796082ebafd561f5ffe864b6e7ef62c25a6043764bb84R102) [[2]](diffhunk://#diff-fbe65a7e710b59bf67db57d1d53ed72fe347e8ea6b9729c6ce64008109b8f793L63-R63) [[3]](diffhunk://#diff-fbe65a7e710b59bf67db57d1d53ed72fe347e8ea6b9729c6ce64008109b8f793R95-R96) [[4]](diffhunk://#diff-fbe65a7e710b59bf67db57d1d53ed72fe347e8ea6b9729c6ce64008109b8f793R134-R140) [[5]](diffhunk://#diff-fbe65a7e710b59bf67db57d1d53ed72fe347e8ea6b9729c6ce64008109b8f793R463-R468)
* Improved logging for DFU events and start operations. [[1]](diffhunk://#diff-fbe65a7e710b59bf67db57d1d53ed72fe347e8ea6b9729c6ce64008109b8f793R95-R96) [[2]](diffhunk://#diff-fbe65a7e710b59bf67db57d1d53ed72fe347e8ea6b9729c6ce64008109b8f793L588-R603)

**Storage auto-detection improvements:**

* Refactored `ciot_storage_auto_new` in `src/esp32/ciot_storage_auto.c` to search for a data partition labeled `"storage"` instead of an app partition, return `NULL` if not found, and update the function signature to accept a `label` parameter. Also fixed the include from `.c` to `.h`.
* Improved error handling and logging in `src/esp8266/ciot_storage_auto.c` by logging errors when no storage partition is found and indicating which storage type is being used.
* Added new header file `include/ciot_storage_auto.h` for the storage auto-detection functionality, declaring the `ciot_storage_auto_new` function.

**WiFi improvements:**

* Updated `ciot_wifi_get_rssi` in `src/esp32/ciot_wifi.c` to only attempt to retrieve RSSI if the TCP state is connected, preventing invalid reads.

**Core CIOT API Additions:**

* Introduced a new function (`ciot_iface_get_state`) to query the state of a specific interface by ID, with corresponding header and implementation updates. [[1]](diffhunk://#diff-e3a87c483eaad2e3f831ea32fa5bb0c2538d10b268422ac8bf3e6da43110f554R111) [[2]](diffhunk://#diff-ac3505c8e7850d45dfcfdd990afb140a041683f93b640fc2b17a8e05bdd30c10R666-R672)

**Miscellaneous:**

* Minor improvements to event handling and logging, such as restoring event types after sending responses.

**NTP First Sync Time Tracking and API:**

* Added `ciot_ntp_first_sync_time()` API to all platforms (ESP32, ESP8266, Linux, Windows) to provide the timestamp of the first successful NTP sync. This replaces previous per-platform mechanisms for tracking system initialization time. (`include/ciot_ntp.h`, `src/esp32/ciot_ntp.c`, `src/esp8266/ciot_ntp.c`, `src/linux/ciot_ntp.c`, `src/win/ciot_ntp.c`) [[1]](diffhunk://#diff-b7d92361932850e67d4b55a7249b2a1b60e327bdf86e6a79b19d98b6fd1008d5L47-R48) [[2]](diffhunk://#diff-b90864bd7b441575f4f4a33e3593ab52c575a94f98a545eef63c8a773520c506R49-R53) [[3]](diffhunk://#diff-f0e18b87ec4938a8558c8ad97950821b858f176e2970e152a000c75cc365a046R98-R102) [[4]](diffhunk://#diff-df1b9dfb52d12a962128af748d2d5da7ad0caad3239d5e56a45053218167102dR102-R106) [[5]](diffhunk://#diff-e0c9f32b0a2806032f895989391bb01b0e200ed3e637544fcdfee67ce3bdb011R51-R55) [[6]](diffhunk://#diff-8a0b3683cceb8a7edfbf8b6fff6eb6ea1d58b683cd520c5998b92e350b9b561eR51-R55)

* Modified NTP synchronization callbacks to set `first_sync_time` only on the first successful sync, ensuring the timestamp is accurate and stable. (`src/esp32/ciot_ntp.c`, `src/esp8266/ciot_ntp.c`) [[1]](diffhunk://#diff-f0e18b87ec4938a8558c8ad97950821b858f176e2970e152a000c75cc365a046R112-R115) [[2]](diffhunk://#diff-df1b9dfb52d12a962128af748d2d5da7ad0caad3239d5e56a45053218167102dR117-R120)

**System Lifetime Calculation Improvements:**

* System lifetime is now calculated based on the time since the first NTP sync, instead of the previous `init_time` field, on all platforms. This ensures a more accurate representation of uptime relative to synchronized time. (`src/esp32/ciot_sys.c`, `src/esp8266/ciot_sys.c`, `src/linux/ciot_sys.c`, `src/win/ciot_sys.c`) [[1]](diffhunk://#diff-89e41b8a6f089a8c5a3cd4d31a6b4b8298d3e8fa721e6dca06bc4ad5f29c6b05L73-R73) [[2]](diffhunk://#diff-e0ea8830e0f110f5ab5be0bbfd4a810f6746a667332d85a5340ac96e210bcadaL73-R73) [[3]](diffhunk://#diff-d089c49e84691467dccbd06b177d3c6119299cee5f10ff1ecb42b366757e6943L65-R64) [[4]](diffhunk://#diff-34fc73335d58756935a3d9f6bff9b322edf0f956c5b105679c84be819182432bL65-R64)

* Removed the `init_time` field from the `ciot_sys` struct, as it is no longer needed. (`src/esp32/ciot_sys.c`, `src/esp8266/ciot_sys.c`, `src/linux/ciot_sys.c`, `src/win/ciot_sys.c`) [[1]](diffhunk://#diff-89e41b8a6f089a8c5a3cd4d31a6b4b8298d3e8fa721e6dca06bc4ad5f29c6b05L29) [[2]](diffhunk://#diff-e0ea8830e0f110f5ab5be0bbfd4a810f6746a667332d85a5340ac96e210bcadaL29) [[3]](diffhunk://#diff-d089c49e84691467dccbd06b177d3c6119299cee5f10ff1ecb42b366757e6943R25-L31) [[4]](diffhunk://#diff-34fc73335d58756935a3d9f6bff9b322edf0f956c5b105679c84be819182432bR24-L31)

**Error Handling Improvements:**

* Improved null pointer checks in `ciot_mqtt_client_get_state()` and `ciot_iface_get_state()`, returning default states instead of relying solely on macros. (`src/common/ciot_mqtt_client_base.c`, `src/core/ciot.c`) [[1]](diffhunk://#diff-3c29355ff0857480c7a57573c88ad3ec552a012ff62443b7bcc7fd8537b3efb9L104-R107) [[2]](diffhunk://#diff-ac3505c8e7850d45dfcfdd990afb140a041683f93b640fc2b17a8e05bdd30c10L669-R672)

**Header and Dependency Updates:**

* Included necessary headers for `ciot_ntp.h` and platform-specific files to support the new API and ensure consistency. (`include/ciot_ntp.h`, `src/esp32/ciot_sys.c`, `src/esp8266/ciot_sys.c`, `src/linux/ciot_ntp.c`, `src/win/ciot_ntp.c`, `src/linux/ciot_sys.c`, `src/win/ciot_sys.c`) [[1]](diffhunk://#diff-b7d92361932850e67d4b55a7249b2a1b60e327bdf86e6a79b19d98b6fd1008d5R21) [[2]](diffhunk://#diff-89e41b8a6f089a8c5a3cd4d31a6b4b8298d3e8fa721e6dca06bc4ad5f29c6b05R21) [[3]](diffhunk://#diff-e0ea8830e0f110f5ab5be0bbfd4a810f6746a667332d85a5340ac96e210bcadaR13) [[4]](diffhunk://#diff-e0c9f32b0a2806032f895989391bb01b0e200ed3e637544fcdfee67ce3bdb011R18) [[5]](diffhunk://#diff-8a0b3683cceb8a7edfbf8b6fff6eb6ea1d58b683cd520c5998b92e350b9b561eR18) [[6]](diffhunk://#diff-d089c49e84691467dccbd06b177d3c6119299cee5f10ff1ecb42b366757e6943R25-L31) [[7]](diffhunk://#diff-34fc73335d58756935a3d9f6bff9b322edf0f956c5b105679c84be819182432bR24-L31)