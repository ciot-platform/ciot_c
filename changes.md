# ciot_c Release

### Changes

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
