# ciot_c Release

### Changes

**PlatformIO Example Project:**

* Added a complete PlatformIO example for ESP32, including configuration files (`platformio.ini`, `CMakeLists.txt`, partition table, and VSCode settings), source code (`main.c`, `main.h`), and documentation for headers, libraries, and tests. This provides a ready-to-use template for ESP32 development with CIOT. [[1]](diffhunk://#diff-ac23d73afce33fdffd3955bde561026effe49229d00515a2aec38748f2cf5e39R1-R34) [[2]](diffhunk://#diff-4227f1584cf8af0da0a8099524a7abda4c388be7566da1bd6a0c0c0ac5bc9b9aR1-R3) [[3]](diffhunk://#diff-5af74fdad6fcfcff5860c915c8f79ec78ecee3174c675d12103987c6e564a795R1-R6) [[4]](diffhunk://#diff-436f575e32384e621b1abe4a572c9faea18867f1f48afa4758e4ed334f83f2b9R1-R208) [[5]](diffhunk://#diff-46d1bd8d5e97532d81ecc4f132deaefe1accd29013d810d243e225d658908cb3R1-R83) [[6]](diffhunk://#diff-8e5d92913cd451c249ac01ed03d254c606001d442876aba19284ef8ad1788812R1-R9) [[7]](diffhunk://#diff-5fbfc34c4c2679cb0ea46c1188b26e05860b117d0e043f4badb75b4cd777b508R1-R37) [[8]](diffhunk://#diff-99be0ade476c22f102e7e249b65dc195e5c0080c231eeb071623cd8ece2292b0R1-R46) [[9]](diffhunk://#diff-f8fbb20dda2d6e682b4bed94067ea41d732b2f46500ddfc17cfa81e77f7f8301R1-R11) [[10]](diffhunk://#diff-1394c74609a5a22e3efb7e65f96cc098a23a34b2d727c826ef3076ab190421e9R1-R5) [[11]](diffhunk://#diff-daa08993ffd5f7bfa630c12738d4676bc4e5493f44bce0b35c2cb6fa8ff15152R1-R10) [[12]](diffhunk://#diff-0fa9dbc5a135d1c620264b0b9f8449aed629605472b662e84867bf33fd3acf45R1-R16)

**New Proxy Example:**

* Added a new `proxy` example project with its own CMake configuration and custom CIOT feature configuration header, demonstrating how to build and configure a proxy application using CIOT. [[1]](diffhunk://#diff-c34bbe988fbe83ad25dc3eb04cba40a8cc1d81f069b1773b76de741e5612cb70R14) [[2]](diffhunk://#diff-93a469c005d2ba2d4108fee75b9e8639b3ed4b69f4098fc21c17bc19a2421ef9R1-R24) [[3]](diffhunk://#diff-c908031da5b5241dfc406bbda163bde5a1b12bf780e6198bc4047f3b16857489R1-R53)

**Build System and Directory Structure Updates:**

* Updated all relevant build scripts (`CMakeLists.txt`, `component.mk`, and Makefiles) to use the new `protos` directory structure for protocol buffers and nanopb sources and includes, ensuring builds reference the correct file locations. [[1]](diffhunk://#diff-d17ce4e060bb5a5386cdbdb3d43b27e52cf16debb3a062aa77300f8373f25488L25-R33) [[2]](diffhunk://#diff-d1cf1a7d069010ec62f1934509acd3e120134843ee80eb6faef07bdad2654c06L13-R13) [[3]](diffhunk://#diff-d1cf1a7d069010ec62f1934509acd3e120134843ee80eb6faef07bdad2654c06R22)

**Proxy:**

* Added option to use an interface as `proxy`. This feature allows an client to use an CIoT device as proxy to send an message to another CIoT device as below sample:

```
ciot_mqtt_client (ciot application) ------> ciot_mqtt_client (ciot device 1) ---proxy---> ciot_uart (ciot device 1) ------> ciot_uart (ciot device 2) 
``` 

This can be achieved adding an proxy configuration to the message sent by ciot application to ciot device 1

```json
// message sent by ciot application
{
    "message": {
        "id": 0,
        // ciot device 2 interface
        "iface": {
            "id": 1,
            "type": "IFACE_TYPE_SYS"
        },
        // message to sent to ciot device 2
        "data": {
            "get_data": {
                "type": "DATA_TYPE_INFO"
            }
        },
        // ciot device 1 proxy used to send message
        "proxy": {
            "iface": {
                "id": 9,
                "type": "IFACE_TYPE_UART"
            }
        }
    }
}
```