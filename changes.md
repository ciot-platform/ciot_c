# ciot_c Release

### Changes

**UART configuration enhancements:**

* Added `ciot_uart_data_bits_t` and `ciot_uart_stop_bits_t` enums, and included `data_bits` and `stop_bits` fields in `ciot_uart_cfg_t` to allow configurable UART frame format. Updated protocol buffer tags, macros, and default initializers accordingly. [[1]](diffhunk://#diff-75f714e563b81ee4365627f7f13e6da4ff540570a419f65b14ae753f8db924e1R41-R55) [[2]](diffhunk://#diff-75f714e563b81ee4365627f7f13e6da4ff540570a419f65b14ae753f8db924e1R81-R82) [[3]](diffhunk://#diff-75f714e563b81ee4365627f7f13e6da4ff540570a419f65b14ae753f8db924e1R145-R163) [[4]](diffhunk://#diff-75f714e563b81ee4365627f7f13e6da4ff540570a419f65b14ae753f8db924e1L140-R180) [[5]](diffhunk://#diff-75f714e563b81ee4365627f7f13e6da4ff540570a419f65b14ae753f8db924e1R199-R200) [[6]](diffhunk://#diff-75f714e563b81ee4365627f7f13e6da4ff540570a419f65b14ae753f8db924e1L196-R234)

* Updated maximum encoded size constants for UART, Modbus client, and Modbus server protocol messages to account for the new configuration fields. [[1]](diffhunk://#diff-75f714e563b81ee4365627f7f13e6da4ff540570a419f65b14ae753f8db924e1L241-R279) [[2]](diffhunk://#diff-6bf44a776dd28cd8ec06193fb80a558c71c3f9214b21e0596abb4a124d546fd1L213-R213) [[3]](diffhunk://#diff-2879311ce531e391c9c16a7e6a20f966629ea88f91d30ecbf7f0844f3ed63f0aL215-R218)

**Platform-specific UART implementation updates:**

* On ESP32 (`src/esp32/ciot_uart.c`): Added validation for baud rate, data bits, and stop bits in `ciot_uart_start`. Introduced helper functions to map protocol enums to ESP-IDF UART driver values, and updated UART configuration accordingly. [[1]](diffhunk://#diff-c8a5a36ea012ff34598ad0ee6f74aaf1c434a0083acbc5a0c877c116db5bcceaR63-R64) [[2]](diffhunk://#diff-c8a5a36ea012ff34598ad0ee6f74aaf1c434a0083acbc5a0c877c116db5bcceaR89-R101) [[3]](diffhunk://#diff-c8a5a36ea012ff34598ad0ee6f74aaf1c434a0083acbc5a0c877c116db5bcceaL103-R119) [[4]](diffhunk://#diff-c8a5a36ea012ff34598ad0ee6f74aaf1c434a0083acbc5a0c877c116db5bcceaR248-R288)

* On Windows (`src/win/ciot_uart.c`): Added functions to convert protocol enums to Windows API values for byte size and stop bits. Updated UART initialization to use these values and handle invalid arguments. [[1]](diffhunk://#diff-63972ff7c01e2e6d441e83c0332b572d8442cca850beecff8f5a82296a80448fR37-R38) [[2]](diffhunk://#diff-63972ff7c01e2e6d441e83c0332b572d8442cca850beecff8f5a82296a80448fR60-R64) [[3]](diffhunk://#diff-63972ff7c01e2e6d441e83c0332b572d8442cca850beecff8f5a82296a80448fL84-R92) [[4]](diffhunk://#diff-63972ff7c01e2e6d441e83c0332b572d8442cca850beecff8f5a82296a80448fR253-R293)

### Modbus TCP over Raw Sockets

* Added a new generic TCP socket interface (`ciot_socket`) to provide raw TCP byte-stream transport for Modbus TCP communication, with APIs for client/server roles, status, and data transfer (`include/ciot_socket.h`, `src/common/ciot_mbus_client.c`). [[1]](diffhunk://#diff-6108660ddaac73c6497a3fca463709f73f84db9439a0b71fa64065710344ffb7R1-R71) [[2]](diffhunk://#diff-3902ca8296f5f13e0b494219ee7accbede4d5edf43a7b2e8c2ff31b2764dedc3R17) [[3]](diffhunk://#diff-3902ca8296f5f13e0b494219ee7accbede4d5edf43a7b2e8c2ff31b2764dedc3R27-R31)
* Refactored Modbus client and server examples to support both RTU (serial) and TCP (socket) transports, including new configuration and initialization logic for TCP-based communication (`examples/mbus_client/main/main.c`, `examples/mbus_server/main/main.c`). [[1]](diffhunk://#diff-3e64b6f6c4c7d4d417e0b83dd9dd23c33146f7a585ffb978a6b2e60ebe434233R55-R95) [[2]](diffhunk://#diff-3e64b6f6c4c7d4d417e0b83dd9dd23c33146f7a585ffb978a6b2e60ebe434233R118-R135) [[3]](diffhunk://#diff-3e64b6f6c4c7d4d417e0b83dd9dd23c33146f7a585ffb978a6b2e60ebe434233R161-R185) [[4]](diffhunk://#diff-717c8555120faf298ae429af6061d31a4313a0a8784cbee63a548ed318a32bedR24-R37) [[5]](diffhunk://#diff-717c8555120faf298ae429af6061d31a4313a0a8784cbee63a548ed318a32bedR60-R77) [[6]](diffhunk://#diff-717c8555120faf298ae429af6061d31a4313a0a8784cbee63a548ed318a32bedR93-R95)

### Device Interface and Configuration Updates

* Extended device interface enums and structures to include new interface types for WiFi STA, Modbus TCP socket, and Modbus TCP client/server, and updated configuration management to support these (`examples/mbus_client/main/main.h`, `examples/mbus_server/main/main.h`). [[1]](diffhunk://#diff-80255e78f181c18eb0a9f6c948ba6842cef695922ce783f6da9f35f81b262cc3R36-R38) [[2]](diffhunk://#diff-80255e78f181c18eb0a9f6c948ba6842cef695922ce783f6da9f35f81b262cc3R54-R56) [[3]](diffhunk://#diff-80255e78f181c18eb0a9f6c948ba6842cef695922ce783f6da9f35f81b262cc3R71) [[4]](diffhunk://#diff-e01decc571909ceb22eb5964a250ce38a9dd4ead856172ba0829fc6e642c803bR40-R42) [[5]](diffhunk://#diff-e01decc571909ceb22eb5964a250ce38a9dd4ead856172ba0829fc6e642c803bR54-R61)
* Added WiFi STA configuration for ESP32 to enable network connectivity for Modbus TCP operation (`examples/mbus_client/main/main.c`, `examples/mbus_server/main/main.c`, `examples/mbus_server/main/target.h`, `examples/mbus_server/main/target_esp32.h`). [[1]](diffhunk://#diff-3e64b6f6c4c7d4d417e0b83dd9dd23c33146f7a585ffb978a6b2e60ebe434233R55-R95) [[2]](diffhunk://#diff-717c8555120faf298ae429af6061d31a4313a0a8784cbee63a548ed318a32bedR24-R37) [[3]](diffhunk://#diff-3c6aa96a6b44e3664f82a95b821bb2cf8b31c933e7a07b8a20b79e647b3b11b6R62-R98) [[4]](diffhunk://#diff-33481545503a2907ce2d465579555748e86444f4c5f0f2378022e2bd5e3bc9d4R42-R49)

### Modbus Server API Enhancements

* Updated the Modbus server API and base structure to support both RTU and TCP transports, including a new method to set the TCP connection and fields to track active and available transports (`include/ciot_mbus_server.h`).

### Platform and Build System Updates

* Updated target configuration headers for ESP32 and Win32 to define TCP port and WiFi credentials for Modbus TCP operation (`examples/mbus_server/main/target_esp32.h`, `examples/mbus_server/main/target_win32.h`). [[1]](diffhunk://#diff-33481545503a2907ce2d465579555748e86444f4c5f0f2378022e2bd5e3bc9d4R42-R49) [[2]](diffhunk://#diff-78816beef3a985853692f5d9beafe713a585459ede5cc8225ea39c394f31c013R40)
* Enabled the socket feature in the global configuration (`include/ciot_config.h`).
* Bumped the CIOT version to 0.26.0.0 to reflect the new features (`include/ciot.h`).

**New upload API support:**

* Introduced `ciot_http_server_upload_api_t` struct and associated handler type to define upload endpoints, including URI, method, max payload size, and callback handler. This struct is added to the main HTTP server base struct (`ciot_http_server_base_t`).
* Added `ciot_http_server_set_upload_api()` function to allow registration of upload endpoints at runtime. [[1]](diffhunk://#diff-30be0adb0cad3dca9597199fc8e81a0da02c60e787a149fc2f378b9ea209fc1cR96) [[2]](diffhunk://#diff-c177ce0c72f538dbaee642d82ed4a16a68060a6fc05da2b6932c167920cea1f7R59-R67)
* Ensured the upload API is initialized as disabled by default in the HTTP server init routine.

**ESP32 backend implementation:**

* Registered the upload route during server startup if enabled, using the provided URI and method, and added a handler to receive, validate size, and process the upload payload. [[1]](diffhunk://#diff-fa8c3f4021f791ed328fcccd28ee915b3f4373dabba5ef1756aca492c007b692R139-R155) [[2]](diffhunk://#diff-fa8c3f4021f791ed328fcccd28ee915b3f4373dabba5ef1756aca492c007b692R47-R48)
* Implemented `ciot_http_server_upload_handler` to receive the entire request body, enforce the size limit, and invoke the user-provided handler, returning appropriate HTTP status codes.
* Increased the maximum number of URI handlers to accommodate the new upload endpoint.

**Mongoose backend implementation:**

* Added logic to the event handler to process upload endpoints, enforcing the maximum size and invoking the registered handler, returning HTTP 200 or 400 as appropriate.

**Ethernet interface improvements:**

* Added a `started` boolean flag to the `ciot_eth` struct to explicitly track the Ethernet interface's runtime state, improving clarity and reliability in start/stop logic.
* Refactored the `ciot_eth_start` and `ciot_eth_stop` functions to use the new `started` flag, ensuring that the driver is only started or stopped when appropriate, and that hardware initialization is always checked and handled robustly.
* Enhanced the hardware initialization (`ciot_eth_hw_init`) to check for previous initialization, handle errors during driver installation, and clean up resources on failure, preventing resource leaks and improving error reporting.

**TCP state synchronization:**

* Updated the Ethernet event handler to synchronize the TCP state (`tcp->status->state`) with Ethernet events, ensuring the reported state accurately reflects the interface's status.