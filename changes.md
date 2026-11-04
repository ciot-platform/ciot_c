# ciot_c Release

### Changes

**Busy notification in ciot core:**

* Added `ciot_set_busy_handler(ciot, handler, args)`. When `ciot_iface_event_handler` drops an event with `CIOT_ERR_BUSY`, the registered handler is called with the event exactly as received; `MSG` events are still raw. Until now senders such as the ESP32 MQTT client ignored this return value, so requests dropped this way got no response. The application can now decode the event and notify the requester. Without a handler, behavior is unchanged (`include/ciot.h`, `src/core/ciot.c`).
* Both drop points call the handler: the core is `BUSY` and the event comes from another iface, or a `MSG` event arrives while the core is not `STARTED` (`STARTING` or `PENDING_EVENT`).
* The handler runs in the sender's context. On ESP32 that is the esp-mqtt or UART task, not the task running `ciot_task`.

**Fixes:**

* The "same iface" check in the `BUSY` state only runs for events that carry a decoded msg. For raw `MSG` events it read `event->msg.iface` from the union's raw bytes, and a false match could move the core to `PENDING_EVENT` and reprocess the previous event (`src/core/ciot.c`).
* `MSG` events dropped because the core is not started are now logged at error level with the iface and event (`src/core/ciot.c`).
* On ESP32, `ciot_eth_stop` now sets `cfg.disabled = true` on the stored config, as `ciot_wifi_stop` already does. A config saved after stopping ethernet now keeps it disabled; before, the saved config was still enabled and ethernet came back on at boot. The next start with an enabled config resets the flag (`src/esp32/ciot_eth.c`).

**Modbus server transport switch (#87):**

Switching `mbus_server` between RTU and TCP without a reboot left the previous transport running. Back in RTU, port 502 still accepted connections that nobody answered. On the next switch to TCP, the config request got no response (HTTP 408), although the config was applied.

* `ciot_mbus_server_start` now stops the running transport, the one in the old config, when the config switches between RTU and TCP. It closes the TCP socket, or the RS485 UART when the server started it (`rtu.has_uart`). The new transport starts first. If it fails, the call returns the error and the server keeps running on the old transport and config, as it already did for an invalid config of the same type. A new config of the same type is applied as before (`src/common/ciot_mbus_server.c`).
* `start` sets `status.error` from the result of the start. It stayed at `CIOT_ERR_TIMEOUT` after a TCP client disconnected. An RTU `server_id` of 0 or above 255 is now rejected with `CIOT_ERR_INVALID_ARG` before any transport is touched. nanomodbus takes a one-byte address, so 256 used to wrap to 0, and 257 silently became address 1. `stop` closes the UART started by the server even when a previous start failed (`src/common/ciot_mbus_server.c`).
* Cause of the missing response: after `ciot_mbus_server_start` sends `STARTED`, the core waits in `PENDING_EVENT` until the next tick to send the response. Events from another iface in that state overwrote the pending event, and the response was lost. The second switch to TCP reused the old listener, and accepting the stale connections in its backlog sent `STARTED` from the socket iface right inside that window. `ciot_iface_event_handler` now treats `PENDING_EVENT` like `BUSY`: events from another iface are dropped (the busy handler is called), and events from the same iface still replace the pending one (`src/core/ciot.c`).
* Mongoose: `ciot_socket_stop` marks the listener and the client connection `is_closing` instead of calling `mg_close_conn`. `mg_close_conn` only unlinks the connection from the manager and never closes the OS socket, so the port stayed bound, and starting the server again failed with `EADDRINUSE`. `stop` also closes the OS socket right away, so a start on the same port can bind as soon as `stop` returns, without waiting for a `mg_mgr_poll` (`src/mg/ciot_socket.c`).
* ESP32: `ciot_uart_stop` no longer deletes the caller. A UART without a decoder (e.g. RS485) has no event task, and `vTaskDelete(NULL)` deleted the task calling `ciot_uart_stop`, the application's main loop, until the task watchdog reset the device. The event task, when it exists, is now deleted before `uart_driver_delete` frees the queue it waits on. When `ciot_uart_stop` runs in that task itself, the task is deleted last, as before (`src/esp32/ciot_uart.c`).

**Modbus server: last holding register never written:**

* `ciot_mbus_server_set_reg` and `ciot_mbus_server_get_reg` compared the register index with `regs.count - size`, but `size` is in bytes and each register is 2 bytes. A 16-bit access to the last register (index `count - 1`) was rejected with `CIOT_ERR_INVALID_INDEX`, so that register stayed at 0. In hg_tcp, the 125th TAG (40125) was updated in the bridge but read 0 over Modbus. The check now converts `size` to registers and only rejects accesses that end past the last one. Reads through Modbus (FC03) already used the right limit (`src/common/ciot_mbus_server.c`).

**Frames sent whole on nRF, and send errors reach the caller (#92):**

* `ciot_decoder_s_send` builds the whole frame (`{`, size, payload, `}`) in a buffer of the decoder and hands it to the iface in a single `send_data` call, then returns its error. Before, it called `send_data` four times and always returned `CIOT_ERR_OK`, so `ciot_iface_send_msg` and `ciot_iface_send_rsp` reported success for frames that were cut or never sent. A payload larger than `CIOT_CONFIG_MSG_SIZE` now returns `CIOT_ERR_OVERFLOW` without sending anything. `ciot_decoder_s_new` allocates the `CIOT_CONFIG_MSG_SIZE + 4` byte buffer, kept off the stack because `ciot_iface_send` already holds a `CIOT_CONFIG_MSG_SIZE` buffer there (`src/common/ciot_decoder_s.c`).
* nRF UART: `ciot_uart` uses `nrf_drv_uart` with its own TX FIFO instead of `app_uart_fifo`, whose FIFO has no free-space API. `ciot_uart_send_bytes` queues the bytes only if all of them fit in the FIFO (`CIOT_CONFIG_UART_TX_BUF_SIZE`); otherwise it queues nothing and returns `CIOT_ERR_OVERFLOW`. Before, it wrote byte by byte and left the start of the frame in the FIFO when it filled up, which also broke the next frame on the receiver ("Missing terminator"). The bytes go to the driver in chunks of up to `CIOT_CONFIG_UART_TX_CHUNK_SIZE` (32) instead of one per transfer. `ciot_uart_read_bytes` returns `CIOT_ERR_NOT_IMPLEMENTED`: received bytes already went to the iface from the driver event, so it had nothing to read (`src/nrf/ciot_uart.c`).
* nRF USB CDC: the same all-or-nothing check now covers the whole frame. Before, each of the four parts was checked on its own, so the host got `{` + size + `}` without the payload (e.g. `{ 0x35 0x00 }`). The CDC sends up to 64 bytes per transfer instead of 1, and bytes leave the FIFO only after `app_usbd_cdc_acm_write` accepts them. Before, a refused write (e.g. port closed) lost the byte taken from the FIFO; now what was queued goes out when the port opens (`src/nrf/ciot_usb.c`).
* On both, the free-space check, the write and the TX start run in a critical region, so senders at different priorities do not interleave.
* With the error propagated, an application that resends on failure now gets to resend.

**Linux build fix:**

* `src/mg/ciot_socket.c` did not build on Linux in 0.27.0.2 (`call to undeclared function 'closesocket'`). `mongoose.h` declares `closesocket()` only for some architectures (Windows, FreeRTOS); on Unix, `mongoose.c` maps it to `close()` internally. `ciot_socket.c` now does the same when the macro is missing. Windows is unchanged.

**Version:**

* `CIOT_VER` bumped to `0.27.0.0`, opening the 0.27 alpha cycle after the v0.26.0.5 stable release.
* `CIOT_VER` bumped to `0.27.0.1` (ethernet stop fix).
* `CIOT_VER` bumped to `0.27.0.2` (Modbus server transport switch fix).
* `CIOT_VER` bumped to `0.27.0.3` (Linux build fix).
* `CIOT_VER` bumped to `0.27.0.4` (Modbus server last register fix).
