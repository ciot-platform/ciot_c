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

* `ciot_mbus_server_start` now stops the running transport, the one in the old config, when the config switches between RTU and TCP. It closes the TCP socket, or the RS485 UART when the server started it (`rtu.has_uart`). A new config of the same type is applied as before. `start` also resets `status.error`, which stayed at `CIOT_ERR_TIMEOUT` after a TCP client disconnected (`src/common/ciot_mbus_server.c`).
* Cause of the missing response: after `ciot_mbus_server_start` sends `STARTED`, the core waits in `PENDING_EVENT` until the next tick to send the response. Events from another iface in that state overwrote the pending event, and the response was lost. The second switch to TCP reused the old listener, and accepting the stale connections in its backlog sent `STARTED` from the socket iface right inside that window. `ciot_iface_event_handler` now treats `PENDING_EVENT` like `BUSY`: events from another iface are dropped (the busy handler is called), and events from the same iface still replace the pending one (`src/core/ciot.c`).
* Mongoose: `ciot_socket_stop` marks the listener and the client connection `is_closing` instead of calling `mg_close_conn`. `mg_close_conn` only unlinks the connection from the manager and never closes the OS socket, so the port stayed bound, and starting the server again failed with `EADDRINUSE` (`src/mg/ciot_socket.c`).
* ESP32: `ciot_uart_stop` no longer deletes the caller. A UART without a decoder (e.g. RS485) has no event task, and `vTaskDelete(NULL)` deleted the task calling `ciot_uart_stop`, the application's main loop, until the task watchdog reset the device. The event task, when it exists, is now deleted before `uart_driver_delete` frees the queue it waits on (`src/esp32/ciot_uart.c`).

**Version:**

* `CIOT_VER` bumped to `0.27.0.0`, opening the 0.27 alpha cycle after the v0.26.0.5 stable release.
* `CIOT_VER` bumped to `0.27.0.1` (ethernet stop fix).
* `CIOT_VER` bumped to `0.27.0.2` (Modbus server transport switch fix).
