/**
 * @file main.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-05-16
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef __MAIN__H__
#define __MAIN__H__

#include "ciot.h"
#include "ciot_sys.h"
#include "ciot_uart.h"
#include "ciot_http_server.h"
#include "ciot_http_client.h"

#include "ciot_timer.h"

/**
 * @brief Enumeration of all interface identifiers.
 */
typedef enum iface_id
{
    IFACE_ID_CIOT = 0,         /**< CIoT base interface. */
    IFACE_ID_CIOT_SYS,         /**< System interface. */
    IFACE_ID_CIOT_UART,        /**< UART interface. */
    IFACE_ID_CIOT_HTTP_SERVER, /**< HTTP Server interface. */
    IFACE_ID_CIOT_HTTP_CLIENT, /**< HTTP Client interface. */
    IFACE_ID_CIOT_OTA,         /**< OTA interface. */
    IFACE_ID_MAX               /**< Maximum number of interfaces (used for bounds). */
} iface_id_t;

/**
 * @brief Enumeration of sender types that can initiate OTA operations.
 */
typedef enum sender_type
{
    SENDER_TYPE_NONE = 0,   /**< No sender defined. */
    SENDER_TYPE_UART,       /**< Sender is a UART interface. */
    SENDER_TYPE_HTTP_CLIENT /**< Sender is an HTTP client interface. */
} sender_type_t;

/**
 * @brief Enumeration representing the states of the OTA process.
 */
typedef enum ciota_state
{
    CIOTA_STATE_IDLE = 0,        /**< No OTA operation ongoing. */
    CIOTA_STATE_GET_OTA_STATUS,  /**< Requesting OTA status from device. */
    CIOTA_STATE_WAIT_OTA_STATUS, /**< Waiting for OTA status response. */
    CIOTA_STATE_START,           /**< Starting OTA process. */
    CIOTA_STATE_IN_PROGRESS,     /**< OTA process is currently in progress. */
    CIOTA_STATE_CHECK_PROGRESS,  /**< Checking progress of OTA. */
    CIOTA_STATE_ERROR            /**< OTA process encountered an error. */
} ciota_state_t;

/**
 * @brief Structure grouping all interface instances and configurations.
 */
typedef struct ifaces
{
    ciot_t ciot;                         /**< Base CIoT instance. */
    ciot_sys_t sys;                      /**< System interface instance. */
    ciot_uart_t uart;                    /**< UART interface instance. */
    ciot_http_server_t http_server;      /**< HTTP Server interface instance. */
    ciot_http_client_t http_client;      /**< HTTP Client interface instance. */
    ciot_iface_t *list[IFACE_ID_MAX];    /**< List of interface pointers indexed by iface_id_t. */
    ciot_msg_data_t *cfgs[IFACE_ID_MAX]; /**< Configuration data for each interface. */
} ifaces_t;

/**
 * @brief Configuration parameters for an OTA operation.
 */
typedef struct ciota_cfg
{
    char dir[32];                   /**< Directory path for the OTA file. */
    char filename[32];              /**< Name of the OTA file. */
    char host[48];                  /**< Hostname or IP of the OTA server. */
    sender_type_t sender_type;      /**< Type of interface that initiated the OTA. */
    uint16_t iface_id;              /**< ID of the interface to apply OTA. */
    uint32_t baudrate;              /**< UART baudrate (if applicable). */
    uint32_t port;                  /**< Port number for the OTA server. */
    char url[CIOT_CONFIG_URL_SIZE]; /**< Full URL for OTA file download. */
    bool force;                     /**< Whether to force the update regardless of current state. */
    ciot_ota_type_t type;           /**< OTA type (e.g., firmware, filesystem). */
    bool restart;                   /**< Whether to restart the system after update. */
} ciota_cfg_t;

/**
 * @brief Main structure managing OTA logic and interface context.
 */
typedef struct ciota
{
    ifaces_t ifaces;               /**< All available interfaces and their configuration. */
    ciota_cfg_t cfg;               /**< OTA configuration parameters. */
    ciota_state_t state;           /**< Current state of the OTA state machine. */
    uint64_t timer_check_progress; /**< Timer to control when to check OTA progress. */
} ciota_t;

#endif //!__MAIN__H__