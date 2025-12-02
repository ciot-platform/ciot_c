/**
 * @file parser.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-05-16
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "main.h"
#include "parser.h"

ciot_err_t parse_args(int argc, char *argv[], ciota_cfg_t *cfg)
{
    if (!cfg) return CIOT_ERR_NULL_ARG;

    memset(cfg, 0, sizeof(*cfg));
    cfg->sender_type = SENDER_TYPE_NONE;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--dir") == 0 && i + 1 < argc) {
            strncpy(cfg->dir, argv[++i], sizeof(cfg->dir) - 1);
        }
        else if (strcmp(argv[i], "--host") == 0 && i + 1 < argc) {
            strncpy(cfg->host, argv[++i], sizeof(cfg->host) - 1);
        }
        else if (strcmp(argv[i], "--filename") == 0 && i + 1 < argc) {
            strncpy(cfg->filename, argv[++i], sizeof(cfg->filename) - 1);
        }
        else if (strcmp(argv[i], "--sender") == 0 && i + 1 < argc) {
            const char *type = argv[++i];
            if (strcmp(type, "uart") == 0) {
                cfg->sender_type = SENDER_TYPE_UART;
            } else if (strcmp(type, "http") == 0) {
                cfg->sender_type = SENDER_TYPE_HTTP_CLIENT;
            } else {
                fprintf(stderr, "Error: invalid sender: %s\n", type);
                return CIOT_ERR_INVALID_ARG;
            }
        }
        else if (strcmp(argv[i], "--iface-id") == 0 && i + 1 < argc) {
            cfg->iface_id = (uint16_t)atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "--baudrate") == 0 && i + 1 < argc) {
            cfg->baudrate = (uint32_t)atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "--port") == 0 && i + 1 < argc) {
            cfg->port = (uint32_t)atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "--url") == 0 && i + 1 < argc) {
            strncpy(cfg->url, argv[++i], CIOT_CONFIG_URL_SIZE - 1);
        }
        else if (strcmp(argv[i], "--force") == 0) {
            cfg->force = true;
        }
        else if (strcmp(argv[i], "--type") == 0 && i + 1 < argc) {
            const char *type = argv[++i];
            if (strcmp(type, "spiffs") == 0) {
                cfg->type = CIOT_OTA_TYPE_DATA_SPIFFS;
            } else {
                cfg->type = CIOT_OTA_TYPE_DEFAULT;
            }
        }
        else if (strcmp(argv[i], "--restart") == 0) {
            cfg->restart = true;
        }
        else {
            fprintf(stderr, "Error: unknown argument: %s\n", argv[i]);
            return CIOT_ERR_INVALID_ARG;
        }
    }

    return CIOT_ERR_OK;
}
