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
* On nRF (S140), `ciot_ble_adv_send_bytes` advertises only the bytes of the message: it sets `adv_data.len` to the message size and reconfigures the stopped adv set before `sd_ble_gap_adv_start`. Before, the adv data always had 31 bytes and the buffer was not cleared, so a message shorter than the previous one went out with the tail of the previous one, which scanners read as extra AD structures (`src/nrf/ciot_ble_adv.c`, hedrosistemas/hg_ble#101).

**Modbus server transport switch (#87):**

Switching `mbus_server` between RTU and TCP without a reboot left the previous transport running. Back in RTU, port 502 still accepted connections that nobody answered. On the next switch to TCP, the config request got no response (HTTP 408), although the config was applied.

* `ciot_mbus_server_start` now stops the running transport, the one in the old config, when the config switches between RTU and TCP. It closes the TCP socket, or the RS485 UART when the server started it (`rtu.has_uart`). The new transport starts first. If it fails, the call returns the error and the server keeps running on the old transport and config, as it already did for an invalid config of the same type. A new config of the same type is applied as before (`src/common/ciot_mbus_server.c`).
* `start` sets `status.error` from the result of the start. It stayed at `CIOT_ERR_TIMEOUT` after a TCP client disconnected. An RTU `server_id` of 0 or above 255 is now rejected with `CIOT_ERR_INVALID_ARG` before any transport is touched. nanomodbus takes a one-byte address, so 256 used to wrap to 0, and 257 silently became address 1. `stop` closes the UART started by the server even when a previous start failed (`src/common/ciot_mbus_server.c`).
* Cause of the missing response: after `ciot_mbus_server_start` sends `STARTED`, the core waits in `PENDING_EVENT` until the next tick to send the response. Events from another iface in that state overwrote the pending event, and the response was lost. The second switch to TCP reused the old listener, and accepting the stale connections in its backlog sent `STARTED` from the socket iface right inside that window. `ciot_iface_event_handler` now treats `PENDING_EVENT` like `BUSY`: events from another iface are dropped (the busy handler is called), and events from the same iface still replace the pending one (`src/core/ciot.c`).
* Mongoose: `ciot_socket_stop` marks the listener and the client connection `is_closing` instead of calling `mg_close_conn`. `mg_close_conn` only unlinks the connection from the manager and never closes the OS socket, so the port stayed bound, and starting the server again failed with `EADDRINUSE`. `stop` also closes the OS socket right away, so a start on the same port can bind as soon as `stop` returns, without waiting for a `mg_mgr_poll` (`src/mg/ciot_socket.c`).
* ESP32: `ciot_uart_stop` no longer deletes the caller. A UART without a decoder (e.g. RS485) has no event task, and `vTaskDelete(NULL)` deleted the task calling `ciot_uart_stop`, the application's main loop, until the task watchdog reset the device. The event task, when it exists, is now deleted before `uart_driver_delete` frees the queue it waits on. When `ciot_uart_stop` runs in that task itself, the task is deleted last, as before (`src/esp32/ciot_uart.c`).

**Modbus server: last holding register never written:**

* `ciot_mbus_server_set_reg` and `ciot_mbus_server_get_reg` compared the register index with `regs.count - size`, but `size` is in bytes and each register is 2 bytes. A 16-bit access to the last register (index `count - 1`) was rejected with `CIOT_ERR_INVALID_INDEX`, so that register stayed at 0. In hg_tcp, the 125th TAG (40125) was updated in the bridge but read 0 over Modbus. The check now converts `size` to registers and only rejects accesses that end past the last one. Reads through Modbus (FC03) already used the right limit (`src/common/ciot_mbus_server.c`).

**Linux build fix:**

* `src/mg/ciot_socket.c` did not build on Linux in 0.27.0.2 (`call to undeclared function 'closesocket'`). `mongoose.h` declares `closesocket()` only for some architectures (Windows, FreeRTOS); on Unix, `mongoose.c` maps it to `close()` internally. `ciot_socket.c` now does the same when the macro is missing. Windows is unchanged.

**Core: data from a device is no longer dropped while the core is busy (#89):**

* A raw `MSG` is first decoded into a buffer apart from the receiver (`direct`, one `ciot_event_t` in `struct ciot`), whether the core is busy or not. `ciot_iface_event_handler` then checks whether it needs the receiver (the busy state and `ciot_busy_task`): proxied messages, requests for a core iface, and answers to a request pending on the sender. Anything else goes straight to the application handler, in the sender's context, without making the core busy and without touching `receiver.event`, so it cannot overwrite a request another sender is about to hand to the core. This was already the case for `CUSTOM` messages. It now also covers non-request messages from a sender with no pending request, e.g. acks that a device sends as responses without an iface. Before, each of them made the core `BUSY` until the next `ciot_task`, where `ciot_busy_task` only called the application handler. On ESP32 that can take up to 100 ms (`ciot_sys_task` wait), and every frame that arrived in between was dropped (`src/core/ciot.c`).
* While the core is `BUSY` or `PENDING_EVENT`, a message from another iface that does not need the receiver now reaches the application handler instead of being dropped. Requests and answers are still dropped and reach the busy handler as before.
* The `direct` buffer serves one sender at a time: a critical section on nRF, ESP32 and ESP8266, an atomic test-and-set elsewhere. A sender that finds it in use follows the receiver path, as before. `struct ciot` grows by one `ciot_event_t` (896 B on nRF with `CIOT_CONFIG_MSG_SIZE` 256) (`include/ciot.h`, `src/core/ciot.c`).
* A `MSG` that fails to decode is dropped with `CIOT_ERR_DESERIALIZATION`. Before, a partly decoded message took the receiver anyway.
* Seen on an ESP32 with an nRF52840 on its UART, during load tests: advs and acks from the nRF were dropped with `ciot busy. UART(9) evt:EVENT_MSG ignored`, 314 times in about 10 minutes of log (232 with the nRF scanning every advertiser, 82 at its default scan level). These frames are not requests, so no busy handler could answer for them.
* As before, an error from the application handler is sent back only for `CUSTOM` messages. It now carries the iface id of the message; before, the iface type was sent as the id.
* Not changed: status events (`STARTED`, `STOPPED`, ...) still take the receiver, and an HTTP request dropped as busy still gets no response (the HTTP case of #89).

**Version:**

* `CIOT_VER` bumped to `0.27.0.0`, opening the 0.27 alpha cycle after the v0.26.0.5 stable release.
* `CIOT_VER` bumped to `0.27.0.1` (ethernet stop fix).
* `CIOT_VER` bumped to `0.27.0.2` (Modbus server transport switch fix).
* `CIOT_VER` bumped to `0.27.0.3` (Linux build fix).
* `CIOT_VER` bumped to `0.27.0.4` (Modbus server last register fix).
* `CIOT_VER` bumped to `0.27.0.7` (device data no longer dropped while the core is busy).
