# ciot_c Release

### Changes

### WiFi Scanning & AP Management Enhancements

* Added a unified API for WiFi scanning, including `ciot_wifi_scan`, `ciot_wifi_get_scanned_ap`, and `ciot_wifi_get_scanned_aps` functions, as well as the `ciot_wifi_ap_list_t` structure for managing scanned APs in `include/ciot_wifi.h`, with implementations for both ESP32 and ESP8266. [[1]](diffhunk://#diff-ca06c9e501fbf825c6fd9ca3cd0cce0ce9db4ddb5c48c00c94d2c023ccb800d0R31-R44) [[2]](diffhunk://#diff-ca06c9e501fbf825c6fd9ca3cd0cce0ce9db4ddb5c48c00c94d2c023ccb800d0R57-R59) [[3]](diffhunk://#diff-1e5fcda8059ac0f18c5557188647b0a89a590410665a42881e045675369a6279R152-R192) [[4]](diffhunk://#diff-3178431e9d6c268319f8e759b5ad1f5805a9c1a44db140add61f0b66bcb3580dR152-R192)
* Implemented logic to update and store the list of scanned APs after a scan completes, including memory management and populating AP info fields, in both ESP32 and ESP8266 WiFi modules. [[1]](diffhunk://#diff-1e5fcda8059ac0f18c5557188647b0a89a590410665a42881e045675369a6279R292-R316) [[2]](diffhunk://#diff-3178431e9d6c268319f8e759b5ad1f5805a9c1a44db140add61f0b66bcb3580dR292-R316)
* Modified event handlers to process scan completion events, update scan state, send scan result events, and refresh the AP list. [[1]](diffhunk://#diff-1e5fcda8059ac0f18c5557188647b0a89a590410665a42881e045675369a6279L308-R389) [[2]](diffhunk://#diff-3178431e9d6c268319f8e759b5ad1f5805a9c1a44db140add61f0b66bcb3580dL313-R388)

### API & Request Handling

* Updated the WiFi request handler to support new scan and AP info request types, enabling clients to trigger scans and retrieve AP details via the request interface.

### Build System & Configuration Updates

* Updated build system paths for ESP8266 to reference the new `ciot_c` directory structure, ensuring correct inclusion and source directories.
* Added `iphlpapi` to Windows target link libraries in `examples/device/CMakelists.txt`.
* Increased ESP32 flash size configuration from 2MB to 4MB and enabled header flash size update in `sdkconfig.old`.
* Added a new `grpc` target to the `Makefile` for .NET-based gRPC operations.

### Minor Fixes

* Corrected typos (`reconecting` → `reconnecting`) and improved logic for reconnecting and AP disconnect reason handling in ESP8266 WiFi code. [[1]](diffhunk://#diff-3178431e9d6c268319f8e759b5ad1f5805a9c1a44db140add61f0b66bcb3580dL29-R36) [[2]](diffhunk://#diff-3178431e9d6c268319f8e759b5ad1f5805a9c1a44db140add61f0b66bcb3580dL347-R422) [[3]](diffhunk://#diff-3178431e9d6c268319f8e759b5ad1f5805a9c1a44db140add61f0b66bcb3580dL361-R440) [[4]](diffhunk://#diff-3178431e9d6c268319f8e759b5ad1f5805a9c1a44db140add61f0b66bcb3580dL292)
* Updated UART configuration to use GPIO struct for pin assignment in ESP32 and ESP8266 examples. [[1]](diffhunk://#diff-6773dffa63c9d9ef98be84b887af7e3d9e42cbddd75bbdb9d2d997e7d1a072b3L27-R31) [[2]](diffhunk://#diff-df44eea7045a5329763fff4c6777ffff67783c090181511ecbcd327cbb4487cbL26-R27)