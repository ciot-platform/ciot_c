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

**Build Script Update:**

* Updated the `make/scripts.mk` script to use new environment variable names and the correct path for the MQTT translator script.