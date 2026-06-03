# ciot_c Release

### Changes

**Wi-Fi AP client management improvements:**

* Added logging to show the current number of AP clients whenever a client connects or disconnects, making it easier to monitor client activity.
* Ensured that the AP client count (`conn_count`) is decremented only when greater than zero, preventing negative counts.
* Updated the AP state to `CIOT_TCP_STATE_CONNECTED` if there are still clients connected, or to `CIOT_TCP_STATE_STARTED` if no clients remain, ensuring the AP state accurately reflects the connection status.