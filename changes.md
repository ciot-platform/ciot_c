# ciot_c Release

### Changes

**Core event handling and queueing:**

* Replaced the previous `ciot_receiver` structure with a new event queue system (`ciot_event_queue_t` and `ciot_event_slot_t`), enabling buffered event processing and improved concurrency. The event queue now supports configurable size and tracks dropped events. (`[[1]](diffhunk://#diff-e3a87c483eaad2e3f831ea32fa5bb0c2538d10b268422ac8bf3e6da43110f554L73-R97)`, `[[2]](diffhunk://#diff-bebf3b3742ecc13fb8bff23dc362b537e593146621f52ab115c0c492d7459c74R73-R76)`, `[[3]](diffhunk://#diff-ac3505c8e7850d45dfcfdd990afb140a041683f93b640fc2b17a8e05bdd30c10R27-R122)`, `[[4]](diffhunk://#diff-ac3505c8e7850d45dfcfdd990afb140a041683f93b640fc2b17a8e05bdd30c10L263-R383)`, `[[5]](diffhunk://#diff-ac3505c8e7850d45dfcfdd990afb140a041683f93b640fc2b17a8e05bdd30c10L361-L377)`)
* Updated `ciot_busy_task` and `ciot_starting_task` to process events from the queue, improving robustness and handling of unexpected or out-of-order events. (`[[1]](diffhunk://#diff-ac3505c8e7850d45dfcfdd990afb140a041683f93b640fc2b17a8e05bdd30c10L263-R383)`, `[[2]](diffhunk://#diff-ac3505c8e7850d45dfcfdd990afb140a041683f93b640fc2b17a8e05bdd30c10L361-L377)`, `[[3]](diffhunk://#diff-ac3505c8e7850d45dfcfdd990afb140a041683f93b640fc2b17a8e05bdd30c10L397-L410)`, `[[4]](diffhunk://#diff-ac3505c8e7850d45dfcfdd990afb140a041683f93b640fc2b17a8e05bdd30c10L462-R572)`, `[[5]](diffhunk://#diff-ac3505c8e7850d45dfcfdd990afb140a041683f93b640fc2b17a8e05bdd30c10L482)`, `[[6]](diffhunk://#diff-ac3505c8e7850d45dfcfdd990afb140a041683f93b640fc2b17a8e05bdd30c10L493-L504)`)
* Introduced helper functions for queue operations, such as reserving, committing, pushing, and popping events, and handling raw event data deserialization. (`[src/core/ciot.cR27-R122](diffhunk://#diff-ac3505c8e7850d45dfcfdd990afb140a041683f93b640fc2b17a8e05bdd30c10R27-R122)`)

**Wi-Fi multi-network support:**

* Added new header `ciot_wifi_multi.h` with APIs, types, and structures to support managing multiple Wi-Fi networks, including switching, invalidation, and status tracking. (`[[1]](diffhunk://#diff-6cf31a1232ed8b6b5dc2edbe6c329bf3486d16f2ee281fdf267d20c99e339a1dR1-R68)`, `[[2]](diffhunk://#diff-bfc9182acb9823e60afe0cc6ba5b04a80381b18b4384ccb0c22782f52f857897R41)`)
* Integrated new protobuf definitions for Wi-Fi multi-network (`wifi_multi.pb.h`). (`[include/ciot_types.hR41](diffhunk://#diff-bfc9182acb9823e60afe0cc6ba5b04a80381b18b4384ccb0c22782f52f857897R41)`)

**BLE scanner and interface API updates:**

* Changed BLE scanner event handling to use a specific event type (`ciot_ble_scn_event_adv_report_t`) and updated related function signatures for clarity and correctness. (`[[1]](diffhunk://#diff-9cc3ff9f4c60d80c2855302798490b054d719bbcb6706bb5179002e28506ccbaL76-R76)`, `[[2]](diffhunk://#diff-1d56b9dfd453aa5d1584b74b104467528a195ea593194e1b200838f769b93ea2L139-R142)`, `[[3]](diffhunk://#diff-1d56b9dfd453aa5d1584b74b104467528a195ea593194e1b200838f769b93ea2L162-R161)`, `[[4]](diffhunk://#diff-1d56b9dfd453aa5d1584b74b104467528a195ea593194e1b200838f769b93ea2L195-R194)`)
* Added `ciot_iface_send_event_data` function to the interface API, allowing direct sending of event data buffers. (`[include/ciot_iface.hR53](diffhunk://#diff-d81cd7438a00c22a4f450ac673da1c4398d739083d28a53d8b1a6bfde11f7558R53)`)

**Miscellaneous improvements:**

* Bumped the CIOT version from `0.21.0.4` to `0.22.0.0`. (`[include/ciot.hL36-R37](diffhunk://#diff-e3a87c483eaad2e3f831ea32fa5bb0c2538d10b268422ac8bf3e6da43110f554L36-R37)`)
* Added `ciot_sys_log_run_time_stats` for system runtime statistics logging. (`[include/ciot_sys.hR50-R51](diffhunk://#diff-cede3fd5e0673989c303211b2c499187cf13932ea5c130e71efd982aae0406faR50-R51)`)
* Included `ciot_config.h` in `ciot.h` to ensure configuration macros are available. (`[include/ciot.hR15](diffhunk://#diff-e3a87c483eaad2e3f831ea32fa5bb0c2538d10b268422ac8bf3e6da43110f554R15)`)