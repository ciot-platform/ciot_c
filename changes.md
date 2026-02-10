# ciot_c Release

### Changes

**GPIO Module Enhancements:**

* Added a new function `ciot_gpio_set_blink_interval` to allow setting the blink interval for GPIO pins, including its implementation and header declaration. (`include/ciot_gpio.h`, `src/common/ciot_gpio_base.c`) [[1]](diffhunk://#diff-b86b342071e52eb107559f17e2c57e0cdb9d07d681ff2dd3992d9a98cfdd420bR49) [[2]](diffhunk://#diff-ca74fefe242cdfc7f78a04c8c2ed8bf09d18045410717c57b75e73b859865d36R226-R239)
* Updated `ciot_gpio_get_state` to use the new `CIOT_ERR_UID_CHECK` macro for improved bounds checking on pin IDs. (`src/common/ciot_gpio_base.c`)

**Error Handling Improvements:**

* Introduced two new error-checking macros: `CIOT_ERR_UID_CHECK` and `CIOT_ERR_UEXISTENCE_CHECK` for more consistent and readable bounds/error checking throughout the codebase. (`include/ciot_err.h`) [[1]](diffhunk://#diff-591e79e9e18aae7e803dda8a44c81c344fca0e7a18acadaa782aa99c84c78e3aR70-R73) [[2]](diffhunk://#diff-591e79e9e18aae7e803dda8a44c81c344fca0e7a18acadaa782aa99c84c78e3aR86-R89)

**Nanopb Library Updates:**

* Updated the Nanopb version macro to `nanopb-0.4.9.1` for version tracking. (`src/nanopb/pb.h`)
* Fixed a bug in `pb_decode_ex` by ensuring the `status` variable is set to `false` on substream close failure, improving error propagation. (`src/nanopb/pb_decode.c`)

**Minor/Cleanup Changes:**

* Simplified the inclusion of the Nanomodbus header. (`include/ciot_mbus.h`)
* Minor whitespace cleanup in Nanopb source/header files. (`src/nanopb/pb_common.c`, `src/nanopb/pb_common.h`) [[1]](diffhunk://#diff-5d2ca72bb275bfaee8f609245e580b5a7794f8e3891fe0ba0ff5c870968da78aL388) [[2]](diffhunk://#diff-30ac05be7edbd02b6d0b7bd2f7fe246fb74cbdca0ecff86ddc7027e5a983c167L49)