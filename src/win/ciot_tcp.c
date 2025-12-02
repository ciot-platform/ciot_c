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

#if CIOT_CONFIG_FEATURE_TCP == 1 && defined(CIOT_PLATFORM_WIN)

#include <stdlib.h>
#include <string.h>
#include "ciot_tcp.h"
#include "ciot_types.h"

#include <winsock2.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

struct ciot_tcp_netif
{
    void *ptr;
};

struct ciot_tcp
{
    ciot_tcp_base_t base;
    struct ciot_tcp_netif netif;
};

static bool init = false;
static const char *TAG = "ciot_tcp";

static ciot_err_t ciot_tcp_get_addr(ciot_tcp_t self);

ciot_tcp_t ciot_tcp_new(ciot_iface_t *iface, ciot_tcp_type_t type)
{
    ciot_tcp_t self = calloc(1, sizeof(struct ciot_tcp));
    ciot_tcp_init(self);
    self->base.type = type;
    self->base.iface = iface;
    return self;
}

ciot_err_t ciot_tcp_init_netif(ciot_tcp_t self)
{
    return CIOT_ERR_OK;
}

ciot_err_t ciot_tcp_start(ciot_tcp_t self)
{
    ciot_tcp_get_addr(self);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_tcp_stop(ciot_tcp_t self)
{
    return CIOT_ERR_OK;
}

static ciot_err_t ciot_tcp_get_addr(ciot_tcp_t self)
{
    ULONG outBufLen = 100000;
    IP_ADAPTER_ADDRESSES *pAddresses = (IP_ADAPTER_ADDRESSES *)malloc(outBufLen);

    if (!pAddresses)
    {
        printf("Memory allocation error\n");
        return 1;
    }

    DWORD dwRetVal = GetAdaptersAddresses(
        AF_UNSPEC,
        GAA_FLAG_INCLUDE_PREFIX,
        NULL,
        pAddresses,
        &outBufLen);

    if (dwRetVal != NO_ERROR)
    {
        CIOT_LOGE(TAG, "GetAdaptersAddresses failed with error: %lu", dwRetVal);
        free(pAddresses);
        return 1;
    }

    IP_ADAPTER_ADDRESSES *pCurr = pAddresses;
    while (pCurr)
    {
        // Confere o tipo da interface conforme seu interesse
        if ((self->base.type == CIOT_TCP_TYPE_ETHERNET && pCurr->IfType == IF_TYPE_ETHERNET_CSMACD) ||
            (self->base.type == CIOT_TCP_TYPE_WIFI_STA && pCurr->IfType == IF_TYPE_IEEE80211))
        {
            // Verifica se existe endereço unicast
            IP_ADAPTER_UNICAST_ADDRESS *pUnicast = pCurr->FirstUnicastAddress;
            while (pUnicast)
            {
                SOCKADDR *sa = pUnicast->Address.lpSockaddr;
                if (sa->sa_family == AF_INET)
                { // IPv4
                    struct sockaddr_in *sa_in = (struct sockaddr_in *)sa;
                    memcpy(self->base.info->ip, &(sa_in->sin_addr), 4);
                    self->base.status->state = CIOT_TCP_STATE_CONNECTED;
                    self->base.status->conn_count++;
                    ciot_iface_send_event_type(self->base.iface, CIOT_EVENT_TYPE_STARTED);
                }
                pUnicast = pUnicast->Next;
            }
        }
        pCurr = pCurr->Next;
    }

    if(self->base.status->state != CIOT_TCP_STATE_CONNECTED)
    {
        ciot_iface_send_event_type(self->base.iface, CIOT_EVENT_TYPE_STOPPED);
    }

    free(pAddresses);
    return 0;
}

#endif // CIOT_CONFIG_FEATURE_TCP == 1
