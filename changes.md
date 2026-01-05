# ciot_c Release

### Changes

**Error handling improvements:**

* Added a new macro `CIOT_ERR_MEMORY_CHECK` in `ciot_err.h` to standardize and simplify memory allocation error checking throughout the codebase.

**Memory management and function refactoring:**

* Refactored the `ciot_save_cfg` function in `ciot.c` to allocate the `ciot_msg_t` structure dynamically using `calloc`, apply the new memory check macro, and ensure the allocated memory is freed after use. This reduces stack usage and improves robustness against memory allocation failures.