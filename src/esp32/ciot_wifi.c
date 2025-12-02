/**
 * @file ciot_wifi.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "ciot_config.h"

#if CIOT_CONFIG_FEATURE_WIFI == 1 && defined(CIOT_PLATFORM_ESP32)

#include <stdlib.h>
#include <string.h>
#include "ciot_wifi.h"
#include "ciot_err.h"

#include "esp_err.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_wifi_types.h"

struct ciot_wifi
{
    ciot_wifi_base_t base;
    bool reconnecting;
};

static esp_err_t esp_wifi_init_mode(ciot_wifi_type_t type);
static ciot_err_t ciot_wifi_start_sta(ciot_wifi_t self, ciot_wifi_cfg_t *cfg);
static ciot_err_t ciot_wifi_start_ap(ciot_wifi_t self, ciot_wifi_cfg_t *cfg);
static ciot_err_t ciot_wifi_set_cfg(ciot_wifi_t self, ciot_wifi_cfg_t *cfg);
static ciot_err_t ciot_wifi_update_ap_list(ciot_wifi_t self);
static void ciot_wifi_ap_event_handler(void *handler_args, esp_event_base_t event_base, int32_t event_id, void *event_data);
static void ciot_wifi_sta_event_handler(void *handler_args, esp_event_base_t event_base, int32_t event_id, void *event_data);

static const char *TAG = "ciot_wifi";
static bool wifi_init = false;

ciot_wifi_t ciot_wifi_new(ciot_wifi_type_t type)
{
    ciot_wifi_t self = calloc(1, sizeof(struct ciot_wifi));
    ciot_wifi_base_t *base = &self->base;
    
    if (type == CIOT_WIFI_TYPE_STA)
    {
        CIOT_LOGI(TAG, "Creating wifi sta");
        base->tcp = ciot_tcp_new(&base->iface, CIOT_TCP_TYPE_WIFI_STA);
        ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, ciot_wifi_sta_event_handler, self));
    }

    if (type == CIOT_WIFI_TYPE_AP)
    {
        CIOT_LOGI(TAG, "Creating wifi ap");
        base->tcp = ciot_tcp_new(&base->iface, CIOT_TCP_TYPE_WIFI_AP);
        ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, ciot_wifi_ap_event_handler, self));
    }

    ESP_ERROR_CHECK(ciot_tcp_init_netif(base->tcp));

    if (!wifi_init)
    {
        CIOT_LOGI(TAG, "Initializing");
        wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();
        ESP_ERROR_CHECK(esp_wifi_init(&config));
        ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
        ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));
        wifi_init = true;
    }

    ciot_wifi_init(self);
    base->cfg.type = type;
    CIOT_ERR_PRINT(TAG, esp_wifi_get_mac(type, base->info.tcp.mac));

    uint8_t *mac = base->info.tcp.mac;
    CIOT_LOGI(TAG, "Wifi %d [%02X:%02X:%02X:%02X:%02X:%02X] created",
              type,
              mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    ESP_ERROR_CHECK(esp_wifi_init_mode(type));
    ESP_ERROR_CHECK(esp_wifi_start());

    return self;
}

ciot_err_t ciot_wifi_start(ciot_wifi_t self, ciot_wifi_cfg_t *cfg)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(cfg);

    CIOT_ERR_RETURN(ciot_wifi_set_cfg(self, cfg));

    if (cfg->disabled)
    {
        return ciot_wifi_stop(self);
    }

    switch (cfg->type)
    {
    case CIOT_WIFI_TYPE_STA:
        ESP_LOGI(TAG, "Starting Station [ssid:%s pass:%s]", cfg->ssid, cfg->password);
        return ciot_wifi_start_sta(self, cfg);
    case CIOT_WIFI_TYPE_AP:
        ESP_LOGI(TAG, "Starting AP [ssid:%s pass:%s]", cfg->ssid, cfg->password);
        return ciot_wifi_start_ap(self, cfg);
    default:
        return CIOT_ERR_INVALID_TYPE;
    }

    return CIOT_ERR_OK;
}

ciot_err_t ciot_wifi_stop(ciot_wifi_t self)
{
    CIOT_LOGI(TAG, "Stopping WiFi %d", self->base.status.tcp.state);

    self->base.cfg.disabled = true;

    wifi_mode_t mode;
    ESP_ERROR_CHECK(esp_wifi_get_mode(&mode));

    switch (self->base.cfg.type)
    {
    case CIOT_WIFI_TYPE_STA:
        CIOT_LOGI(TAG, "Stopping station");
        ESP_ERROR_CHECK(esp_wifi_disconnect());
        return CIOT_ERR_OK;
    case CIOT_WIFI_TYPE_AP:
        CIOT_LOGI(TAG, "Stopping access point");
        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
        return CIOT_ERR_OK;
    default:
        return CIOT_ERR_INVALID_TYPE;
    }

    return CIOT_ERR_OK;
}

ciot_err_t ciot_wifi_task(ciot_wifi_t self)
{
    return CIOT_ERR_NOT_SUPPORTED;
}

ciot_err_t ciot_wifi_send_bytes(ciot_iface_t *iface, uint8_t *bytes, int size)
{
    return CIOT_ERR_NOT_SUPPORTED;
}

ciot_err_t ciot_wifi_scan(ciot_wifi_t self)
{
    wifi_mode_t wifi_mode;

    CIOT_ERR_RETURN(esp_wifi_get_mode(&wifi_mode));
    if (wifi_mode != WIFI_MODE_APSTA && wifi_mode != WIFI_MODE_STA)
    {
        return CIOT_ERR_INVALID_STATE;
    }

    if(self->base.status.tcp.state == CIOT_TCP_STATE_STOPPED)
    {
        CIOT_ERR_RETURN(esp_wifi_start());
    }

    self->base.status.scan_state = CIOT_WIFI_SCAN_STATE_SCANNING;
    return esp_wifi_scan_start(NULL, false);
}

ciot_err_t ciot_wifi_get_scanned_ap(ciot_wifi_t self, int id, ciot_wifi_ap_info_t *ap_info)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(ap_info);

    if (id < 0 || id >= self->base.ap_list.count)
    {
        return CIOT_ERR_INVALID_INDEX;
    }

    *ap_info = self->base.ap_list.items[id];
    return CIOT_ERR_OK;
}

ciot_err_t ciot_wifi_get_scanned_aps(ciot_wifi_t self, ciot_wifi_ap_list_t *ap_list)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(ap_list);
    *ap_list = self->base.ap_list;
    return CIOT_ERR_OK;
}

static esp_err_t esp_wifi_init_mode(ciot_wifi_type_t type)
{
    wifi_mode_t mode;
    ESP_ERROR_CHECK(esp_wifi_get_mode(&mode));

    switch (type)
    {
    case CIOT_WIFI_TYPE_STA:
        if(mode == WIFI_MODE_STA || mode == WIFI_MODE_APSTA)
        {
            CIOT_LOGI(TAG, "STA mode already initialized");
            return ESP_OK;
        }
        CIOT_LOGI(TAG, "Initializing WiFi in STA mode");
        return(esp_wifi_set_mode(mode == WIFI_MODE_AP ? WIFI_MODE_APSTA: WIFI_MODE_STA));
    case CIOT_WIFI_TYPE_AP:
        if(mode == WIFI_MODE_AP || mode == WIFI_MODE_APSTA)
        {
            CIOT_LOGI(TAG, "AP mode already initialized");
            return ESP_OK;
        }
        CIOT_LOGI(TAG, "Initializing WiFi in AP mode");
        return(esp_wifi_set_mode(mode == WIFI_MODE_STA ? WIFI_MODE_APSTA: WIFI_MODE_AP));
    default:
        return ESP_ERR_INVALID_ARG;
    }
}

static ciot_err_t ciot_wifi_start_sta(ciot_wifi_t self, ciot_wifi_cfg_t *cfg)
{
    ciot_tcp_base_t *tcp = (ciot_tcp_base_t *)self->base.tcp;
    wifi_config_t conf = { 0 };
    strncpy((char *)conf.sta.ssid, cfg->ssid, sizeof(conf.sta.ssid));
    strncpy((char *)conf.sta.password, cfg->password, sizeof(conf.sta.password));

    if (tcp->status->state == CIOT_TCP_STATE_CONNECTED)
    {
        self->reconnecting = false;
        ESP_ERROR_CHECK(esp_wifi_disconnect());
    }

    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &conf));
    CIOT_ERR_PRINT(TAG, esp_wifi_start());

    CIOT_LOGI(TAG, "WiFi state %d", tcp->status->state);
    if (tcp->status->state == CIOT_TCP_STATE_DISCONNECTED ||
        tcp->status->state == CIOT_TCP_STATE_STARTED)
    {
        CIOT_LOGI(TAG, "Wifi sta connecting...");
        tcp->status->state = CIOT_TCP_STATE_CONNECTING;
        CIOT_ERR_PRINT(TAG, esp_wifi_connect());
    }

    return CIOT_ERR_OK;
}

static ciot_err_t ciot_wifi_start_ap(ciot_wifi_t self, ciot_wifi_cfg_t *cfg)
{
    ciot_tcp_base_t *tcp = (ciot_tcp_base_t *)self->base.tcp;
    wifi_config_t conf = { 0 };
    strncpy((char *)conf.ap.ssid, cfg->ssid, sizeof(conf.ap.ssid));
    strncpy((char *)conf.ap.password, cfg->password, sizeof(conf.ap.password));
    conf.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;
    conf.ap.max_connection = 4;
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &conf));
    if(tcp->status->state == CIOT_TCP_STATE_STOPPED)
    {
        esp_wifi_start();
    }
    return CIOT_ERR_OK;
}

static ciot_err_t ciot_wifi_set_cfg(ciot_wifi_t self, ciot_wifi_cfg_t *cfg)
{
    CIOT_LOGI(TAG, "Configuring");

    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(cfg);

    ciot_wifi_base_t *base = &self->base;

    if (self->base.cfg.type != cfg->type)
    {
        CIOT_LOGE(TAG, "Invalid wifi type: %d. Expected: %d", cfg->type, self->base.cfg.type);
        return CIOT_ERR_INVALID_TYPE;
    }

    base->cfg = *cfg;

    if (cfg->has_tcp)
    {
        CIOT_LOGI(TAG, "Set tcp config");
        CIOT_ERR_RETURN(ciot_tcp_set_cfg(base->tcp, &cfg->tcp));
    }

    return CIOT_ERR_OK;
}

static ciot_err_t ciot_wifi_update_ap_list(ciot_wifi_t self)
{
    ciot_wifi_base_t *base = &self->base;
    if(base->ap_list.count > 0 && base->ap_list.items != NULL) {
        free(base->ap_list.items);
        base->ap_list.items = NULL;
    }
    esp_wifi_scan_get_ap_num((uint16_t*)&self->base.ap_list.count);
    base->ap_list.items = calloc(base->ap_list.count, sizeof(ciot_wifi_ap_info_t));
    if(base->ap_list.items == NULL) {
        return CIOT_ERR_NO_MEMORY;
    }
    for (size_t i = 0; i < base->ap_list.count; i++)
    {
        wifi_ap_record_t wifi_ap = { 0 };
        esp_wifi_scan_get_ap_record(&wifi_ap);
        base->ap_list.items[i].authmode = wifi_ap.authmode;
        base->ap_list.items[i].rssi = wifi_ap.rssi;
        memcpy(base->ap_list.items[i].ssid, wifi_ap.ssid, 32);
        memcpy(base->ap_list.items[i].bssid, wifi_ap.bssid, 3);
    }
    esp_wifi_clear_ap_list();
    return CIOT_ERR_OK;
}

static void ciot_wifi_ap_event_handler(void *handler_args, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    ciot_wifi_t self = (ciot_wifi_t)handler_args;
    ciot_wifi_base_t *base = &self->base;
    ciot_tcp_base_t *tcp = (ciot_tcp_base_t *)base->tcp;

    switch (event_id)
    {
    case WIFI_EVENT_AP_START:
    {
        CIOT_LOGI(TAG, "WIFI_EVENT_AP_START");
        tcp->status->state = CIOT_TCP_STATE_STARTED;
        CIOT_ERR_PRINT(TAG, ciot_tcp_start(base->tcp));
        CIOT_ERR_PRINT(TAG, ciot_tcp_get_ip(base->tcp, tcp->info->ip));
        ciot_iface_send_event_type(&self->base.iface, CIOT_EVENT_TYPE_STARTED);
        break;
    }
    case WIFI_EVENT_AP_STOP:
    {
        CIOT_LOGI(TAG, "WIFI_EVENT_AP_STOP");
        tcp->status->state = CIOT_TCP_STATE_STOPPED;
        ciot_iface_send_event_type(&base->iface, CIOT_EVENT_TYPE_STOPPED);
        break;
    }
    case WIFI_EVENT_AP_STACONNECTED:
    {
        CIOT_LOGI(TAG, "WIFI_EVENT_AP_STACONNECTED");
        tcp->status->state = CIOT_TCP_STATE_CONNECTED;
        tcp->status->conn_count++;
        base->status.disconnect_reason = 0;
        ciot_iface_send_event_type(&base->iface, CIOT_WIFI_EVENT_AP_STA_CONNECTED);
        break;
    }
    case WIFI_EVENT_AP_STADISCONNECTED:
    {
        CIOT_LOGI(TAG, "WIFI_EVENT_AP_STADISCONNECTED");
        tcp->status->state = CIOT_TCP_STATE_STARTED;
        base->status.disconnect_reason = ((wifi_event_ap_stadisconnected_t*)event_data)->reason;
        CIOT_LOGI(TAG, "reason: %u", (unsigned int)base->status.disconnect_reason);
        ciot_iface_send_event_type(&base->iface, CIOT_WIFI_EVENT_AP_STA_DISCONNECTED);
        break;
    }
    default:
        break;
    }
}

static void ciot_wifi_sta_event_handler(void *handler_args, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    ciot_wifi_t self = (ciot_wifi_t)handler_args;
    ciot_wifi_base_t *base = &self->base;
    ciot_tcp_base_t *tcp = (ciot_tcp_base_t *)base->tcp;

    switch (event_id)
    {
    case WIFI_EVENT_SCAN_DONE:
    {
        CIOT_LOGI(TAG, "WIFI_EVENT_SCAN_DONE");
        base->status.scan_state = CIOT_WIFI_SCAN_STATE_DONE;
        ciot_event_t *event = calloc(1, sizeof(ciot_event_t));
        event->type = CIOT_EVENT_TYPE_DONE;
        event->which_data = CIOT_EVENT_MSG_TAG;
        event->msg.id = base->iface.req_status.id;
        event->msg.has_iface = true;
        event->msg.iface = base->iface.info;
        event->msg.has_data = true;
        event->msg.data.which_type = CIOT_MSG_DATA_WIFI_TAG;
        event->msg.data.wifi.which_type = CIOT_WIFI_DATA_REQUEST_TAG;
        event->msg.data.wifi.request.which_type = CIOT_WIFI_REQ_SCAN_RESULT_TAG;
        esp_wifi_scan_get_ap_num((uint16_t*)&event->msg.data.wifi.request.scan_result.count);
        ciot_wifi_update_ap_list(self);
        ciot_iface_send_event(&base->iface, event);
        free(event);
        break;
    }
    case WIFI_EVENT_STA_START:
    {
        CIOT_LOGI(TAG, "WIFI_EVENT_STA_START");
        if (base->cfg.ssid[0] != '\0')
        {
            CIOT_LOGI(TAG, "Wifi sta connecting...");
            tcp->status->state = CIOT_TCP_STATE_CONNECTING;
            CIOT_ERR_PRINT(TAG, esp_wifi_connect());
        }
        else
        {
            tcp->status->state = CIOT_TCP_STATE_STARTED;
        }
        break;
    }
    case WIFI_EVENT_STA_STOP:
    {
        CIOT_LOGI(TAG, "WIFI_EVENT_STA_STOP");
        tcp->status->state = CIOT_TCP_STATE_STOPPED;
        break;
    }
    case WIFI_EVENT_STA_CONNECTED:
    {
        CIOT_LOGI(TAG, "WIFI_EVENT_STA_CONNECTED");
        wifi_event_sta_connected_t *data = (wifi_event_sta_connected_t *)event_data;
        tcp->status->state = CIOT_TCP_STATE_CONNECTED;
        tcp->status->conn_count++;
        base->status.disconnect_reason = 0;
        base->info.ap.authmode = data->authmode;
        memcpy(base->info.ap.bssid, data->bssid, sizeof(data->bssid));
        memcpy(base->info.ap.ssid, data->ssid, sizeof(data->ssid));
        self->reconnecting = false;
        CIOT_ERR_PRINT(TAG, ciot_tcp_start(base->tcp));
        break;
    }
    case WIFI_EVENT_STA_DISCONNECTED:
    {
        CIOT_LOGI(TAG, "WIFI_EVENT_STA_DISCONNECTED");
        wifi_event_sta_disconnected_t *data = (wifi_event_sta_disconnected_t *)event_data;
        base->status.disconnect_reason = data->reason;
        base->info.ap.authmode = 0;
        memset(base->info.ap.bssid, 0, sizeof(base->info.ap.bssid));
        memset(base->info.ap.ssid, 0, sizeof(base->info.ap.ssid));
        ciot_iface_send_event_type(&self->base.iface, CIOT_EVENT_TYPE_STOPPED);
        CIOT_LOGI(TAG, "reason: %u", (unsigned int)base->status.disconnect_reason);
        if (base->status.tcp.state == CIOT_TCP_STATE_CONNECTED || self->reconnecting)
        {
            CIOT_LOGI(TAG, "Connection losted. Connecting again...");
            esp_wifi_connect();
            self->reconnecting = true;
        }
        tcp->status->state = CIOT_TCP_STATE_DISCONNECTED;
        break;
    }
    default:
        break;
    }
}

#endif //! CIOT_CONFIG_FEATURE_WIFI == 1