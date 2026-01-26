# ciot_c Release

### Changes

**Proxy request handling improvements:**

* Added a new condition in `ciot_busy_task` (in `ciot.c`) to save interface configuration data when a proxy request with `save` set to true is received. This involves generating a filename, saving the data to storage, updating the proxy state, sending a response, and logging the operation.

**Minor formatting fix:**

* Moved the `#endif // CONFIG_ESP_HTTPS_OTA_DECRYPT_CB` directive to after the function declaration in `ciot_ota.c` to correctly scope conditional compilation.