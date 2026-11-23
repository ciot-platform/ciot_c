# ciot_c Release

### Changes

**nRF DFU: new session on an open iface (#106):**

* Added `ciot_dfu_nrf_reset(self)`: ends the current session without closing the transport iface. It clears the session state (states, counters, CRC, current object, the ping retry, the SLIP decoder) and releases the iface event handler, so the next `ciot_dfu_nrf_start` begins a new session on the iface that is still open. `ciot_dfu_nrf_stop` now runs the same reset and then closes the iface, as before (`include/ciot_dfu_nrf.h`, `src/common/ciot_dfu_nrf.c`).
* Use it between sessions on the same link, e.g. after a ping-only session (`ciot_dfu_nrf_set_test_mode`), which ends in `COMPLETED` with the iface open. Until now the only way to start again was `ciot_dfu_nrf_stop`, which on ESP32 deletes the UART driver (`uart_driver_delete`); the next start installs it again. On an ESP32 with an nRF52840 bootloader on UART0, every session after a delete + reinstall got no bytes back (ping sent, 0 bytes received, `CIOT_ERR_TIMEOUT`), while sessions that kept the driver installed all got the ping response.
* The first ping refused with `OP_CODE_NOT_SUPPORTED` is now retried once per session. `ping_refused` was never cleared, so the retry was allowed only once per `ciot_dfu_nrf_t`.
* Added `ciot_decoder_slip_reset(decoder)`: drops a partially decoded frame. The DFU reset uses it, so a frame cut at the end of a session no longer corrupts the first response of the next one (`include/ciot_decoder_slip.h`, `src/common/ciot_decoder_slip.c`).
* Tests: `tests/common/ciot_dfu_nrf_test.c` (reset vs stop on a mock iface, ping retry per session, partial frame) and `test_ciot_decoder_slip_reset_drops_partial_frame`.
