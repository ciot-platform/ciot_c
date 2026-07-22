# ciot_c Release

### Changes

**BLE SCN Advertisement FIFO Improvements:**

* Introduced a new `ciot_ble_scn_adv_fifo_slot_t` struct with a `locked` flag for each FIFO slot, and updated the FIFO to use this structure for improved thread-safety. (`include/ciot_ble_scn.h`, `src/common/ciot_ble_scn_base.c`) [[1]](diffhunk://#diff-9cc3ff9f4c60d80c2855302798490b054d719bbcb6706bb5179002e28506ccbaR40-R50) [[2]](diffhunk://#diff-1d56b9dfd453aa5d1584b74b104467528a195ea593194e1b200838f769b93ea2L128-L196)
* Added new public API functions for the FIFO: `ciot_ble_scn_adv_fifo_pop`, `ciot_ble_scn_adv_fifo_count`, and `ciot_ble_scn_adv_fifo_lost`. These provide safe access and monitoring of the FIFO state. (`include/ciot_ble_scn.h`, `src/common/ciot_ble_scn_base.c`) [[1]](diffhunk://#diff-9cc3ff9f4c60d80c2855302798490b054d719bbcb6706bb5179002e28506ccbaR87-R92) [[2]](diffhunk://#diff-1d56b9dfd453aa5d1584b74b104467528a195ea593194e1b200838f769b93ea2L128-L196)
* Refactored and improved FIFO push/pop logic to handle contention, overflow, and error reporting, and to track lost advertisements. (`src/common/ciot_ble_scn_base.c`)
* Changed configuration macros from `CIOT_CONFIG_BLE_SCN_ADV_FIFO_SIZE` to `CIOT_CONFIG_BLE_SCN_ADV_FIFO_ENABLED` for better feature toggling. (`src/common/ciot_ble_scn_base.c`) [[1]](diffhunk://#diff-1d56b9dfd453aa5d1584b74b104467528a195ea593194e1b200838f769b93ea2R19-R35) [[2]](diffhunk://#diff-1d56b9dfd453aa5d1584b74b104467528a195ea593194e1b200838f769b93ea2L42-R48) [[3]](diffhunk://#diff-1d56b9dfd453aa5d1584b74b104467528a195ea593194e1b200838f769b93ea2L218-R283)

**NRF UART Refactor:**

* Refactored the NRF UART implementation to use the higher-level `app_uart` driver, removing custom FIFO management and direct use of `nrf_drv_uart`. (`src/nrf/ciot_uart.c`) [[1]](diffhunk://#diff-ffccc2f85616dfd12ec2dbc6a3a71126f30eb2c9fdd873a1d71ee6f6fb46885fL16-R29) [[2]](diffhunk://#diff-ffccc2f85616dfd12ec2dbc6a3a71126f30eb2c9fdd873a1d71ee6f6fb46885fR43-R48) [[3]](diffhunk://#diff-ffccc2f85616dfd12ec2dbc6a3a71126f30eb2c9fdd873a1d71ee6f6fb46885fL74-L121) [[4]](diffhunk://#diff-ffccc2f85616dfd12ec2dbc6a3a71126f30eb2c9fdd873a1d71ee6f6fb46885fL132-R98) [[5]](diffhunk://#diff-ffccc2f85616dfd12ec2dbc6a3a71126f30eb2c9fdd873a1d71ee6f6fb46885fL143-R135)
* Simplified UART event handling and error reporting by using `app_uart_evt_t` events and global instance management. (`src/nrf/ciot_uart.c`)

**Modbus Server Improvements:**

* Added configurable timeout macros for Modbus server read and byte timeouts in `ciot_mbus_server.h`, allowing easier adjustment of communication timeouts.
* Added an `nmbs_initialized` flag to the `ciot_mbus_server` struct to track initialization state, supporting safer resource management.
* Updated the `ciot_mbus_server_start` function to return errors from `ciot_uart_start` using `CIOT_ERR_RETURN`, improving error propagation.
* Implemented the `ciot_mbus_server_stop` function to properly stop the UART connection, reset the Modbus server state, and send a stopped event, replacing the previous "not implemented" stub.

**UART Handling and Logging:**

* Improved UART buffer full event logging in `ciot_uart_event_handler` to include both the event size and the actual RX buffer usage, aiding in debugging buffer overflows.
* Added a local `rx_buffer_used` variable in the UART event handler to support the improved logging.
* Changed the `log_buffer` in `ciot_logger.c` from global to static scope for better encapsulation and to avoid namespace pollution.