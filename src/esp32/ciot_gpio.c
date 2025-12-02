/**
 * @file ciot_gpio.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-06-07
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "ciot_config.h"

#if CIOT_CONFIG_FEATURE_GPIO == 1 && defined(CIOT_PLATFORM_ESP32)

#include <stdlib.h>
#include "ciot_gpio.h"
#include "ciot_err.h"
#include "driver/gpio.h"

static const char *TAG = "ciot_gpio";

struct ciot_gpio
{
    ciot_gpio_base_t base;
};

ciot_gpio_t ciot_gpio_new(void *handle)
{
    ciot_gpio_t self = calloc(1, sizeof(struct ciot_gpio));
    ciot_gpio_init(self);
    self->base.set_state = (ciot_hal_gpio_set_state_fn*)gpio_set_level;
    self->base.get_state = gpio_get_level;
    return self;
}

ciot_err_t ciot_gpio_start(ciot_gpio_t self, ciot_gpio_cfg_t *cfg)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(cfg);

    CIOT_ERR_RETURN(ciot_gpio_set_cfg(self, cfg));

    for (size_t i = 0; i < cfg->pins_count; i++)
    {
        gpio_config_t config = {
            .pin_bit_mask = (1LL << cfg->pins[i].num),
            .mode = cfg->pins[i].mode,
            .pull_down_en = cfg->pins[i].pull == CIOT_GPIO_PULL_DOWN,
            .pull_up_en = cfg->pins[i].pull == CIOT_GPIO_PULL_UP,
        };
        esp_err_t err = gpio_config(&config);
        if(err != ESP_OK)
        {
            CIOT_LOGE(TAG, "config gpio id:%d pin:%d error: %s", i, (int)cfg->pins[i].num, esp_err_to_name(err));
            self->base.status.states.bytes[i] = CIOT_GPIO_STATE_ERROR;
        }
    }

    ciot_iface_send_event_type(&self->base.iface, CIOT_EVENT_TYPE_STARTED);

    return CIOT_ERR_OK;
}

ciot_err_t ciot_gpio_stop(ciot_gpio_t self)
{
    CIOT_ERR_NULL_CHECK(self);
    return CIOT_ERR_NOT_IMPLEMENTED;
}

#endif  //!CIOT_CONFIG_FEATURE_GPIO == 1