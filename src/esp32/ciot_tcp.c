/**
 * @file ciot_tcp.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "ciot_config.h"

#if CIOT_CONFIG_FEATURE_TCP == 1 && defined(CIOT_PLATFORM_ESP32)

#include <stdlib.h>
#include <string.h>
#include "ciot_tcp.h"
#include "ciot_types.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "lwip/ip_addr.h"

#define CIOT_ESP_NETIF_INHERENT_DEFAULT_ETH()                   \
    {                                                           \
        .get_ip_event = IP_EVENT_ETH_GOT_IP,                    \
        .lost_ip_event = 0,                                     \
        .flags = ESP_NETIF_DHCP_SERVER | ESP_NETIF_FLAG_AUTOUP, \
        .ip_info = (esp_netif_ip_info_t *)&ciot_eth_ap_ip,      \
        .if_key = "ETHDHCPS",                                   \
        .if_desc = "eth",                                       \
        .route_prio = 50}

#define CIOT_NETIF_DEFAULT_ETH()                        \
    {                                                   \
        .base = &_g_ciot_esp_netif_inherent_eth_config, \
        .driver = NULL,                                 \
        .stack = ESP_NETIF_NETSTACK_DEFAULT_ETH,        \
    }

struct ciot_tcp
{
    ciot_tcp_base_t base;
    esp_netif_t *netif;
};

static ciot_err_t ciot_tcp_set_ip_cfg(ciot_tcp_t self, ciot_tcp_cfg_t *cfg);
static void ciot_tcp_sta_event_handler(void *handler_args, esp_event_base_t event_base, int32_t event_id, void *event_data);
static void ciot_tcp_ap_event_handler(void *handler_args, esp_event_base_t event_base, int32_t event_id, void *event_data);
static void ciot_tcp_eth_event_handler(void *handler_args, esp_event_base_t event_base, int32_t event_id, void *event_data);
static void ciot_tcp_event_handler(void *handler_args, esp_event_base_t event_base, int32_t event_id, void *event_data);

static bool tcp_init = false;
static const char *TAG = "ciot_tcp";

const esp_netif_ip_info_t ciot_eth_ap_ip = {
    .ip = {.addr = PP_HTONL(LWIP_MAKEU32(192, 168, 1, 1))},
    .gw = {.addr = PP_HTONL(LWIP_MAKEU32(192, 168, 1, 1))},
    .netmask = {.addr = PP_HTONL(LWIP_MAKEU32(255, 255, 255, 0))},
};

const esp_netif_inherent_config_t _g_ciot_esp_netif_inherent_eth_config = CIOT_ESP_NETIF_INHERENT_DEFAULT_ETH();

ciot_tcp_t ciot_tcp_new(ciot_iface_t *iface, ciot_tcp_type_t type)
{
    ciot_tcp_t self = calloc(1, sizeof(struct ciot_tcp));
    ciot_tcp_init(self);
    if (!tcp_init)
    {
        ESP_ERROR_CHECK(esp_netif_init());
        ESP_ERROR_CHECK(esp_event_loop_create_default());
        tcp_init = true;
    }
    self->base.type = type;
    self->base.iface = iface;
    return self;
}

ciot_err_t ciot_tcp_init_netif(ciot_tcp_t self)
{
    CIOT_ERR_NULL_CHECK(self);

    if (self->netif != NULL)
    {
        CIOT_LOGW(TAG, "Netif already created");
        return CIOT_ERR_OK;
    }

    switch (self->base.type)
    {
    case CIOT_TCP_TYPE_WIFI_STA:
    {
        self->netif = esp_netif_create_default_wifi_sta();
        ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID, ciot_tcp_sta_event_handler, self));
        CIOT_LOGI(TAG, "Wifi station netif created at %p", self->netif);
        break;
    }
    case CIOT_TCP_TYPE_WIFI_AP:
    {
        self->netif = esp_netif_create_default_wifi_ap();
        ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID, ciot_tcp_ap_event_handler, self));
        CIOT_LOGI(TAG, "Wifi access point netif created at %p", self->netif);
        break;
    }
    case CIOT_TCP_TYPE_ETHERNET:
    {
        esp_netif_config_t netif_cfg = ESP_NETIF_DEFAULT_ETH();
        self->netif = esp_netif_new(&netif_cfg);
        ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID, ciot_tcp_eth_event_handler, self));
        CIOT_LOGI(TAG, "Ethernet netif created at %p", self->netif);
        break;
    }
    default:
        return CIOT_ERR_INVALID_TYPE;
    }

    return self->netif != NULL ? CIOT_ERR_OK : CIOT_ERR_FAIL;
}

ciot_err_t ciot_tcp_start(ciot_tcp_t self)
{
    CIOT_ERR_NULL_CHECK(self);
    ciot_tcp_base_t *base = &self->base;
    
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(self->netif);

    esp_netif_t *netif = self->netif;
    esp_netif_dhcp_status_t dhcpc = ESP_NETIF_DHCP_STOPPED;
    esp_netif_dhcp_status_t dhcps = ESP_NETIF_DHCP_STOPPED;
    esp_netif_flags_t flags = esp_netif_get_flags(netif);

    if (flags & ESP_NETIF_DHCP_CLIENT)
    {
        CIOT_ERR_PRINT(TAG, esp_netif_dhcpc_get_status(netif, &dhcpc));
    }

    if (flags & ESP_NETIF_DHCP_SERVER)
    {
        CIOT_ERR_PRINT(TAG, esp_netif_dhcps_get_status(netif, &dhcps));
    }

    switch (self->base.cfg->dhcp)
    {
    case CIOT_TCP_DHCP_CFG_NO_CHANGE:
        ESP_LOGI(TAG, "CIOT_TCP_DHCP_CFG_NO_CHANGE");
        return CIOT_ERR_OK;
    case CIOT_TCP_DHCP_CFG_CLIENT:
        ESP_LOGI(TAG, "CIOT_TCP_DHCP_CFG_CLIENT");
        if (dhcps != ESP_NETIF_DHCP_STOPPED)
        {
            CIOT_LOGI(TAG, "Stopping dhcp server");
            CIOT_ERR_RETURN(esp_netif_dhcps_stop(netif));
        }
        if (dhcpc != ESP_NETIF_DHCP_STARTED)
        {
            CIOT_LOGI(TAG, "Starting dhcp client");
            CIOT_ERR_RETURN(esp_netif_dhcpc_start(netif));
        }
        break;
    case CIOT_TCP_DHCP_CFG_SERVER:
        ESP_LOGI(TAG, "CIOT_TCP_DHCP_CFG_SERVER");
        if (dhcpc != ESP_NETIF_DHCP_STOPPED)
        {
            CIOT_LOGI(TAG, "Stopping dhcp client");
            CIOT_ERR_RETURN(esp_netif_dhcpc_stop(netif));
        }
        if (dhcps != ESP_NETIF_DHCP_STARTED)
        {
            CIOT_LOGI(TAG, "Starting dhcp server");
            CIOT_ERR_RETURN(ciot_tcp_set_ip_cfg(self, self->base.cfg));
            CIOT_ERR_RETURN(esp_netif_dhcps_start(netif));
        }
        break;
    case CIOT_TCP_DHCP_CFG_DISABLED:
        ESP_LOGI(TAG, "CIOT_TCP_DHCP_CFG_DISABLED");
        if (dhcpc != ESP_NETIF_DHCP_STOPPED)
        {
            CIOT_LOGI(TAG, "Stopping dhcp client");
            CIOT_ERR_RETURN(esp_netif_dhcpc_stop(netif));
        }
        if (dhcps != ESP_NETIF_DHCP_STOPPED)
        {
            CIOT_LOGI(TAG, "Stopping dhcp server");
            CIOT_ERR_RETURN(esp_netif_dhcps_stop(netif));
        }
        CIOT_ERR_RETURN(ciot_tcp_set_ip_cfg(self, self->base.cfg));
        break;
    default:
        return CIOT_ERR_INVALID_ARG;
    }
    return CIOT_ERR_OK;

    base->status->state = CIOT_TCP_STATE_STARTED;
    return CIOT_ERR_OK;
}

ciot_err_t ciot_tcp_stop(ciot_tcp_t self)
{
    CIOT_ERR_NULL_CHECK(self);
    return CIOT_ERR_NOT_IMPLEMENTED;
}

static ciot_err_t ciot_tcp_set_ip_cfg(ciot_tcp_t self, ciot_tcp_cfg_t *cfg)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(cfg);
    CIOT_ERR_NULL_CHECK(self->netif);

    if (cfg->dhcp == CIOT_TCP_DHCP_CFG_DISABLED ||
        cfg->dhcp == CIOT_TCP_DHCP_CFG_SERVER)
    {
        ESP_LOGI(TAG, "cfg: ip:%d.%d.%d.%d gw:%d.%d.%d.%d mask:%d.%d.%d.%d dns:%d.%d.%d.%d",
                 cfg->ip[0], cfg->ip[1], cfg->ip[2], cfg->ip[3],
                 cfg->gateway[0], cfg->gateway[1], cfg->gateway[2], cfg->gateway[3],
                 cfg->mask[0], cfg->mask[1], cfg->mask[2], cfg->mask[3],
                 cfg->dns[0], cfg->dns[1], cfg->dns[2], cfg->dns[3]);

        char ip[16];
        char gateway[16];
        char mask[16];
        char dns[16];

        esp_netif_t *netif = self->netif;
        esp_netif_ip_info_t ip_info;
        esp_netif_dns_info_t dns_info;

        sprintf(ip, "%d.%d.%d.%d", cfg->ip[0], cfg->ip[1], cfg->ip[2], cfg->ip[3]);
        sprintf(gateway, "%d.%d.%d.%d", cfg->gateway[0], cfg->gateway[1], cfg->gateway[2], cfg->gateway[3]);
        sprintf(mask, "%d.%d.%d.%d", cfg->mask[0], cfg->mask[1], cfg->mask[2], cfg->mask[3]);
        sprintf(dns, "%d.%d.%d.%d", cfg->dns[0], cfg->dns[1], cfg->dns[2], cfg->dns[3]);

        ip_info.ip.addr = ipaddr_addr(ip);
        ip_info.gw.addr = ipaddr_addr(gateway);
        ip_info.netmask.addr = ipaddr_addr(mask);
        dns_info.ip.u_addr.ip4.addr = ipaddr_addr(mask);
        dns_info.ip.type = IPADDR_TYPE_V4;

        CIOT_ERR_RETURN(esp_netif_set_ip_info(netif, &ip_info));
        CIOT_ERR_RETURN(esp_netif_set_dns_info(netif, ESP_NETIF_DNS_MAIN, &dns_info));

        ciot_tcp_info_t *info = self->base.info;
        info->ip[0] = ip4_addr1(&ip_info.ip);
        info->ip[1] = ip4_addr2(&ip_info.ip);
        info->ip[2] = ip4_addr3(&ip_info.ip);
        info->ip[3] = ip4_addr4(&ip_info.ip);
    }
    return CIOT_ERR_OK;
}

static void ciot_tcp_sta_event_handler(void *handler_args, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_id == IP_EVENT_STA_GOT_IP || event_id == IP_EVENT_STA_LOST_IP)
    {
        ciot_tcp_event_handler(handler_args, event_base, event_id, event_data);
    }
}

static void ciot_tcp_ap_event_handler(void *handler_args, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_id == IP_EVENT_AP_STAIPASSIGNED)
    {
        ciot_tcp_event_handler(handler_args, event_base, event_id, event_data);
    }
}

static void ciot_tcp_eth_event_handler(void *handler_args, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_id == IP_EVENT_ETH_GOT_IP || event_id == IP_EVENT_ETH_LOST_IP)
    {
        ciot_tcp_event_handler(handler_args, event_base, event_id, event_data);
    }
}

static void ciot_tcp_event_handler(void *handler_args, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    ciot_tcp_t self = (ciot_tcp_t)handler_args;
    ciot_tcp_base_t *base = &self->base;

    switch ((ip_event_t)event_id)
    {
    case IP_EVENT_ETH_GOT_IP:
    case IP_EVENT_STA_GOT_IP:
    {
        CIOT_LOGI(TAG, "IP_EVENT_GOT_IP");

        ip_event_got_ip_t *ip_event = (ip_event_got_ip_t *)event_data;
        esp_netif_dhcp_status_t dhcpc = ESP_NETIF_DHCP_STOPPED;
        esp_netif_dhcp_status_t dhcps = ESP_NETIF_DHCP_STOPPED;

        base->info->ip[0] = ip4_addr1(&ip_event->ip_info.ip);
        base->info->ip[1] = ip4_addr2(&ip_event->ip_info.ip);
        base->info->ip[2] = ip4_addr3(&ip_event->ip_info.ip);
        base->info->ip[3] = ip4_addr4(&ip_event->ip_info.ip);

        if (esp_netif_dhcpc_get_status(ip_event->esp_netif, &dhcpc) == ESP_OK)
        {
            base->status->dhcp.client = dhcpc;
        }
        if (esp_netif_dhcps_get_status(ip_event->esp_netif, &dhcps) == ESP_OK)
        {
            base->status->dhcp.server = dhcps;
        }

        base->status->state = CIOT_TCP_STATE_CONNECTED;
        ciot_iface_send_event_type(base->iface, CIOT_EVENT_TYPE_STARTED);
        break;
    }
    case IP_EVENT_AP_STAIPASSIGNED:
    {
        CIOT_LOGI(TAG, "IP_EVENT_AP_STAIPASSIGNED");
        break;
    }
    case IP_EVENT_ETH_LOST_IP:
    case IP_EVENT_STA_LOST_IP:
    {
        CIOT_LOGI(TAG, "IP_EVENT_LOST_IP");
        base->status->state = CIOT_TCP_STATE_DISCONNECTED;
        ciot_iface_send_event_type(base->iface, CIOT_EVENT_TYPE_STOPPED);
        break;
    }
    default:
        return;
    }
}

ciot_err_t ciot_tcp_get_ip(ciot_tcp_t self, uint8_t *ip)
{
    CIOT_ERR_NULL_CHECK(self);
    esp_netif_ip_info_t ip_info;
    esp_netif_get_ip_info(self->netif, &ip_info);
    memcpy(ip, &ip_info.ip.addr, sizeof(ip_info.ip.addr));
    return CIOT_ERR_OK;
}

void *ciot_tcp_get_netif(ciot_tcp_t self)
{
    if (self == NULL)
        return NULL;
    return self->netif;
}

#endif  //!CIOT_CONFIG_FEATURE_TCP == 1