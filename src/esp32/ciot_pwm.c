/**
 * @file ciot_pwm.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-04-21
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "ciot_config.h"

#if CIOT_CONFIG_FEATURE_PWM == 1 && defined(CIOT_PLATFORM_ESP32)

#include <inttypes.h>
#include "soc/soc_caps.h"
#include "driver/ledc.h"
#include "driver/gpio.h"
#include "soc/gpio_sig_map.h"

#include "ciot_err.h"

#ifdef SOC_LEDC_SUPPORT_HS_MODE
#define LEDC_CHANNELS (SOC_LEDC_CHANNEL_NUM << 1)
#else
#define LEDC_CHANNELS (SOC_LEDC_CHANNEL_NUM)
#endif

// Use XTAL clock if possible to avoid timer frequency error when setting APB clock < 80 Mhz
// Need to be fixed in ESP-IDF
#ifdef SOC_LEDC_SUPPORT_XTAL_CLOCK
#define LEDC_DEFAULT_CLK LEDC_USE_XTAL_CLK
#else
#define LEDC_DEFAULT_CLK LEDC_AUTO_CLK
#endif

#define LEDC_MAX_BIT_WIDTH (20)

/*
 * LEDC Chan to Group/Channel/Timer Mapping
 ** ledc: 0  => Group: 0, Channel: 0, Timer: 0
 ** ledc: 1  => Group: 0, Channel: 1, Timer: 0
 ** ledc: 2  => Group: 0, Channel: 2, Timer: 1
 ** ledc: 3  => Group: 0, Channel: 3, Timer: 1
 ** ledc: 4  => Group: 0, Channel: 4, Timer: 2
 ** ledc: 5  => Group: 0, Channel: 5, Timer: 2
 ** ledc: 6  => Group: 0, Channel: 6, Timer: 3
 ** ledc: 7  => Group: 0, Channel: 7, Timer: 3
 ** ledc: 8  => Group: 1, Channel: 0, Timer: 0
 ** ledc: 9  => Group: 1, Channel: 1, Timer: 0
 ** ledc: 10 => Group: 1, Channel: 2, Timer: 1
 ** ledc: 11 => Group: 1, Channel: 3, Timer: 1
 ** ledc: 12 => Group: 1, Channel: 4, Timer: 2
 ** ledc: 13 => Group: 1, Channel: 5, Timer: 2
 ** ledc: 14 => Group: 1, Channel: 6, Timer: 3
 ** ledc: 15 => Group: 1, Channel: 7, Timer: 3
 */

typedef enum
{
    NOTE_C,
    NOTE_Cs,
    NOTE_D,
    NOTE_Eb,
    NOTE_E,
    NOTE_F,
    NOTE_Fs,
    NOTE_G,
    NOTE_Gs,
    NOTE_A,
    NOTE_Bb,
    NOTE_B,
    NOTE_MAX
} note_t;

static uint32_t ledc_setup(uint8_t chan, uint32_t freq, uint8_t bit_num);
static void ledc_write(uint8_t channel, uint32_t duty);
// static uint32_t ledc_write_tone(uint8_t channel, uint32_t freq);
// static uint32_t ledc_write_note(uint8_t channel, note_t note, uint8_t octave);
// static uint32_t ledc_read(uint8_t channel);
// static uint32_t ledc_read_freq(uint8_t channel);
static void ledc_attach_pin(uint8_t pin, uint8_t channel);
// static void ledc_detach_pin(uint8_t pin);
static uint32_t ledc_change_freq(uint8_t channel, uint32_t freq, uint8_t resolution_bits);
static int8_t analog_get_ch(uint8_t pin);

static uint8_t channels_resolution[LEDC_CHANNELS] = {0};
static int8_t pin_to_channel[SOC_GPIO_PIN_COUNT] = {0};
static int cnt_channel = LEDC_CHANNELS;
static uint8_t analog_resolution = 8;
static int analog_frequency = 1000;

static const char *TAG = "ciot_pwm";

ciot_err_t ciot_pwm_init(uint8_t pin)
{
    if (pin < SOC_GPIO_PIN_COUNT)
    {
        int8_t channel = -1;
        if (pin_to_channel[pin] == 0)
        {
            if (!cnt_channel)
            {
                CIOT_LOGE(TAG, "No more analog_write channels available! You can have maximum %d", LEDC_CHANNELS);
                return CIOT_ERR_INVALID_ARG;
            }
            cnt_channel--;
            channel = cnt_channel;
        }
        else
        {
            channel = analog_get_ch(pin);
        }
        if (ledc_setup(channel, analog_frequency, analog_resolution) == 0)
        {
            CIOT_LOGE(TAG, "analog_write setup failed (freq = %d, resolution = %d). Try setting different resolution or frequency", analog_frequency, analog_resolution);
            return CIOT_ERR_FAIL;
        }
        ledc_attach_pin(pin, channel);
        pin_to_channel[pin] = channel + 1;
    }
    return CIOT_ERR_OK;
}

ciot_err_t ciot_pwm_set_duty(uint8_t pin, uint16_t duty)
{
    int8_t channel = -1;
    if (pin_to_channel[pin] == 0)
    {
        if (!cnt_channel)
        {
            CIOT_LOGE(TAG, "No more analog_write channels available! You can have maximum %d", LEDC_CHANNELS);
            return CIOT_ERR_INVALID_ARG;
        }
        cnt_channel--;
        channel = cnt_channel;
    }
    else
    {
        channel = analog_get_ch(pin);
    }
    ledc_write(channel, duty);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_pwm_set_freq(uint32_t freq)
{
    if (cnt_channel != LEDC_CHANNELS)
    {
        for (int channel = LEDC_CHANNELS - 1; channel >= cnt_channel; channel--)
        {
            if (ledc_change_freq(channel, freq, analog_resolution) == 0)
            {
                CIOT_LOGE(TAG, "analog_write frequency cant be set due to selected resolution! Try to adjust resolution first");
                return CIOT_ERR_INVALID_ARG;
            }
        }
    }
    analog_frequency = freq;
    return CIOT_ERR_OK;
}

ciot_err_t ciot_pwm_set_resolution(uint8_t bits)
{
    if (bits > LEDC_MAX_BIT_WIDTH)
    {
        CIOT_LOGW(TAG, "analog_write resolution width too big! Setting to maximum %d bits)", LEDC_MAX_BIT_WIDTH);
        bits = LEDC_MAX_BIT_WIDTH;
    }
    if (cnt_channel != LEDC_CHANNELS)
    {
        for (int channel = LEDC_CHANNELS - 1; channel >= cnt_channel; channel--)
        {
            if (ledc_change_freq(channel, analog_frequency, bits) == 0)
            {
                CIOT_LOGE(TAG, "analog_write resolution cant be set due to selected frequency! Try to adjust frequency first");
                return CIOT_ERR_INVALID_ARG;
            }
        }
    }
    analog_resolution = bits;
    return CIOT_ERR_OK;
}

static uint32_t ledc_setup(uint8_t chan, uint32_t freq, uint8_t bit_num)
{
    if (chan >= LEDC_CHANNELS || bit_num > LEDC_MAX_BIT_WIDTH)
    {
        CIOT_LOGE(TAG, "No more LEDC channels available! (maximum %d) or bit width too big (maximum %d)", LEDC_CHANNELS, LEDC_MAX_BIT_WIDTH);
        return 0;
    }

    uint8_t group = (chan / 8), timer = ((chan / 2) % 4);

    ledc_timer_config_t ledc_timer = {
        .speed_mode = group,
        .timer_num = timer,
        .duty_resolution = bit_num,
        .freq_hz = freq,
        .clk_cfg = LEDC_DEFAULT_CLK};
    if (ledc_timer_config(&ledc_timer) != ESP_OK)
    {
        CIOT_LOGE(TAG, "ledc setup failed!");
        return 0;
    }
    channels_resolution[chan] = bit_num;
    return ledc_get_freq(group, timer);
}

static void ledc_write(uint8_t chan, uint32_t duty)
{
    if (chan >= LEDC_CHANNELS)
    {
        return;
    }
    uint8_t group = (chan / 8), channel = (chan % 8);

    // Fixing if all bits in resolution is set = LEDC FULL ON
    uint32_t max_duty = (1 << channels_resolution[chan]) - 1;

    if ((duty == max_duty) && (max_duty != 1))
    {
        duty = max_duty + 1;
    }

    ledc_set_duty(group, channel, duty);
    ledc_update_duty(group, channel);
}

// static uint32_t ledc_read(uint8_t chan)
// {
//     if (chan >= LEDC_CHANNELS)
//     {
//         return 0;
//     }
//     uint8_t group = (chan / 8), channel = (chan % 8);
//     return ledc_get_duty(group, channel);
// }

// static uint32_t ledc_read_freq(uint8_t chan)
// {
//     if (!ledc_read(chan))
//     {
//         return 0;
//     }
//     uint8_t group = (chan / 8), timer = ((chan / 2) % 4);
//     return ledc_get_freq(group, timer);
// }

// static uint32_t ledc_write_tone(uint8_t chan, uint32_t freq)
// {
//     if (chan >= LEDC_CHANNELS)
//     {
//         return 0;
//     }
//     if (!freq)
//     {
//         ledc_write(chan, 0);
//         return 0;
//     }

//     uint8_t group = (chan / 8), timer = ((chan / 2) % 4);

//     ledc_timer_config_t ledc_timer = {
//         .speed_mode = group,
//         .timer_num = timer,
//         .duty_resolution = 10,
//         .freq_hz = freq,
//         .clk_cfg = LEDC_DEFAULT_CLK};

//     if (ledc_timer_config(&ledc_timer) != ESP_OK)
//     {
//         CIOT_LOGE(TAG, "ledc_setup failed!");
//         return 0;
//     }
//     channels_resolution[chan] = 10;

//     uint32_t res_freq = ledc_get_freq(group, timer);
//     ledc_write(chan, 0x1FF);
//     return res_freq;
// }

// static uint32_t ledc_write_note(uint8_t chan, note_t note, uint8_t octave)
// {
//     const uint16_t noteFrequencyBase[12] = {
//         //   C        C#       D        Eb       E        F       F#        G       G#        A       Bb        B
//         4186, 4435, 4699, 4978, 5274, 5588, 5920, 6272, 6645, 7040, 7459, 7902};

//     if (octave > 8 || note >= NOTE_MAX)
//     {
//         return 0;
//     }
//     uint32_t noteFreq = (uint32_t)noteFrequencyBase[note] / (uint32_t)(1 << (8 - octave));
//     return ledc_write_tone(chan, noteFreq);
// }

static void ledc_attach_pin(uint8_t pin, uint8_t chan)
{
    if (chan >= LEDC_CHANNELS)
    {
        return;
    }
    uint8_t group = (chan / 8), channel = (chan % 8), timer = ((chan / 2) % 4);
    uint32_t duty = ledc_get_duty(group, channel);

    ledc_channel_config_t ledc_channel = {
        .speed_mode = group,
        .channel = channel,
        .timer_sel = timer,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = pin,
        .duty = duty,
        .hpoint = 0};
    ledc_channel_config(&ledc_channel);
}

// static void ledc_detach_pin(uint8_t pin)
// {
//     gpio_matrix_out(pin, SIG_GPIO_OUT_IDX, false, false);
// }

static uint32_t ledc_change_freq(uint8_t chan, uint32_t freq, uint8_t bit_num)
{
    if (chan >= LEDC_CHANNELS || bit_num > LEDC_MAX_BIT_WIDTH)
    {
        CIOT_LOGE(TAG, "LEDC channel not available! (maximum %u) or bit width too big (maximum %u)", LEDC_CHANNELS, LEDC_MAX_BIT_WIDTH);
        return 0;
    }
    uint8_t group = (chan / 8), timer = ((chan / 2) % 4);

    ledc_timer_config_t ledc_timer = {
        .speed_mode = group,
        .timer_num = timer,
        .duty_resolution = bit_num,
        .freq_hz = freq,
        .clk_cfg = LEDC_DEFAULT_CLK};

    if (ledc_timer_config(&ledc_timer) != ESP_OK)
    {
        CIOT_LOGE(TAG, "ledc_change_freq failed!");
        return 0;
    }
    channels_resolution[chan] = bit_num;
    return ledc_get_freq(group, timer);
}

// static void analog_write(uint8_t pin, int value)
// {
//     // Use ledc hardware for internal pins
//     if (pin < SOC_GPIO_PIN_COUNT)
//     {
//         int8_t channel = -1;
//         if (pin_to_channel[pin] == 0)
//         {
//             if (!cnt_channel)
//             {
//                 CIOT_LOGE(TAG, "No more analog_write channels available! You can have maximum %u", LEDC_CHANNELS);
//                 return;
//             }
//             cnt_channel--;
//             channel = cnt_channel;
//         }
//         else
//         {
//             channel = analog_get_ch(pin);
//         }
//         CIOT_LOGV(TAG, "GPIO %d - Using Channel %d, Value = %d", pin, channel, value);
//         if (ledc_setup(channel, analog_frequency, analog_resolution) == 0)
//         {
//             CIOT_LOGE(TAG, "analog_write setup failed (freq = %u, resolution = %u). Try setting different resolution or frequency");
//             return;
//         }
//         ledc_attach_pin(pin, channel);
//         pin_to_channel[pin] = channel + 1;
//         ledc_write(channel, value);
//     }
// }

static int8_t analog_get_ch(uint8_t pin)
{
    return pin_to_channel[pin] - 1;
}

#endif //! CIOT_CONFIG_FEATURE_PWM == 1