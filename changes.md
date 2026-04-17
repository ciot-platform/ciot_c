# ciot_c Release

### Changes

**Modbus Server Enhancements:**

* Added new status fields to `ciot_mbus_server_status_t` to track error codes, timestamps for last pool, update, and request, as well as request count. These fields are now updated in the server logic to provide more detailed runtime information. [[1]](diffhunk://#diff-2879311ce531e391c9c16a7e6a20f966629ea88f91d30ecbf7f0844f3ed63f0aR54-R58) [[2]](diffhunk://#diff-2879311ce531e391c9c16a7e6a20f966629ea88f91d30ecbf7f0844f3ed63f0aL101-R115) [[3]](diffhunk://#diff-2879311ce531e391c9c16a7e6a20f966629ea88f91d30ecbf7f0844f3ed63f0aR127-R131) [[4]](diffhunk://#diff-2879311ce531e391c9c16a7e6a20f966629ea88f91d30ecbf7f0844f3ed63f0aL154-R171) [[5]](diffhunk://#diff-7692b439c4b1916834495a096ae6cdb3cbb16652175e7d0b1240cd81dcd8a909L116-R119) [[6]](diffhunk://#diff-7692b439c4b1916834495a096ae6cdb3cbb16652175e7d0b1240cd81dcd8a909R133) [[7]](diffhunk://#diff-7692b439c4b1916834495a096ae6cdb3cbb16652175e7d0b1240cd81dcd8a909L169-R176) [[8]](diffhunk://#diff-7692b439c4b1916834495a096ae6cdb3cbb16652175e7d0b1240cd81dcd8a909L178-R196) [[9]](diffhunk://#diff-7692b439c4b1916834495a096ae6cdb3cbb16652175e7d0b1240cd81dcd8a909L195-R216) [[10]](diffhunk://#diff-7692b439c4b1916834495a096ae6cdb3cbb16652175e7d0b1240cd81dcd8a909L212-R236)

* Improved error handling in server read/write functions by returning standard NMBS error codes instead of custom enum values. [[1]](diffhunk://#diff-7692b439c4b1916834495a096ae6cdb3cbb16652175e7d0b1240cd81dcd8a909L160-R164) [[2]](diffhunk://#diff-7692b439c4b1916834495a096ae6cdb3cbb16652175e7d0b1240cd81dcd8a909L178-R196) [[3]](diffhunk://#diff-7692b439c4b1916834495a096ae6cdb3cbb16652175e7d0b1240cd81dcd8a909L195-R216) [[4]](diffhunk://#diff-7692b439c4b1916834495a096ae6cdb3cbb16652175e7d0b1240cd81dcd8a909L212-R236)

**Modbus Client and Server Error Type Unification:**

* Changed MBUS client and server error fields to use the unified `ciot_err_t` type, and removed the custom `ciot_mbus_error_t` enum from the protocol definitions. [[1]](diffhunk://#diff-6bf44a776dd28cd8ec06193fb80a558c71c3f9214b21e0596abb4a124d546fd1L54-R55) [[2]](diffhunk://#diff-6bf44a776dd28cd8ec06193fb80a558c71c3f9214b21e0596abb4a124d546fd1L94-R95) [[3]](diffhunk://#diff-6bf44a776dd28cd8ec06193fb80a558c71c3f9214b21e0596abb4a124d546fd1L104-R112) [[4]](diffhunk://#diff-cf8eec702cf8be674eacc67764003b9b6f78cabc7edf27711fce43fdd3974a2cL13-L24) [[5]](diffhunk://#diff-cf8eec702cf8be674eacc67764003b9b6f78cabc7edf27711fce43fdd3974a2cL56-L67) [[6]](diffhunk://#diff-2879311ce531e391c9c16a7e6a20f966629ea88f91d30ecbf7f0844f3ed63f0aR98)

* Updated protocol buffer includes to add `errors.pb.h` where needed to support the new error type. [[1]](diffhunk://#diff-6bf44a776dd28cd8ec06193fb80a558c71c3f9214b21e0596abb4a124d546fd1R9) [[2]](diffhunk://#diff-2879311ce531e391c9c16a7e6a20f966629ea88f91d30ecbf7f0844f3ed63f0aR7)

**UART Start Logic Improvements:**

* Refined the UART start logic in `ciot_uart_start` to prevent changing the UART number without stopping the interface first, and improved logging and error returns for invalid state transitions. [[1]](diffhunk://#diff-c8a5a36ea012ff34598ad0ee6f74aaf1c434a0083acbc5a0c877c116db5bcceaL79-L100) [[2]](diffhunk://#diff-c8a5a36ea012ff34598ad0ee6f74aaf1c434a0083acbc5a0c877c116db5bcceaL113-R113) [[3]](diffhunk://#diff-c8a5a36ea012ff34598ad0ee6f74aaf1c434a0083acbc5a0c877c116db5bcceaR123-R125)

**HTTP Server changes:**

* Increased the default value of `CIOT_CONFIG_HTTP_SERVER_TIMEOUT_MS` from 15000 ms to 30000 ms in `ciot_http_server.c` to allow more time for HTTP server responses. New extended timeout is needed to support wifi reconnection attempts.

**Build Script Update:**

* Updated the `make/scripts.mk` script to use new environment variable names and the correct path for the MQTT translator script.

**Test coverage improvements**

* Added new test data and a test case (`test_ciot_crypt_dec_ok_2`) to `ciot_crypt_test.c` for verifying decryption with a different key and input, increasing cryptographic test coverage. [[1]](diffhunk://#diff-4715a2e7f2ac8777d2b274311ac2cfb1e4459dc0144dba852cfb67e75ca3cfa6R26-R31) [[2]](diffhunk://#diff-4715a2e7f2ac8777d2b274311ac2cfb1e4459dc0144dba852cfb67e75ca3cfa6R154-R166) [[3]](diffhunk://#diff-4715a2e7f2ac8777d2b274311ac2cfb1e4459dc0144dba852cfb67e75ca3cfa6R201)

**Bug fixes and functional improvements**

* Moved the `uart_set_pin` call in `ciot_uart_start` so that UART pins are only set after checking if the UART is already started, preventing unnecessary hardware reconfiguration.
* In `ciot_dfu_nrf_event_handler`, changed the event type check from `CIOT_EVENT_TYPE_MSG` to `CIOT_EVENT_TYPE_DATA` to correctly process incoming data events.
* Improved the interface selection logic in `ciot_tcp_get_addr` (Windows): now only iterates network adapters if the TCP type is Ethernet or WiFi, and breaks out of the loop as soon as a connection is established, optimizing connection setup. [[1]](diffhunk://#diff-fd3885fdf60ec1ccd9aef56ede22dc496c00a524e4c0131e794d7aca2a5f1e8aR97-R102) [[2]](diffhunk://#diff-fd3885fdf60ec1ccd9aef56ede22dc496c00a524e4c0131e794d7aca2a5f1e8aR116-R127)

**Version update**

* Incremented the `CIOT_VER` macro to `0,21,0,2` to reflect these changes.