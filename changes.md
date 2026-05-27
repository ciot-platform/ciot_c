# ciot_c Release

### Changes

**Event Handling Refactor:**

* Removed the `ciot_event_queue_t` structure and all associated event queue logic, including event slot management and queue push/pop functions. Events are now stored directly in the `ciot_receiver_t` struct, which holds only the latest event and its sender. (`include/ciot.h` [[1]](diffhunk://#diff-e3a87c483eaad2e3f831ea32fa5bb0c2538d10b268422ac8bf3e6da43110f554L74-R81) `src/core/ciot.c` [[2]](diffhunk://#diff-ac3505c8e7850d45dfcfdd990afb140a041683f93b640fc2b17a8e05bdd30c10L27-L122) [[3]](diffhunk://#diff-ac3505c8e7850d45dfcfdd990afb140a041683f93b640fc2b17a8e05bdd30c10L352-L383) [[4]](diffhunk://#diff-ac3505c8e7850d45dfcfdd990afb140a041683f93b640fc2b17a8e05bdd30c10L470-R377) [[5]](diffhunk://#diff-ac3505c8e7850d45dfcfdd990afb140a041683f93b640fc2b17a8e05bdd30c10L683-R662)
* Updated the event processing flow in `ciot_busy_task`, `ciot_starting_task`, and `ciot_iface_event_handler` to work with the new receiver-based event storage instead of queue-based logic. (`src/core/ciot.c` [[1]](diffhunk://#diff-ac3505c8e7850d45dfcfdd990afb140a041683f93b640fc2b17a8e05bdd30c10L352-L383) [[2]](diffhunk://#diff-ac3505c8e7850d45dfcfdd990afb140a041683f93b640fc2b17a8e05bdd30c10L470-R377) [[3]](diffhunk://#diff-ac3505c8e7850d45dfcfdd990afb140a041683f93b640fc2b17a8e05bdd30c10L683-R662)

**Configuration Management Improvements:**

* Replaced the hardcoded `CIOT_IFACE_CFG_FILENAME` macro with a configurable `CIOT_CONFIG_IFACE_CFG_FILENAME_MASK`, allowing the filename pattern to be overridden as needed. Updated all usages in the codebase. (`include/ciot.h` [[1]](diffhunk://#diff-e3a87c483eaad2e3f831ea32fa5bb0c2538d10b268422ac8bf3e6da43110f554L37-R40) `src/core/ciot.c` [[2]](diffhunk://#diff-ac3505c8e7850d45dfcfdd990afb140a041683f93b640fc2b17a8e05bdd30c10L226-R137) [[3]](diffhunk://#diff-ac3505c8e7850d45dfcfdd990afb140a041683f93b640fc2b17a8e05bdd30c10L236-R147) [[4]](diffhunk://#diff-ac3505c8e7850d45dfcfdd990afb140a041683f93b640fc2b17a8e05bdd30c10L256-R167) [[5]](diffhunk://#diff-ac3505c8e7850d45dfcfdd990afb140a041683f93b640fc2b17a8e05bdd30c10L282-R193) [[6]](diffhunk://#diff-ac3505c8e7850d45dfcfdd990afb140a041683f93b640fc2b17a8e05bdd30c10L301-R212) [[7]](diffhunk://#diff-ac3505c8e7850d45dfcfdd990afb140a041683f93b640fc2b17a8e05bdd30c10L533-R424)

**Code Cleanup and Minor Changes:**

* Removed the now-unused `ciot_event_queue_t`, `ciot_event_slot_t`, and related macros and function declarations from headers and source files. (`include/ciot.h` [[1]](diffhunk://#diff-e3a87c483eaad2e3f831ea32fa5bb0c2538d10b268422ac8bf3e6da43110f554L74-R81) `src/core/ciot.c` [[2]](diffhunk://#diff-ac3505c8e7850d45dfcfdd990afb140a041683f93b640fc2b17a8e05bdd30c10L27-L122)
* Updated version macro to `0,21,2,0`. (`include/ciot.h` [include/ciot.hL37-R40](diffhunk://#diff-e3a87c483eaad2e3f831ea32fa5bb0c2538d10b268422ac8bf3e6da43110f554L37-R40))