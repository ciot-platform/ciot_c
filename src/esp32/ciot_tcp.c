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

#include <stdlib.h>
#include <string.h>
#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "lwip/ip_addr.h"
#include "ciot_tcp.h"
#include "ciot_msg.h"
#include "ciot_err.h"

struct ciot_tcp_netif
{
    esp_netif_t *ptr;
};

struct ciot_tcp
{
    ciot_tcp_base_t base;
    struct ciot_tcp_netif netif;
};

static ciot_err_t ciot_tcp_set_dhcp_cfg(ciot_tcp_t self, ciot_tcp_dhcp_cfg_t dhcp);
static ciot_err_t ciot_tcp_set_ip_cfg(ciot_tcp_t self, ciot_tcp_cfg_t *cfg);
static void ciot_tcp_event_handler(void *handler_args, esp_event_base_t event_base, int32_t event_id, void *event_data);

static bool tcp_init = false;
static const char *TAG = "ciot_tcp";

ciot_tcp_t ciot_tcp_new(ciot_iface_t *iface, ciot_tcp_type_t type)
{
    ciot_tcp_t self = calloc(1, sizeof(struct ciot_tcp));
    ciot_tcp_init(self);
    if(!tcp_init)
    {
        ESP_ERROR_CHECK(esp_netif_init());
        ESP_ERROR_CHECK(esp_event_loop_create_default());
        tcp_init = true;
    }
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID, ciot_tcp_event_handler, self));
    self->base.netif = &self->netif;
    self->base.type = type;
    self->base.iface_p = iface;
    return self;
}

ciot_err_t ciot_tcp_init_netif(ciot_tcp_t self)
{
    CIOT_ERR_NULL_CHECK(self);

    switch (self->base.type)
    {
    case CIOT_TCP_TYPE_WIFI_STA:
    {
        self->netif.ptr = esp_netif_create_default_wifi_sta();
        CIOT_LOGI(TAG, "Wifi station netif created at %p", self->netif.ptr);
        break;
    }
    case CIOT_TCP_TYPE_WIFI_AP:
    {
        self->netif.ptr = esp_netif_create_default_wifi_ap();
        CIOT_LOGI(TAG, "Wifi access point netif created at %p", self->netif.ptr);
        break;
    }
    case CIOT_TCP_TYPE_ETHERNET:
    {
        esp_netif_config_t netif_cfg = ESP_NETIF_DEFAULT_ETH();
        self->netif.ptr = esp_netif_new(&netif_cfg);
        CIOT_LOGI(TAG, "Ethernet netif created at %p", self->netif.ptr);
        break;
    }
    default:
        return CIOT_ERR__INVALID_TYPE;
    }

    return self->netif.ptr != NULL ? CIOT_ERR__OK : CIOT_ERR__FAIL;
}

ciot_err_t ciot_tcp_start(ciot_tcp_t self)
{
    CIOT_ERR_NULL_CHECK(self);

    ciot_tcp_base_t *base = &self->base;

    CIOT_ERR_RETURN(ciot_tcp_set_dhcp_cfg(self, base->cfg.dhcp));
    CIOT_ERR_RETURN(ciot_tcp_set_ip_cfg(self, &base->cfg));

    ciot_iface_event_t event = { 0 };
    event.type = CIOT_IFACE_EVENT_STARTED;
    event.msg = ciot_msg_get(CIOT__MSG_TYPE__MSG_TYPE_STATUS, base->iface_p);
    base->status.state = CIOT__TCP_STATE__TCP_STATE_STARTED;
    ciot_iface_send_event(base->iface_p, &event);

    return CIOT_ERR__OK;
}

ciot_err_t ciot_tcp_stop(ciot_tcp_t self)
{
    CIOT_ERR_NULL_CHECK(self);
    return CIOT_ERR__OK;
}

static ciot_err_t ciot_tcp_set_dhcp_cfg(ciot_tcp_t self, ciot_tcp_dhcp_cfg_t dhcp)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(self->netif.ptr);

    esp_netif_t *netif = self->netif.ptr;
    esp_netif_dhcp_status_t dhcpc = ESP_NETIF_DHCP_STOPPED;
    esp_netif_dhcp_status_t dhcps = ESP_NETIF_DHCP_STOPPED;
    esp_netif_flags_t flags = esp_netif_get_flags(netif);

    if(flags & ESP_NETIF_DHCP_CLIENT)
    {
        CIOT_ERR_PRINT(TAG, esp_netif_dhcpc_get_status(netif, &dhcpc));
    }

    if(flags & ESP_NETIF_DHCP_SERVER)
    {
        CIOT_ERR_PRINT(TAG, esp_netif_dhcps_get_status(netif, &dhcps));
    }

    switch (dhcp)
    {
    case CIOT__TCP_DHCP_CFG__TCP_DHCP_CFG_NO_CHANGE:
        ESP_LOGI(TAG, "CIOT_TCP_DHCP_CFG_NO_CHANGE");
        return CIOT_ERR__OK;
    case CIOT__TCP_DHCP_CFG__TCP_DHCP_CFG_CLIENT:
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
    case CIOT__TCP_DHCP_CFG__TCP_DHCP_CFG_SERVER:
        ESP_LOGI(TAG, "CIOT_TCP_DHCP_CFG_SERVER");
        if (dhcpc != ESP_NETIF_DHCP_STOPPED)
        {
            CIOT_LOGI(TAG, "Stopping dhcp client");
            CIOT_ERR_RETURN(esp_netif_dhcpc_stop(netif));
        }
        if (dhcps != ESP_NETIF_DHCP_STARTED)
        {
            CIOT_LOGI(TAG, "Stopping dhcp server");
            CIOT_ERR_RETURN(esp_netif_dhcps_start(netif));
        }
        break;
    case CIOT__TCP_DHCP_CFG__TCP_DHCP_CFG_DISABLED:
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
        break;
    default:
        return CIOT_ERR__INVALID_ARG;
    }
    return CIOT_ERR__OK;
}

static ciot_err_t ciot_tcp_set_ip_cfg(ciot_tcp_t self, ciot_tcp_cfg_t *cfg)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(cfg);
    CIOT_ERR_NULL_CHECK(self->netif.ptr);


    if (cfg->dhcp == CIOT__TCP_DHCP_CFG__TCP_DHCP_CFG_DISABLED)
    {
        ESP_LOGI(TAG, "cfg:cfg: cfg:%d.%d.%d.%d gw:%d.%d.%d.%d mask:%d.%d.%d.%d dns:%d.%d.%d.%d",
                 cfg->ip.data[0], cfg->ip.data[1], cfg->ip.data[2], cfg->ip.data[3],
                 cfg->gateway.data[0], cfg->gateway.data[1], cfg->gateway.data[2], cfg->gateway.data[3],
                 cfg->mask.data[0], cfg->mask.data[1], cfg->mask.data[2], cfg->mask.data[3],
                 cfg->dns.data[0], cfg->dns.data[1], cfg->dns.data[2], cfg->dns.data[3]);

        char ip[16];
        char gateway[16];
        char mask[16];
        char dns[16];

        esp_netif_t *netif = self->netif.ptr;
        esp_netif_ip_info_t ip_info;
        esp_netif_dns_info_t dns_info;

        sprintf(ip, "%d.%d.%d.%d", cfg->ip.data[0], cfg->ip.data[1], cfg->ip.data[2], cfg->ip.data[3]);
        sprintf(gateway, "%d.%d.%d.%d", cfg->gateway.data[0], cfg->gateway.data[1], cfg->gateway.data[2], cfg->gateway.data[3]);
        sprintf(mask, "%d.%d.%d.%d", cfg->mask.data[0], cfg->mask.data[1], cfg->mask.data[2], cfg->mask.data[3]);
        sprintf(dns, "%d.%d.%d.%d", cfg->dns.data[0], cfg->dns.data[1], cfg->dns.data[2], cfg->dns.data[3]);

        ip_info.ip.addr = ipaddr_addr(ip);
        ip_info.gw.addr = ipaddr_addr(gateway);
        ip_info.netmask.addr = ipaddr_addr(mask);
        dns_info.ip.u_addr.ip4.addr = ipaddr_addr(mask);
        dns_info.ip.type = IPADDR_TYPE_V4;

        CIOT_ERR_RETURN(esp_netif_set_ip_info(netif, &ip_info));
        CIOT_ERR_RETURN(esp_netif_set_dns_info(netif, ESP_NETIF_DNS_MAIN, &dns_info));

        ciot_tcp_info_t *info = &self->base.info;
        info->ip.data[0] = ip4_addr1(&ip_info.ip);
        info->ip.data[1] = ip4_addr2(&ip_info.ip);
        info->ip.data[2] = ip4_addr3(&ip_info.ip);
        info->ip.data[3] = ip4_addr4(&ip_info.ip);
    }
    return CIOT_ERR__OK;
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

        ip_event_got_ip_t *ip_event = (ip_event_got_ip_t*)event_data;
        esp_netif_dhcp_status_t dhcpc = ESP_NETIF_DHCP_STOPPED;
        esp_netif_dhcp_status_t dhcps = ESP_NETIF_DHCP_STOPPED;

        base->info.ip.data[0] = ip4_addr1(&ip_event->ip_info.ip);
        base->info.ip.data[1] = ip4_addr2(&ip_event->ip_info.ip);
        base->info.ip.data[2] = ip4_addr3(&ip_event->ip_info.ip);
        base->info.ip.data[3] = ip4_addr4(&ip_event->ip_info.ip);

        if (esp_netif_dhcpc_get_status(ip_event->esp_netif, &dhcpc) == ESP_OK)
        {
            base->status.dhcp->client = dhcpc;
        }
        if (esp_netif_dhcps_get_status(ip_event->esp_netif, &dhcps) == ESP_OK)
        {
            base->status.dhcp->server = dhcps;
        }

        ciot_iface_event_t event = { 0 };
        event.type = CIOT_IFACE_EVENT_STARTED;
        event.msg = ciot_msg_get(CIOT__MSG_TYPE__MSG_TYPE_STATUS, base->iface_p);
        base->status.state = CIOT__TCP_STATE__TCP_STATE_CONNECTED;
        ciot_iface_send_event(base->iface_p, &event);
        break;
    }
    case IP_EVENT_ETH_LOST_IP:
    case IP_EVENT_STA_LOST_IP:
    {
        CIOT_LOGI(TAG, "IP_EVENT_LOST_IP");
        ciot_iface_event_t event = { 0 };
        event.type = CIOT_IFACE_EVENT_STARTED;
        event.msg = ciot_msg_get(CIOT__MSG_TYPE__MSG_TYPE_STATUS, base->iface_p);
        base->status.state = CIOT__TCP_STATE__TCP_STATE_DISCONNECTED;
        ciot_iface_send_event(base->iface_p, &event);
        break;
    }
    default:
        return;
    }
}
