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
* On nRF (S140), `ciot_ble_adv_send_bytes` advertises only the bytes of the message: it sets `adv_data.len` to the message size and reconfigures the stopped adv set before `sd_ble_gap_adv_start`. Before, the adv data always had 31 bytes and the buffer was not cleared, so a message shorter than the previous one went out with the tail of the previous one, which scanners read as extra AD structures (`src/nrf/ciot_ble_adv.c`).

**Modbus server transport switch (#87):**

Switching `mbus_server` between RTU and TCP without a reboot left the previous transport running. Back in RTU, port 502 still accepted connections that nobody answered. On the next switch to TCP, the config request got no response (HTTP 408), although the config was applied.

* `ciot_mbus_server_start` now stops the running transport, the one in the old config, when the config switches between RTU and TCP. It closes the TCP socket, or the RS485 UART when the server started it (`rtu.has_uart`). The new transport starts first. If it fails, the call returns the error and the server keeps running on the old transport and config, as it already did for an invalid config of the same type. A new config of the same type is applied as before (`src/common/ciot_mbus_server.c`).
* `start` sets `status.error` from the result of the start. It stayed at `CIOT_ERR_TIMEOUT` after a TCP client disconnected. An RTU `server_id` of 0 or above 255 is now rejected with `CIOT_ERR_INVALID_ARG` before any transport is touched. nanomodbus takes a one-byte address, so 256 used to wrap to 0, and 257 silently became address 1. `stop` closes the UART started by the server even when a previous start failed (`src/common/ciot_mbus_server.c`).
* Cause of the missing response: after `ciot_mbus_server_start` sends `STARTED`, the core waits in `PENDING_EVENT` until the next tick to send the response. Events from another iface in that state overwrote the pending event, and the response was lost. The second switch to TCP reused the old listener, and accepting the stale connections in its backlog sent `STARTED` from the socket iface right inside that window. `ciot_iface_event_handler` now treats `PENDING_EVENT` like `BUSY`: events from another iface are dropped (the busy handler is called), and events from the same iface still replace the pending one (`src/core/ciot.c`).
* Mongoose: `ciot_socket_stop` marks the listener and the client connection `is_closing` instead of calling `mg_close_conn`. `mg_close_conn` only unlinks the connection from the manager and never closes the OS socket, so the port stayed bound, and starting the server again failed with `EADDRINUSE`. `stop` also closes the OS socket right away, so a start on the same port can bind as soon as `stop` returns, without waiting for a `mg_mgr_poll` (`src/mg/ciot_socket.c`).
* ESP32: `ciot_uart_stop` no longer deletes the caller. A UART without a decoder (e.g. RS485) has no event task, and `vTaskDelete(NULL)` deleted the task calling `ciot_uart_stop`, the application's main loop, until the task watchdog reset the device. The event task, when it exists, is now deleted before `uart_driver_delete` frees the queue it waits on. When `ciot_uart_stop` runs in that task itself, the task is deleted last, as before (`src/esp32/ciot_uart.c`).

**Modbus server: last holding register never written:**

* `ciot_mbus_server_set_reg` and `ciot_mbus_server_get_reg` compared the register index with `regs.count - size`, but `size` is in bytes and each register is 2 bytes. A 16-bit access to the last register (index `count - 1`) was rejected with `CIOT_ERR_INVALID_INDEX`, so that register stayed at 0: with 125 holding registers, register 40125 was written by the application but read 0 over Modbus. The check now converts `size` to registers and only rejects accesses that end past the last one. Reads through Modbus (FC03) already used the right limit (`src/common/ciot_mbus_server.c`).

**Frames sent whole on nRF, and send errors reach the caller (#92):**

* `ciot_decoder_s_send` builds the whole frame (`{`, size, payload, `}`) on the caller's stack and hands it to the iface in a single `send_data` call, then returns its error. Before, it called `send_data` four times and always returned `CIOT_ERR_OK`, so `ciot_iface_send_msg` and `ciot_iface_send_rsp` reported success for frames that were cut or never sent. A payload larger than `CIOT_CONFIG_MSG_SIZE` now returns `CIOT_ERR_OVERFLOW` without sending anything. The `CIOT_CONFIG_MSG_SIZE + 4` byte frame buffer is per call, so senders in different tasks or at different interrupt priorities never share it; a send needs that much more stack (`src/common/ciot_decoder_s.c`).
* nRF UART: `ciot_uart` uses `nrf_drv_uart` with its own TX FIFO instead of `app_uart_fifo`, whose FIFO has no free-space API. `ciot_uart_send_bytes` queues the bytes only if all of them fit in the FIFO (`CIOT_CONFIG_UART_TX_BUF_SIZE`); otherwise it queues nothing and returns `CIOT_ERR_OVERFLOW`. Before, it wrote byte by byte and left the start of the frame in the FIFO when it filled up, which also broke the next frame on the receiver ("Missing terminator"). The bytes go to the driver in chunks of up to `CIOT_CONFIG_UART_TX_CHUNK_SIZE` (32) instead of one per transfer. `ciot_uart_read_bytes` returns `CIOT_ERR_NOT_IMPLEMENTED`: received bytes already went to the iface from the driver event, so it had nothing to read (`src/nrf/ciot_uart.c`).
* nRF USB CDC: the same all-or-nothing check now covers the whole frame. Before, each of the four parts was checked on its own, so the host got `{` + size + `}` without the payload (e.g. `{ 0x35 0x00 }`). The CDC sends up to 64 bytes per transfer instead of 1, and bytes leave the FIFO only after `app_usbd_cdc_acm_write` accepts them. Before, a refused write lost the byte taken from the FIFO. While no host has the port open, `ciot_usb_send_bytes` queues nothing and returns `CIOT_ERR_CLOSED`, and closing the port drops what the host did not read, so a host that opens it later does not get old frames (`src/nrf/ciot_usb.c`).
* On both, the free-space check, the write and the start of each transfer (also from `TX_DONE`) run in a critical region, so senders at different priorities do not interleave or overwrite a chunk being transmitted.
* With the error propagated, an application that resends on failure now gets to resend.

**Linux build fix:**

* `src/mg/ciot_socket.c` did not build on Linux in 0.27.0.2 (`call to undeclared function 'closesocket'`). `mongoose.h` declares `closesocket()` only for some architectures (Windows, FreeRTOS); on Unix, `mongoose.c` maps it to `close()` internally. `ciot_socket.c` now does the same when the macro is missing. Windows is unchanged.

**Core: option to stop dropping data from a device while the core is busy (#89):**

* New `CIOT_CONFIG_DIRECT_MSG` (default `0`, `include/ciot.h`). With `0`, messages take the receiver as before; the items below about the `direct` buffer and the sender's context apply only with `1`. An application that sets it must make its event handler safe to call from the sender's context (another task, or an interrupt on nRF) at the same time as from `ciot_task`.
* A raw `MSG` is first decoded into a buffer apart from the receiver (`direct`, one `ciot_event_t` in `struct ciot`), whether the core is busy or not. `ciot_iface_event_handler` then checks whether it needs the receiver (the busy state and `ciot_busy_task`): proxied messages, requests for a core iface, and answers to a request pending on the sender. Anything else goes straight to the application handler, in the sender's context, without making the core busy and without touching `receiver.event`, so it cannot overwrite a request another sender is about to hand to the core. This was already the case for `CUSTOM` messages. It now also covers non-request messages from a sender with no pending request, e.g. acks that a device sends as responses without an iface. Before, each of them made the core `BUSY` until the next `ciot_task`, where `ciot_busy_task` only called the application handler. On ESP32 that can take up to 100 ms (`ciot_sys_task` wait), and every frame that arrived in between was dropped (`src/core/ciot.c`).
* While the core is `BUSY` or `PENDING_EVENT`, a message from another iface that does not need the receiver now reaches the application handler instead of being dropped. Requests and answers are still dropped and reach the busy handler as before.
* The `direct` buffer serves one sender at a time: a critical section on nRF, ESP32 and ESP8266, an atomic test-and-set elsewhere. A sender that finds it in use follows the receiver path, as before: it takes the receiver and reaches the application from `ciot_task` (only `CUSTOM` messages are still dispatched from `receiver.event` right away). With the option set, `struct ciot` grows by one `ciot_event_t` (896 B on nRF with `CIOT_CONFIG_MSG_SIZE` 256) (`include/ciot.h`, `src/core/ciot.c`).
* Either way, a `MSG` that fails to decode is dropped with `CIOT_ERR_DESERIALIZATION`. Before, a partly decoded message took the receiver anyway.
* Seen on an ESP32 with an nRF52840 on its UART, during load tests: advs and acks from the nRF were dropped with `ciot busy. UART(9) evt:EVENT_MSG ignored`, 314 times in about 10 minutes of log (232 with the nRF scanning every advertiser, 82 at its default scan level). These frames are not requests, so no busy handler could answer for them.
* Either way, as before, an error from the application handler is sent back only for `CUSTOM` messages. It now carries the iface id of the message; before, the iface type was sent as the id. It is sent with `ciot_iface_send_msg` and no longer clears a request pending on the sender (`ciot_iface_send_error` did), so the answer to that request is still forwarded.
* Not changed: status events (`STARTED`, `STOPPED`, ...) still take the receiver, and an HTTP request dropped as busy still gets no response (the HTTP case of #89).

**Free memory on nRF (#96):**

* `ciot_sys_task` on nRF now fills `status.free_memory`, which was always 0. It is what the application can still allocate: the RAM between the end of the heap (`_sbrk(0)`) and the bottom of the stack reservation (`STACK_BASE`), plus the blocks freed back to `malloc` (`mallinfo().fordblks`), the same meaning as `esp_get_free_heap_size()` on ESP32. The heap can grow past `__HeapLimit` because `_sbrk` from `libnosys` does not check it, so the RAM up to the stack counts as free. Refreshed once per second, since `mallinfo` walks the free list, and read in a critical region, since newlib-nano has no malloc lock without an RTOS. Only with newlib (`_NEWLIB_VERSION`); other runtimes keep reporting 0 (`src/nrf/ciot_sys.c`).
* The stack depth is not included: `STACK_BASE` is the bottom of the stack reservation, not how deep the stack has gone.
* Read first in `ciot_sys_new`, then refreshed by `ciot_sys_task` when `lifetime` changes; while `ciot_timer` is not running it keeps that first value, as `lifetime` does. Only newlib `malloc` is covered: the value is too high with an `_sbrk` that stops at `__HeapLimit`, and an RTOS heap (e.g. FreeRTOS) is not seen.
* `ciot_sys_new` on nRF returns NULL when `calloc` fails, instead of writing through NULL in `ciot_sys_init`.

**Scan responses and scanner hold on nRF:**

* `ciot_ble_scn_event_adv_report_t` has a new `scan_response` field, set on nRF from the SoftDevice report (`adv_report.type.scan_response`). The scan filter can now tell a scan response (SCAN_RSP) from an advert. An empty scan response reaches the filter with `payload_len` 0 (`include/ciot_ble_scn.h`, `src/nrf/ciot_ble.c`).
* New `ciot_ble_scn_hold` and `ciot_ble_scn_resume` (nRF). Called from the scan filter, `ciot_ble_scn_hold` stops the scanner after the report being handled, and the report handler skips `ciot_ble_scn_continue`. The radio is then free, for example for an advertising burst whose first event must go out right away. Only skipping `ciot_ble_scn_continue` (the scanner paused by the SoftDevice) is not enough: the paused scanner keeps its scan window, and on a bench the first advertising event still waited ~110 ms for its end. Don't call it on the report of a scannable advert: stopping there aborts the SCAN_REQ that follows. `ciot_ble_scn_resume` restarts the scanner with its current parameters (a scan timeout starts again) and can be called from an interrupt (a timer); without a hold it does nothing. `ciot_ble_scn_start` and `ciot_ble_scn_stop` clear the hold (`src/nrf/ciot_ble_scn.c`).

**Tests:**

* Requests answered by an event of the target iface (e.g. a start acked by `STARTED`) are now covered: an ack sent from inside the request, while the core is busy with it, is kept in `PENDING_EVENT` and answers the request on the next `ciot_task`; an event from another iface in that window is dropped (busy handler) and does not overwrite the ack; an ack sent later does not keep the core busy meanwhile, and other requests are taken and answered before it arrives (`tests/common/ciot_busy_test.c`).

**Version:**

* `CIOT_VER` bumped to `0.27.0.0`, opening the 0.27 alpha cycle after the v0.26.0.5 stable release.
* `CIOT_VER` bumped to `0.27.0.1` (ethernet stop fix).
* `CIOT_VER` bumped to `0.27.0.2` (Modbus server transport switch fix).
* `CIOT_VER` bumped to `0.27.0.3` (Linux build fix).
* `CIOT_VER` bumped to `0.27.0.4` (Modbus server last register fix).
* `CIOT_VER` bumped to `0.27.0.5` (adv data length on nRF).
* `CIOT_VER` bumped to `0.27.0.6` (whole frames and send errors on nRF).
* `CIOT_VER` bumped to `0.27.0.7` (`CIOT_CONFIG_DIRECT_MSG`: device data no longer dropped while the core is busy).
* `CIOT_VER` bumped to `0.27.0.8` (free memory on nRF).
* `CIOT_VER` bumped to `0.27.0.9` (scan responses and scanner hold on nRF).
