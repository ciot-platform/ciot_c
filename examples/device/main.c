/**
 * @file main.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-12-01
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "main.h"
#include "ciot_storage_nvs.h"

device_t self;

ciot_msg_data_t sys_cfg = {};

ciot_msg_data_t uart_cfg = {
    .which_type = CIOT_MSG_DATA_UART_TAG,
    .uart = {
        .which_type = CIOT_UART_DATA_CONFIG_TAG,
        .config = {
            .baud_rate = 115200,
            .num = 2,
            .gpio.rx = 16,
            .gpio.tx = 17,
        },
    },
};

ciot_msg_data_t wifi_ap_cfg = {
    .which_type = CIOT_MSG_DATA_WIFI_TAG,
    .wifi = {
        .which_type = CIOT_WIFI_DATA_CONFIG_TAG,
        .config = {
            .type = CIOT_WIFI_TYPE_AP,
            .ssid = "CIOT ESP32 AP",
            .password = "admin123",
        },
    },
};

ciot_msg_data_t wifi_sta_cfg = {
    .which_type = CIOT_MSG_DATA_WIFI_TAG,
    .wifi = {
        .which_type = CIOT_WIFI_DATA_CONFIG_TAG,
        .config = {
            .type = CIOT_WIFI_TYPE_STA,
            .ssid = "CABO CANAVERAL 2G",
            .password = "16192020",
        },
    },
};

ciot_msg_data_t http_server_cfg = {
    .which_type = CIOT_MSG_DATA_HTTP_SERVER_TAG,
    .http_server = {
        .which_type = CIOT_HTTP_SERVER_DATA_CONFIG_TAG,
        .config = {
            .address = "http://127.0.0.1",
            .route = "/v1/device",
            .port = 5050,
        },
    },
};

ciot_msg_data_t mqtt_client_cfg = {
    .which_type = CIOT_MSG_DATA_MQTT_CLIENT_TAG,
    .mqtt_client = {
        .which_type = CIOT_MQTT_CLIENT_DATA_CONFIG_TAG,
        .config = {
            .client_id = "ciot/device/256848",
            .url = "mqtt://test.mosquitto.org:1883",
            .has_topics = true,
            .topics = {
                .pub = "ciot/device/pub",
                .sub = "ciot/device/sub",
            },
        },
    },
};

ciot_msg_data_t ntp_cfg = {
    .which_type = CIOT_MSG_DATA_NTP_TAG,
    .ntp = {
        .which_type = CIOT_NTP_DATA_CONFIG_TAG,
        .config = {
            .server1 = "pool.ntp.org",
            .server2 = "time.google.com",
            .server3 = "gps.ntp.br",
        },
    },
};

ciot_msg_data_t gpio_cfg = {
    .which_type = CIOT_MSG_DATA_GPIO_TAG,
    .gpio = {
        .which_type = CIOT_GPIO_DATA_CONFIG_TAG,
        .config = {
            .pins_count = 2,
            .pins = {
                {
                    .num = 0,
                    .mode = CIOT_GPIO_MODE_INPUT
                },
                {
                    .num = 2,
                    .mode = CIOT_GPIO_MODE_OUTPUT,
                },
            },
        },
    },
};

static const char *TAG = "main";

static void device_start()
{
    // self.ifaces.storage = ciot_storage_nvs_new();

    self.ifaces.ciot = ciot_new();
    self.ifaces.list[DEVICE_IFACE_ID_CIOT] = (ciot_iface_t *)self.ifaces.ciot;
    self.ifaces.cfgs[DEVICE_IFACE_ID_CIOT] = NULL;

    self.ifaces.sys = ciot_sys_new(CIOT_HANDLE);
    self.ifaces.list[DEVICE_IFACE_ID_SYS] = (ciot_iface_t *)self.ifaces.sys;
    self.ifaces.cfgs[DEVICE_IFACE_ID_SYS] = NULL;

    CIOT_LOGI(TAG, "1 sys cfg %p", self.ifaces.cfgs[DEVICE_IFACE_ID_SYS]);

    self.ifaces.uart = ciot_uart_new(CIOT_HANDLE);
    self.ifaces.list[DEVICE_IFACE_ID_UART] = (ciot_iface_t *)self.ifaces.uart;
    self.ifaces.cfgs[DEVICE_IFACE_ID_UART] = &uart_cfg;

    // self.ifaces.wifi_ap = ciot_wifi_new(CIOT_WIFI_TYPE_AP);
    // self.ifaces.list[DEVICE_IFACE_ID_WIFI_AP] = (ciot_iface_t *)self.ifaces.wifi_ap;
    // self.ifaces.cfgs[DEVICE_IFACE_ID_WIFI_AP] = &wifi_ap_cfg;

    self.ifaces.wifi_sta = ciot_wifi_new(CIOT_WIFI_TYPE_STA);
    self.ifaces.list[DEVICE_IFACE_ID_WIFI_STA] = (ciot_iface_t *)self.ifaces.wifi_sta;
    self.ifaces.cfgs[DEVICE_IFACE_ID_WIFI_STA] = &wifi_sta_cfg;

    self.ifaces.http_server = ciot_http_server_new(CIOT_HANDLE);
    self.ifaces.list[DEVICE_IFACE_ID_HTTP_SERVER] = (ciot_iface_t *)self.ifaces.http_server;
    self.ifaces.cfgs[DEVICE_IFACE_ID_HTTP_SERVER] = &http_server_cfg;

    self.ifaces.mqtt_client = ciot_mqtt_client_new(CIOT_HANDLE);
    self.ifaces.list[DEVICE_IFACE_ID_MQTT_CLIENT] = (ciot_iface_t *)self.ifaces.mqtt_client;
    self.ifaces.cfgs[DEVICE_IFACE_ID_MQTT_CLIENT] = &mqtt_client_cfg;

    self.ifaces.ntp = ciot_ntp_new(CIOT_HANDLE);
    self.ifaces.list[DEVICE_IFACE_ID_NTP] = (ciot_iface_t *)self.ifaces.ntp;
    self.ifaces.cfgs[DEVICE_IFACE_ID_NTP] = &ntp_cfg;

    self.ifaces.gpio = ciot_gpio_new(CIOT_HANDLE);
    self.ifaces.list[DEVICE_IFACE_ID_GPIO] = (ciot_iface_t *)self.ifaces.gpio;
    self.ifaces.cfgs[DEVICE_IFACE_ID_GPIO] = &gpio_cfg;

    self.decoder.handle = ciot_decoder_s_new(self.decoder.buf, sizeof(self.decoder.buf));
    ciot_iface_set_decoder((ciot_iface_t *)self.ifaces.uart, self.decoder.handle);

    ciot_cfg_t ciot_cfg = {
        .ifaces = {
            .list = self.ifaces.list,
            .cfgs = self.ifaces.cfgs,
            .count = DEVICE_IFACE_ID_COUNT,
        },
    };
    ciot_start(self.ifaces.ciot, &ciot_cfg);
}

static void device_task()
{
    ciot_task(self.ifaces.ciot);
    ciot_sys_task(self.ifaces.sys);
    ciot_uart_task(self.ifaces.uart);
}

int main(void)
{
    device_start();

    while (true)
    {
        device_task();
    }

    return 0;
}
