# ciot_c Release

### Changes

**UART Improvements:**

* Implemented the `ciot_uart_stop` function to properly stop the UART interface, including logging, state management, resource cleanup, and event notification. Previously, this function was not implemented.

**DFU (Device Firmware Update) Enhancements:**

* Added an explicit error log for DFU type mismatches in `ciot_dfu_nrf_start`, improving debuggability when an incorrect DFU type is provided.
* Improved status updates in the DFU write process by ensuring `image_written` is updated after data transfer, and cleaned up redundant status assignments. [[1]](diffhunk://#diff-fbe65a7e710b59bf67db57d1d53ed72fe347e8ea6b9729c6ce64008109b8f793L381-R382) [[2]](diffhunk://#diff-fbe65a7e710b59bf67db57d1d53ed72fe347e8ea6b9729c6ce64008109b8f793R397-R398)

**Error Code Updates:**

* Added a new error code `CIOT_ERR_DEPRECATED` to signal deprecated features, and updated macro definitions accordingly. [[1]](diffhunk://#diff-c99f0b7a3810a686c62028202333f39e99321dfa4dfbc2637e6bd30f6d3eea6bR66) [[2]](diffhunk://#diff-c99f0b7a3810a686c62028202333f39e99321dfa4dfbc2637e6bd30f6d3eea6bR138)