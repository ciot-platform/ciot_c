# ciot_c Release

### Changes

**Core event handling and queueing:**

* Replaced the previous `ciot_receiver` structure with a new event queue system (`ciot_event_queue_t` and `ciot_event_slot_t`), enabling buffered event processing and improved concurrency. The event queue now supports configurable size and tracks dropped events. (`[[1]](diffhunk://#diff-e3a87c483eaad2e3f831ea32fa5bb0c2538d10b268422ac8bf3e6da43110f554L73-R97)`, `[[2]](diffhunk://#diff-bebf3b3742ecc13fb8bff23dc362b537e593146621f52ab115c0c492d7459c74R73-R76)`, `[[3]](diffhunk://#diff-ac3505c8e7850d45dfcfdd990afb140a041683f93b640fc2b17a8e05bdd30c10R27-R122)`, `[[4]](diffhunk://#diff-ac3505c8e7850d45dfcfdd990afb140a041683f93b640fc2b17a8e05bdd30c10L263-R383)`, `[[5]](diffhunk://#diff-ac3505c8e7850d45dfcfdd990afb140a041683f93b640fc2b17a8e05bdd30c10L361-L377)`)
* Updated `ciot_busy_task` and `ciot_starting_task` to process events from the queue, improving robustness and handling of unexpected or out-of-order events. (`[[1]](diffhunk://#diff-ac3505c8e7850d45dfcfdd990afb140a041683f93b640fc2b17a8e05bdd30c10L263-R383)`, `[[2]](diffhunk://#diff-ac3505c8e7850d45dfcfdd990afb140a041683f93b640fc2b17a8e05bdd30c10L361-L377)`, `[[3]](diffhunk://#diff-ac3505c8e7850d45dfcfdd990afb140a041683f93b640fc2b17a8e05bdd30c10L397-L410)`, `[[4]](diffhunk://#diff-ac3505c8e7850d45dfcfdd990afb140a041683f93b640fc2b17a8e05bdd30c10L462-R572)`, `[[5]](diffhunk://#diff-ac3505c8e7850d45dfcfdd990afb140a041683f93b640fc2b17a8e05bdd30c10L482)`, `[[6]](diffhunk://#diff-ac3505c8e7850d45dfcfdd990afb140a041683f93b640fc2b17a8e05bdd30c10L493-L504)`)
* Introduced helper functions for queue operations, such as reserving, committing, pushing, and popping events, and handling raw event data deserialization. (`[src/core/ciot.cR27-R122](diffhunk://#diff-ac3505c8e7850d45dfcfdd990afb140a041683f93b640fc2b17a8e05bdd30c10R27-R122)`)

**BLE scanner and interface API updates:**

* Changed BLE scanner event handling to use a specific event type (`ciot_ble_scn_event_adv_report_t`) and updated related function signatures for clarity and correctness. (`[[1]](diffhunk://#diff-9cc3ff9f4c60d80c2855302798490b054d719bbcb6706bb5179002e28506ccbaL76-R76)`, `[[2]](diffhunk://#diff-1d56b9dfd453aa5d1584b74b104467528a195ea593194e1b200838f769b93ea2L139-R142)`, `[[3]](diffhunk://#diff-1d56b9dfd453aa5d1584b74b104467528a195ea593194e1b200838f769b93ea2L162-R161)`, `[[4]](diffhunk://#diff-1d56b9dfd453aa5d1584b74b104467528a195ea593194e1b200838f769b93ea2L195-R194)`)
* Added `ciot_iface_send_event_data` function to the interface API, allowing direct sending of event data buffers. (`[include/ciot_iface.hR53](diffhunk://#diff-d81cd7438a00c22a4f450ac673da1c4398d739083d28a53d8b1a6bfde11f7558R53)`)

**WiFi Multi-Connection Core Improvements**

* Introduced a new `ciot_wifi_multi_wifi_ops_t` structure, allowing injection of custom WiFi operations (start, stop, get_status, get_info) for greater flexibility and easier integration with different WiFi backends. Default implementations are provided and used if no custom ops are set. (`include/ciot_wifi_multi.h`, `src/common/ciot_wifi_multi_base.c`) [[1]](diffhunk://#diff-6cf31a1232ed8b6b5dc2edbe6c329bf3486d16f2ee281fdf267d20c99e339a1dR35-R47) [[2]](diffhunk://#diff-6cf31a1232ed8b6b5dc2edbe6c329bf3486d16f2ee281fdf267d20c99e339a1dR56-R57) [[3]](diffhunk://#diff-7a90ef6b5f3d860783c5c2ab5c25dd1f8738670d999aa3ef433eb0644765fdefR35-L41) [[4]](diffhunk://#diff-7a90ef6b5f3d860783c5c2ab5c25dd1f8738670d999aa3ef433eb0644765fdefL88-R133)
* Added the `ciot_wifi_multi_set_wifi_ops` API and integrated it throughout the codebase, ensuring all WiFi operations use the ops interface for consistency and extensibility. (`include/ciot_wifi_multi.h`, `src/common/ciot_wifi_multi_base.c`) [[1]](diffhunk://#diff-6cf31a1232ed8b6b5dc2edbe6c329bf3486d16f2ee281fdf267d20c99e339a1dR77) [[2]](diffhunk://#diff-7a90ef6b5f3d860783c5c2ab5c25dd1f8738670d999aa3ef433eb0644765fdefL88-R133)
* Improved state management by tracking whether the WiFi multi system has started, preventing invalid operations when not running and ensuring correct event handling. (`src/common/ciot_wifi_multi_base.c`) [[1]](diffhunk://#diff-7a90ef6b5f3d860783c5c2ab5c25dd1f8738670d999aa3ef433eb0644765fdefL151-R237) [[2]](diffhunk://#diff-7a90ef6b5f3d860783c5c2ab5c25dd1f8738670d999aa3ef433eb0644765fdefL170-R296) [[3]](diffhunk://#diff-7a90ef6b5f3d860783c5c2ab5c25dd1f8738670d999aa3ef433eb0644765fdefR360-R364) [[4]](diffhunk://#diff-7a90ef6b5f3d860783c5c2ab5c25dd1f8738670d999aa3ef433eb0644765fdefR404-R407) [[5]](diffhunk://#diff-7a90ef6b5f3d860783c5c2ab5c25dd1f8738670d999aa3ef433eb0644765fdefR420-R424) [[6]](diffhunk://#diff-7a90ef6b5f3d860783c5c2ab5c25dd1f8738670d999aa3ef433eb0644765fdefR579-R587)
* Enhanced the switching logic to avoid unnecessary reconnections if already connected to the target SSID, and to retry all valid SSIDs in a round-robin fashion. (`src/common/ciot_wifi_multi_base.c`) (F2c20b99L634R684, [[1]](diffhunk://#diff-7a90ef6b5f3d860783c5c2ab5c25dd1f8738670d999aa3ef433eb0644765fdefR687-R695) [[2]](diffhunk://#diff-7a90ef6b5f3d860783c5c2ab5c25dd1f8738670d999aa3ef433eb0644765fdefL521-R723)

**Configurability and API Additions**

* Added `ciot_wifi_multi_set_item` API and integrated it into the request handling, allowing dynamic updates to individual WiFi configuration items. (`include/ciot_wifi_multi.h`, `src/common/ciot_wifi_multi_base.c`) [[1]](diffhunk://#diff-6cf31a1232ed8b6b5dc2edbe6c329bf3486d16f2ee281fdf267d20c99e339a1dR68) [[2]](diffhunk://#diff-7a90ef6b5f3d860783c5c2ab5c25dd1f8738670d999aa3ef433eb0644765fdefR306-R331) F2c20b99L470R470)

**Build and Versioning**

* Increased the default value of `CIOT_CONFIG_HTTP_SERVER_TIMEOUT_MS` from 15000 ms to 30000 ms in `ciot_http_server.c` to allow more time for HTTP server responses. New extended timeout is needed to support wifi reconnection attempts.

**Build Script Update:**

* Updated the `make/scripts.mk` script to use new environment variable names and the correct path for the MQTT translator script.

**Test coverage improvements**

* Added new test data and a test case (`test_ciot_crypt_dec_ok_2`) to `ciot_crypt_test.c` for verifying decryption with a different key and input, increasing cryptographic test coverage. [[1]](diffhunk://#diff-4715a2e7f2ac8777d2b274311ac2cfb1e4459dc0144dba852cfb67e75ca3cfa6R26-R31) [[2]](diffhunk://#diff-4715a2e7f2ac8777d2b274311ac2cfb1e4459dc0144dba852cfb67e75ca3cfa6R154-R166) [[3]](diffhunk://#diff-4715a2e7f2ac8777d2b274311ac2cfb1e4459dc0144dba852cfb67e75ca3cfa6R201)

**Bug fixes and functional improvements**

* Moved the `uart_set_pin` call in `ciot_uart_start` so that UART pins are only set after checking if the UART is already started, preventing unnecessary hardware reconfiguration.
* In `ciot_dfu_nrf_event_handler`, changed the event type check from `CIOT_EVENT_TYPE_MSG` to `CIOT_EVENT_TYPE_DATA` to correctly process incoming data events.
* Improved the interface selection logic in `ciot_tcp_get_addr` (Windows): now only iterates network adapters if the TCP type is Ethernet or WiFi, and breaks out of the loop as soon as a connection is established, optimizing connection setup. [[1]](diffhunk://#diff-fd3885fdf60ec1ccd9aef56ede22dc496c00a524e4c0131e794d7aca2a5f1e8aR97-R102) [[2]](diffhunk://#diff-fd3885fdf60ec1ccd9aef56ede22dc496c00a524e4c0131e794d7aca2a5f1e8aR116-R127)

**WiFi reconnection and state management improvements:**

* Added a `reconnect` flag to the `ciot_wifi_base_t` struct and implemented the `ciot_wifi_set_reconnect` function, allowing external configuration of automatic WiFi reconnection attempts (`include/ciot_wifi.h`, `src/common/ciot_wifi_base.c`). [[1]](diffhunk://#diff-ca06c9e501fbf825c6fd9ca3cd0cce0ce9db4ddb5c48c00c94d2c023ccb800d0R45) [[2]](diffhunk://#diff-ca06c9e501fbf825c6fd9ca3cd0cce0ce9db4ddb5c48c00c94d2c023ccb800d0R63) [[3]](diffhunk://#diff-eef672241b635d4c1209cc3ad8db038d86150871db5b6aaf036ec69d255125d5R206-R213)
* Refactored the `ciot_wifi` struct and event handler logic by removing the `reconnecting` and `switching_network` state variables, consolidating connection attempt tracking, and simplifying reconnection flow (`src/esp32/ciot_wifi.c`). [[1]](diffhunk://#diff-1e5fcda8059ac0f18c5557188647b0a89a590410665a42881e045675369a6279L31-R31) [[2]](diffhunk://#diff-1e5fcda8059ac0f18c5557188647b0a89a590410665a42881e045675369a6279L225-L246) [[3]](diffhunk://#diff-1e5fcda8059ac0f18c5557188647b0a89a590410665a42881e045675369a6279L397-L402) [[4]](diffhunk://#diff-1e5fcda8059ac0f18c5557188647b0a89a590410665a42881e045675369a6279L429-L430) [[5]](diffhunk://#diff-1e5fcda8059ac0f18c5557188647b0a89a590410665a42881e045675369a6279L445-R447)

**Connection retry and event handling:**

* Updated the connection retry logic to utilize the new `reconnect` flag, allowing continued reconnection attempts if enabled, and improved event signaling when stopping WiFi (`src/esp32/ciot_wifi.c`). [[1]](diffhunk://#diff-1e5fcda8059ac0f18c5557188647b0a89a590410665a42881e045675369a6279R127-R132) [[2]](diffhunk://#diff-1e5fcda8059ac0f18c5557188647b0a89a590410665a42881e045675369a6279L445-R447)

**Versioning:**

* Bumped the `CIOT_VER` macro to `0,21,0,5` to reflect these changes (`include/ciot.h`).
