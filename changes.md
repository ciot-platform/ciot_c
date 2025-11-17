# ciot_c Release

### Changes

**Timeout Configuration Improvements**

* Added new macros `CIOT_CONFIG_START_TIMEOUT_SECS` and `CIOT_CONFIG_BUSY_TIMEOUT_SECS` to `ciot_config.h` to allow configuration of interface start and busy state timeouts.
* Refactored code in `ciot.c` to use these macros instead of hardcoded timeout values, ensuring consistency and easier configuration. [[1]](diffhunk://#diff-ac3505c8e7850d45dfcfdd990afb140a041683f93b640fc2b17a8e05bdd30c10L19-R25) [[2]](diffhunk://#diff-ac3505c8e7850d45dfcfdd990afb140a041683f93b640fc2b17a8e05bdd30c10L265-R273) [[3]](diffhunk://#diff-ac3505c8e7850d45dfcfdd990afb140a041683f93b640fc2b17a8e05bdd30c10L311-R319)

**Busy State Timeout Handling**

* Introduced a new `timeout_timer` field to `ciot_receiver_t` in `ciot.h`, and added logic to reset this timer on start/stop of CIOT. [[1]](diffhunk://#diff-e3a87c483eaad2e3f831ea32fa5bb0c2538d10b268422ac8bf3e6da43110f554R78) [[2]](diffhunk://#diff-ac3505c8e7850d45dfcfdd990afb140a041683f93b640fc2b17a8e05bdd30c10R68) [[3]](diffhunk://#diff-ac3505c8e7850d45dfcfdd990afb140a041683f93b640fc2b17a8e05bdd30c10R78)
* Implemented timeout checking in the `ciot_busy_task` function; if the busy state exceeds the configured timeout, the state transitions to `CIOT_STATE_STARTED` and returns a timeout error.
* Updated event handling logic to set the busy timeout timer whenever entering the busy state, ensuring that all busy transitions are properly timed.

API improvements:

* Replaced the `ciot_mqtt_client_is_connected` function with `ciot_mqtt_client_get_state`, which returns the client's full state instead of just a boolean, in both `include/ciot_mqtt_client.h` and `src/common/ciot_mqtt_client_base.c`. [[1]](diffhunk://#diff-a638c8162c9fd3c46a613972d55fb3e86f31fb500834c9975f267f2e88874652L64-R64) [[2]](diffhunk://#diff-3c29355ff0857480c7a57573c88ad3ec552a012ff62443b7bcc7fd8537b3efb9L101-R113)