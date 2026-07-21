# ciot_c Release

### Changes

**Enhancements to GPIO blinking functionality:**

* Added two new GPIO states: `CIOT_GPIO_STATE_BLINKING_REVERSE` (alternates state in reverse phase) and `CIOT_GPIO_STATE_BLINKING_SLOW` (alternates state at half speed), and updated the enum helpers accordingly. [[1]](diffhunk://#diff-ef3b99b04bb0fd99e2e8aac7e8d21ff36127ce6229ac4352527520c64c57cd56L20-R22) [[2]](diffhunk://#diff-ef3b99b04bb0fd99e2e8aac7e8d21ff36127ce6229ac4352527520c64c57cd56L97-R108)

**Core logic updates:**

* Modified the `ciot_gpio_base_t` struct to include a new `blink_tick` field for tracking blink phases.
* Updated the `ciot_gpio_task` function to generate blink signals based on `blink_tick`, supporting the new reverse and slow blinking modes.
* Enhanced the GPIO state handling to recognize and enable the new blinking modes when setting state. [[1]](diffhunk://#diff-ca74fefe242cdfc7f78a04c8c2ed8bf09d18045410717c57b75e73b859865d36R81-R92) [[2]](diffhunk://#diff-ca74fefe242cdfc7f78a04c8c2ed8bf09d18045410717c57b75e73b859865d36L213-R230)