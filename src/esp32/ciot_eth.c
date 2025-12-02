/**
 * @file ciot_eth.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-25
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "ciot_config.h"

#if CIOT_CONFIG_FEATURE_ETH == 1 && defined(CIOT_PLATFORM_ESP32)

#include <stdlib.h>
#include "ciot_eth.h"
#include "ciot_tcp.h"

#include "esp_err.h"
#include "esp_eth.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_mac.h"

#ifndef CIOT_CONFIG_ETH_CUSTOM
#define CIOT_CONFIG_ETH_PHY_KSZ8081 1
#define CIOT_CONFIG_ETH_PHY_NEW(x) esp_eth_phy_new_ksz80xx(&x)
#define CIOT_CONFIG_ETH_PHY_ADDR 0
#define CIOT_CONFIG_ETH_GPIO_PHY_RST 5
#define CIOT_CONFIG_ETH_GPIO_MDC 23
#define CIOT_CONFIG_ETH_GPIO_MDIO 18
#endif

struct ciot_eth
{
    ciot_eth_base_t base;
    esp_eth_handle_t eth;
    bool hw_init;
};

static void ciot_eth_event_handler(void *handler_args, esp_event_base_t event_base, int32_t event_id, void *event_data);
static esp_err_t ciot_eth_hw_init(ciot_eth_t self);

static const char *TAG = "ciot_eth";

ciot_eth_t ciot_eth_new(void *handle)
{
    ciot_eth_t self = calloc(1, sizeof(struct ciot_eth));
    ciot_eth_base_t *base = &self->base;
    base->tcp = ciot_tcp_new(&base->iface, CIOT_TCP_TYPE_ETHERNET);
    ciot_eth_init(self);
    return self;
}

ciot_err_t ciot_eth_start(ciot_eth_t self, ciot_tcp_cfg_t *cfg)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(cfg);

    ciot_tcp_base_t *tcp = (ciot_tcp_base_t*)self->base.tcp;

    if(cfg->disabled)
    {
        return ciot_eth_stop(self);
    }

    if(!self->hw_init)
    {
        ESP_ERROR_CHECK(ciot_eth_hw_init(self));
    }

    if(tcp->status->state == CIOT_TCP_STATE_STARTED)
    {
        CIOT_ERR_RETURN(esp_eth_stop(self->eth));
    }

    CIOT_ERR_RETURN(ciot_tcp_set_cfg(self->base.tcp, cfg));
    CIOT_ERR_RETURN(ciot_tcp_start(self->base.tcp));
    CIOT_ERR_RETURN(esp_eth_start(self->eth));

    return CIOT_ERR_NOT_IMPLEMENTED;
}

ciot_err_t ciot_eth_stop(ciot_eth_t self)
{
    ciot_tcp_base_t *tcp = (ciot_tcp_base_t*)self->base.tcp;
    if(tcp->status->state == CIOT_TCP_STATE_STOPPED)
    {
        return ciot_iface_send_event_type(&self->base.iface, CIOT_EVENT_TYPE_STOPPED);
    }
    if(tcp->status->state == CIOT_TCP_STATE_STARTED)
    {
        CIOT_ERR_RETURN(esp_eth_stop(self->eth));
    }
    return CIOT_ERR_OK;
}

static esp_err_t ciot_eth_hw_init(ciot_eth_t self)
{
    ciot_eth_base_t *base = &self->base;

    ciot_tcp_init_netif(base->tcp);

    eth_mac_config_t mac_conf = ETH_MAC_DEFAULT_CONFIG();
    eth_phy_config_t phy_conf = ETH_PHY_DEFAULT_CONFIG();
    eth_esp32_emac_config_t emac_conf = ETH_ESP32_EMAC_DEFAULT_CONFIG();

    CIOT_ERR_RETURN(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, ciot_eth_event_handler, self));

    phy_conf.phy_addr = CIOT_CONFIG_ETH_PHY_ADDR;
    phy_conf.reset_gpio_num = CIOT_CONFIG_ETH_GPIO_PHY_RST;
    
    emac_conf.smi_gpio.mdc_num = CIOT_CONFIG_ETH_GPIO_MDC;
    emac_conf.smi_gpio.mdio_num = CIOT_CONFIG_ETH_GPIO_MDIO;

    esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&emac_conf, &mac_conf);
    esp_eth_phy_t *phy = CIOT_CONFIG_ETH_PHY_NEW(phy_conf);
    esp_eth_config_t eth_conf = ETH_DEFAULT_CONFIG(mac, phy);

    CIOT_ERR_RETURN(esp_eth_driver_install(&eth_conf, &self->eth));
    CIOT_ERR_RETURN(esp_netif_attach((esp_netif_t*)ciot_tcp_get_netif(self->base.tcp), esp_eth_new_netif_glue(self->eth)));

    return CIOT_ERR_OK;
}

static void ciot_eth_event_handler(void *handler_args, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    ciot_eth_t self = (ciot_eth_t)handler_args;
    ciot_eth_base_t *base = &self->base;
    ciot_tcp_base_t *tcp = (ciot_tcp_base_t*)base->tcp;

    switch ((eth_event_t)event_id)
    {
    case ETHERNET_EVENT_START:
        ESP_LOGI(TAG, "ETHERNET_EVENT_START");
        CIOT_ERR_PRINT(TAG, esp_read_mac(tcp->info->mac, ESP_MAC_ETH));
        break;
    case ETHERNET_EVENT_STOP:
        ESP_LOGI(TAG, "ETHERNET_EVENT_STOP");
        break;
    case ETHERNET_EVENT_CONNECTED:
        ESP_LOGI(TAG, "ETHERNET_EVENT_CONNECTED");
        break;
    case ETHERNET_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "ETHERNET_EVENT_DISCONNECTED");
        break;
    default:
        break;
    }
}

#endif  //!CIOT_CONFIG_FEATURE_ETH == 1