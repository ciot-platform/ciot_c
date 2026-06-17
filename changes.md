# ciot_c Release

### Changes

**Modbus client status tracking improvements:**

* Added `requests_success` and `requests_error` fields to the `ciot_mbus_client_status_t` struct to track the number of successful and failed requests. Updated related macros, tags, and field lists in `mbus_client.pb.h` to support these new fields. [[1]](diffhunk://#diff-6bf44a776dd28cd8ec06193fb80a558c71c3f9214b21e0596abb4a124d546fd1R56-R57) [[2]](diffhunk://#diff-6bf44a776dd28cd8ec06193fb80a558c71c3f9214b21e0596abb4a124d546fd1L105-R114) [[3]](diffhunk://#diff-6bf44a776dd28cd8ec06193fb80a558c71c3f9214b21e0596abb4a124d546fd1R128-R129) [[4]](diffhunk://#diff-6bf44a776dd28cd8ec06193fb80a558c71c3f9214b21e0596abb4a124d546fd1L162-R168) [[5]](diffhunk://#diff-6bf44a776dd28cd8ec06193fb80a558c71c3f9214b21e0596abb4a124d546fd1L208-R214)
* Updated the default and zero-initialization macros for `ciot_mbus_client_status_t` to initialize the new counters to zero.

**Modbus client API and internal logic changes:**

* Added a new function, `ciot_mbus_client_get_status`, to retrieve the current Modbus client status, including the new request counters.
* Refactored error handling in `ciot_mbus_client.c` by introducing `ciot_mbus_client_process_request_result`, which updates the request counters and status error code after each Modbus operation. All Modbus request functions now use this helper for consistent status updates.