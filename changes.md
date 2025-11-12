# ciot_c Release

### Changes

**GPIO API enhancements:**

* Added `ciot_gpio_write_pin` and `ciot_gpio_read_pin` functions to the public API in `ciot_gpio.h`, allowing direct writing and reading of individual pin states.
* Implemented the new `ciot_gpio_write_pin` and `ciot_gpio_read_pin` functions in `ciot_gpio_base.c`, enabling the actual hardware interaction for these API calls.

**Status update improvements:**

* Updated the GPIO status retrieval logic to refresh the state of all input pins before returning status data, ensuring the status structure accurately reflects the current hardware state.