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
#include "sparkplug-b/proto/v1/sparkplug_b.pb.h"

device_t self;

ciot_msg_data_t sys_cfg = {};

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
            .has_session = true,
            .session = {
                .clean_session = false,
                .keep_alive = 60,
            },
            .has_last_will = true,
            .last_will = {
                .topic = "spBv1.0/1/NDEATH/1/256848",
                .payload = {},
                .qos = 1,
                .retain = false,
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

static const char *TAG = "main";

static void device_start();
static void device_task();
static void device_health_task();
static ciot_err_t device_event_handler(ciot_iface_t *sender, ciot_event_t *event, void *args);

static void device_start()
{
    // self.ifaces.storage = ciot_storage_nvs_new();
    // self.serializer = ciot_serializer_pb_new();

    self.ifaces.ciot = ciot_new();
    self.ifaces.list[DEVICE_IFACE_ID_CIOT] = (ciot_iface_t *)self.ifaces.ciot;
    self.ifaces.cfgs[DEVICE_IFACE_ID_CIOT] = NULL;

    self.ifaces.sys = ciot_sys_new(CIOT_HANDLE);
    self.ifaces.list[DEVICE_IFACE_ID_SYS] = (ciot_iface_t *)self.ifaces.sys;
    self.ifaces.cfgs[DEVICE_IFACE_ID_SYS] = NULL;

    self.ifaces.mqtt_client = ciot_mqtt_client_new(CIOT_HANDLE);
    self.ifaces.list[DEVICE_IFACE_ID_MQTT_CLIENT] = (ciot_iface_t *)self.ifaces.mqtt_client;
    self.ifaces.cfgs[DEVICE_IFACE_ID_MQTT_CLIENT] = &mqtt_client_cfg;

    self.ifaces.ntp = ciot_ntp_new(CIOT_HANDLE);
    self.ifaces.list[DEVICE_IFACE_ID_NTP] = (ciot_iface_t *)self.ifaces.ntp;
    self.ifaces.cfgs[DEVICE_IFACE_ID_NTP] = &ntp_cfg;

    ciot_cfg_t ciot_cfg = {
        .ifaces = {
            .list = self.ifaces.list,
            .cfgs = self.ifaces.cfgs,
            .count = DEVICE_IFACE_ID_COUNT,
        },
    };
    ciot_start(self.ifaces.ciot, &ciot_cfg);

    ciot_iface_set_event_handler((ciot_iface_t *)self.ifaces.ciot, device_event_handler, NULL);
}

static void device_task()
{
    ciot_task(self.ifaces.ciot);
    ciot_sys_task(self.ifaces.sys);
    device_health_task();
}

// New helper: gera payload, serializa e escreve no buffer.
// Retorna número de bytes escritos no buf, ou < 0 em erro.
static int build_and_serialize_payload(uint8_t *buf, size_t buf_len, int *count)
{
    if (!buf || !count) return -1;

    sparkplugb_payload_t payload = {0};
    payload.has_timestamp = true;
    payload.timestamp = (uint64_t)time(NULL);
    payload.metrics_count = 2;

    // metric 0: counter (int)
    payload.metrics[0].has_name = true;
    snprintf(payload.metrics[0].name, sizeof(payload.metrics[0].name), "counter");
    payload.metrics[0].which_value = SPARKPLUGB_PAYLOAD_METRIC_INT_VALUE_TAG;
    payload.metrics[0].has_datatype = true;
    payload.metrics[0].datatype = 3; // int32
    payload.metrics[0].int_value = *count; // assign current value
    // increment count to keep same behavior as original count++
    (*count)++;
    payload.metrics[0].has_properties = true;
    payload.metrics[0].properties.keys_count = 1;
    payload.metrics[0].properties.values_count = 1;
    snprintf(payload.metrics[0].properties.keys[0], sizeof(payload.metrics[0].properties.keys[0]), "unit");
    payload.metrics[0].properties.values[0].which_value = SPARKPLUGB_PAYLOAD_PROPERTY_VALUE_STRING_VALUE_TAG;
    snprintf(payload.metrics[0].properties.values[0].string_value, sizeof(payload.metrics[0].properties.values[0].string_value), "number");

    // metric 1: temperature (float) - uses incremented count like original
    payload.metrics[1].has_name = true;
    snprintf(payload.metrics[1].name, sizeof(payload.metrics[1].name), "temperature");
    payload.metrics[1].has_datatype = true;
    payload.metrics[1].datatype = 9; // float
    payload.metrics[1].which_value = SPARKPLUGB_PAYLOAD_METRIC_FLOAT_VALUE_TAG;
    payload.metrics[1].float_value = 25.0f + (float)(*count % 10);
    payload.metrics[1].has_properties = true;
    payload.metrics[1].properties.keys_count = 1;
    payload.metrics[1].properties.values_count = 1;
    snprintf(payload.metrics[1].properties.keys[0], sizeof(payload.metrics[1].properties.keys[0]), "unit");
    payload.metrics[1].properties.values[0].which_value = SPARKPLUGB_PAYLOAD_PROPERTY_VALUE_STRING_VALUE_TAG;
    snprintf(payload.metrics[1].properties.values[0].string_value, sizeof(payload.metrics[1].properties.values[0].string_value), "celsius");

    return ciot_serializer_pb_to_bytes(buf, buf_len, &payload, &sparkplugb_payload_t_msg);
}

// Helper: monta um NBIRTH simples (node-level) e serializa no buffer.
// Retorna número de bytes escritos no buf, ou < 0 em erro.
static int build_and_serialize_nbirth(uint8_t *buf, size_t buf_len)
{
    if (!buf) return -1;

    sparkplugb_payload_t payload = {0};
    payload.has_timestamp = true;
    payload.timestamp = (uint64_t)time(NULL);
    payload.metrics_count = 1;

    // simple node metric: bdseq (birth/death sequence)
    payload.metrics[0].has_name = true;
    snprintf(payload.metrics[0].name, sizeof(payload.metrics[0].name), "bdseq");
    payload.metrics[0].which_value = SPARKPLUGB_PAYLOAD_METRIC_INT_VALUE_TAG;
    payload.metrics[0].has_datatype = true;
    payload.metrics[0].datatype = 3; // int32
    payload.metrics[0].int_value = 0;

    return ciot_serializer_pb_to_bytes(buf, buf_len, &payload, &sparkplugb_payload_t_msg);
}

static void device_health_task()
{
    if (time(NULL) > self.health_timer + 5 && ciot_mqtt_client_get_state(self.ifaces.mqtt_client) == CIOT_MQTT_CLIENT_STATE_CONNECTED)
    {
        self.health_timer = time(NULL);
        static int count = 0;

        uint8_t buf[512];
        int size = build_and_serialize_payload(buf, sizeof(buf), &count);
        if (size > 0)
        {
            ciot_mqtt_client_pub(self.ifaces.mqtt_client, "spBv1.0/1/DDATA/1/256848", buf, size, 0);
        }

        CIOT_LOGI(TAG, "Published health message");
    }
}

static ciot_err_t device_event_handler(ciot_iface_t *sender, ciot_event_t *event, void *args)
{
    if (sender->info.type == CIOT_IFACE_TYPE_MQTT_CLIENT)
    {
        if (event->type == CIOT_EVENT_TYPE_EVENT_TYPE_STARTED)
        {
            CIOT_LOGI(TAG, "MQTT Client started");
            static int count = 0;
            uint8_t buf[512];
            int size = 0;

            // Publish NBIRTH (node birth) first
            size = build_and_serialize_nbirth(buf, sizeof(buf));
            if (size > 0)
            {
                ciot_mqtt_client_pub(self.ifaces.mqtt_client, "spBv1.0/1/NBIRTH/1/256848", buf, size, 0);
            }

            // Then publish DBIRTH (device birth)
            size = build_and_serialize_payload(buf, sizeof(buf), &count);
            if (size > 0)
            {
                ciot_mqtt_client_pub(self.ifaces.mqtt_client, "spBv1.0/1/DBIRTH/1/256848", buf, size, 0);
            }
        }
        else if (event->type == CIOT_EVENT_TYPE_EVENT_TYPE_STOPPED)
        {
            CIOT_LOGI(TAG, "MQTT Client stopped");
        }
        else if (event->type == CIOT_EVENT_TYPE_EVENT_TYPE_ERROR)
        {
            CIOT_LOGE(TAG, "MQTT Client error occurred");
        }
    }
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
