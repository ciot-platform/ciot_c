# ciot_c Release

### Changes

**PlatformIO support**

Great changes on project structure do support PlatformIO projects.

**Project structure and configuration improvements:**

* Refactored source and include directory paths in `common/Common.cmake` to move protocol, nanopb, modbus, and cryptographic sources from `proto` and `libs` subfolders to top-level `src` subfolders, simplifying the build system and project organization. (`common/Common.cmake`)
* Updated platform detection macros in `include/ciot_config.h` to automatically define platform-specific flags for ESP32, ESP8266, Linux, and Windows, improving portability and configuration management. (`include/ciot_config.h`)

**Windows build improvements:**

* Added `iphlpapi` to the list of libraries linked for Windows builds in multiple example CMake files, ensuring proper compilation and runtime support on Windows. (`examples/ciota/CMakelists.txt`, `examples/cli/CMakelists.txt`, `examples/mbus_client/main/CMakeLists.txt`, `examples/mbus_server/main/CMakeLists.txt`, `examples/sparkplugb/CMakelists.txt`) [[1]](diffhunk://#diff-4458cdb0e5751d1b3eeccbc8a8e3e6290cb8599e18ec539f9449564d691525eaL22-R22) [[2]](diffhunk://#diff-6cec75c7deb0227ce976eff10c7c738781314818ebb732f2b1ca44415e344c36L22-R22) [[3]](diffhunk://#diff-00419bd8656d68fb69f978b53d1aefc92cfff0c1593e1c4ee4c0c6578b39a47bL39-R39) [[4]](diffhunk://#diff-077542f7361e5283e88762fac4ae213a8d62cca3b7853bb5fcd513b8170e13e3L38-R38) [[5]](diffhunk://#diff-33a86eb082877b72e29179975da4fa91223d7a430dc91e8c8ecf9ff0eb9a860fL23)
* Updated the cryptography include path for Windows builds from `libs/crypt` to `src/crypt` in `common/Common.cmake`, aligning with the new project structure. (`common/Common.cmake`)

**Project metadata and versioning:**

* Incremented project version to `0.11.0` in both `include/ciot.h` and `library.json`, reflecting the new release and changes. (`include/ciot.h`, `library.json`) [[1]](diffhunk://#diff-e3a87c483eaad2e3f831ea32fa5bb0c2538d10b268422ac8bf3e6da43110f554L36-R36) [[2]](diffhunk://#diff-64cc7cdde6789dc1c1c4a6f406cd1ff6a6a00027097788e2d52fc27b0d75502eL2-R3)
* Updated `library.json` for PlatformIO with the new project name (`ciot_c`), version, and an export section to exclude `examples` and `tests` from published packages. (`library.json`)

**Miscellaneous fixes:**

* Fixed an include path in `include/ciot_mbus.h` to use `nanomodbus/nanomodbus.h` for consistency with the new source layout. (`include/ciot_mbus.h`)
* Removed unused parsing for the `--encrypted` option in the OTA CLI parser. (`examples/cli/parsers/ota_parser.c`)