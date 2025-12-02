/**
 * @file ciot_ota.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-06-07
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "ciot_config.h"

#if CIOT_CONFIG_FEATURE_OTA == 1 && defined(CIOT_PLATFORM_ESP32)

#include <stdlib.h>
#include "ciot_ota.h"
#include "ciot_err.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_ota_ops.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"

#ifdef CONFIG_ESP_HTTPS_OTA_DECRYPT_CB
#include "esp_encrypted_img.h"
#endif // CONFIG_ESP_HTTPS_OTA_DECRYPT_CB

#ifdef CONFIG_MBEDTLS_CERTIFICATE_BUNDLE
#include "esp_crt_bundle.h"
#endif // CONFIG_MBEDTLS_CERTIFICATE_BUNDLE

struct ciot_ota
{
    ciot_ota_base_t base;
    TaskHandle_t task;
    esp_https_ota_handle_t handle;
    esp_partition_type_t partition_type;
    esp_partition_subtype_t partition_subtype;
#ifdef CONFIG_ESP_HTTPS_OTA_DECRYPT_CB
    esp_decrypt_handle_t decrypt_handle;
#endif // CONFIG_ESP_HTTPS_OTA_DECRYPT_CB
};

static void ciot_ota_task(void *pvParameters);
static void ciot_ota_task_advanced(void *pvParameters);
static void __attribute__((noreturn)) ciot_ota_task_fatal_error(ciot_ota_t self);
static void ciot_ota_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
#ifdef CONFIG_ESP_HTTPS_OTA_DECRYPT_CB
static esp_err_t ciot_ota_decrypt_cb(decrypt_cb_arg_t *args, void *user_ctx);
#endif // CONFIG_ESP_HTTPS_OTA_DECRYPT_CB
static esp_err_t ciot_ota_validate_image_header(esp_app_desc_t *new_app_info, bool force);

static const char *TAG = "ciot_ota";

#ifdef CONFIG_ESP_HTTPS_OTA_DECRYPT_CB
extern const char rsa_private_pem_start[] asm("_binary_private_pem_start");
extern const char rsa_private_pem_end[] asm("_binary_private_pem_end");
#endif // CONFIG_ESP_HTTPS_OTA_DECRYPT_CB

ciot_ota_t ciot_ota_new(void *handle)
{
    ciot_ota_t self = calloc(1, sizeof(struct ciot_ota));
    ciot_ota_init(self);
    return self;
}

ciot_err_t ciot_ota_start(ciot_ota_t self, ciot_ota_cfg_t *cfg)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(cfg);

    ciot_ota_base_t *base = &self->base;

    if (base->status.state == CIOT_OTA_STATE_ERROR)
    {
        base->status.state = CIOT_OTA_STATE_IDLE;
        base->status.error = CIOT_ERR_OK;
        base->status.image_size = 0;
        base->status.image_written = 0;
    }

    if (base->status.state != CIOT_OTA_STATE_IDLE)
    {
        CIOT_LOGE(TAG, "OTA invalid state: %d", base->status.state);
        return CIOT_ERR_INVALID_STATE;
    }

    base->status.state = CIOT_OTA_STATE_INIT;
    base->status.error = CIOT_ERR_OK;
    base->cfg = *cfg;

    switch (cfg->type)
    {
    case CIOT_OTA_TYPE_DEFAULT:
        ESP_ERROR_CHECK(esp_event_handler_register(ESP_HTTPS_OTA_EVENT, ESP_EVENT_ANY_ID, ciot_ota_event_handler, self));
        xTaskCreatePinnedToCore(ciot_ota_task, "ciot_ota_task", CIOT_CONFIG_OTA_TASK_STACK_SIZE, self, CIOT_CONFIG_OTA_TASK_PRIORITY, &self->task, CIOT_CONFIG_OTA_TASK_CORE_ID);
        break;
    case CIOT_OTA_TYPE_DATA_SPIFFS:
        self->partition_type = ESP_PARTITION_TYPE_DATA;
        self->partition_subtype = ESP_PARTITION_SUBTYPE_DATA_SPIFFS;
        ESP_ERROR_CHECK(esp_event_handler_register(ESP_HTTPS_OTA_EVENT, ESP_EVENT_ANY_ID, ciot_ota_event_handler, self));
        xTaskCreatePinnedToCore(ciot_ota_task_advanced, "ciot_ota_task", CIOT_CONFIG_OTA_TASK_STACK_SIZE, self, CIOT_CONFIG_OTA_TASK_PRIORITY, &self->task, CIOT_CONFIG_OTA_TASK_CORE_ID);
        break;
    default:
        return CIOT_ERR_INVALID_TYPE;
    }

    return base->status.error;
}

ciot_err_t ciot_ota_stop(ciot_ota_t self)
{
    CIOT_ERR_NULL_CHECK(self);
    vTaskDelete(self->task);
    free(self);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_ota_rollback(ciot_ota_t self)
{
    if (esp_ota_check_rollback_is_possible())
    {
        return esp_ota_mark_app_invalid_rollback_and_reboot();
    }
    else
    {
        return CIOT_ERR_IMPOSSIBLE_OP;
    }
}

static void ciot_ota_task(void *pvParameters)
{
    ciot_ota_t self = (ciot_ota_t)pvParameters;
    ciot_ota_base_t *base = &self->base;

    ESP_LOGI(TAG, "OTA application start.");
    ESP_LOGI(TAG, "cfg: url:%s", base->cfg.url);

    esp_http_client_config_t http_client_config = {
        .url = (const char *)base->cfg.url,
        .timeout_ms = 5000,
        .keep_alive_enable = true,
#ifdef CONFIG_MBEDTLS_CERTIFICATE_BUNDLE
        .crt_bundle_attach = esp_crt_bundle_attach
#endif // CONFIG_MBEDTLS_CERTIFICATE_BUNDLE
    };

#ifdef CONFIG_ESP_HTTPS_OTA_DECRYPT_CB
    self->decrypt_handle = NULL;
    esp_decrypt_cfg_t decrypt_cfg = {};
    decrypt_cfg.rsa_priv_key = rsa_private_pem_start;
    decrypt_cfg.rsa_priv_key_len = rsa_private_pem_end - rsa_private_pem_start;

    self->decrypt_handle = esp_encrypted_img_decrypt_start(&decrypt_cfg);
    if (!self->decrypt_handle)
    {
        ESP_LOGI(TAG, "Failed to start decrypt");
        base->status.error = ESP_ERR_OTA_VALIDATE_FAILED;
        ciot_ota_task_fatal_error(self);
    }
#endif // CONFIG_ESP_HTTPS_OTA_DECRYPT_CB

    esp_https_ota_config_t ota_config = {
        .http_config = &http_client_config,
#ifdef CONFIG_ESP_HTTPS_OTA_DECRYPT_CB
        .decrypt_cb = ciot_ota_decrypt_cb,
        .decrypt_user_ctx = (void *)self,
        .enc_img_header_size = esp_encrypted_img_get_header_size(),
#endif
    };

    self->handle = NULL;
    base->status.error = esp_https_ota_begin(&ota_config, &self->handle);
    if (base->status.error != ESP_OK)
    {
        ESP_LOGE(TAG, "ESP HTTPS OTA Begin failed!");
        ciot_iface_send_event_type(&base->iface, CIOT_EVENT_TYPE_ERROR);
        ciot_ota_task_fatal_error(self);
    }

    ciot_iface_send_event_type(&base->iface, CIOT_EVENT_TYPE_STARTED);

    while (true)
    {
        esp_err_t error = esp_https_ota_perform(self->handle);
        if (error != ESP_ERR_HTTPS_OTA_IN_PROGRESS)
        {
            base->status.error = error;
            break;
        }
    }

    if (!esp_https_ota_is_complete_data_received(self->handle))
    {
        ESP_LOGE(TAG, "Complete data was not received.");
    }

#ifdef CONFIG_ESP_HTTPS_OTA_DECRYPT_CB
    if (self->decrypt_handle != NULL)
    {
        base->status.error = esp_encrypted_img_decrypt_end(self->decrypt_handle);
        if (base->status.error != ESP_OK)
        {
            esp_https_ota_abort(self->handle);
            ESP_LOGE(TAG, "upgrade failed");
            ciot_ota_task_fatal_error(self);
            return;
        }
    }
#endif // CONFIG_ESP_HTTPS_OTA_DECRYPT_CB
    base->status.error = esp_https_ota_finish(self->handle);

    if (base->status.error == ESP_OK && base->cfg.restart)
    {
        ESP_LOGI(TAG, "Prepare to restart system!");
        vTaskDelay(pdMS_TO_TICKS(1000));
        esp_restart();
    }

    if (base->status.error != ESP_OK)
    {
        ESP_LOGE(TAG, "ESP_HTTPS_OTA upgrade failed %ld", base->status.error);
        ciot_ota_task_fatal_error(self);
    }

    (void)vTaskDelete(NULL);
}

static void ciot_ota_task_advanced(void *pvParameters)
{
    ciot_ota_t self = (ciot_ota_t)pvParameters;
    ciot_ota_base_t *base = &self->base;

    ESP_LOGI(TAG, "OTA advanced application start.");
    ESP_LOGI(TAG, "cfg: url:%s", base->cfg.url);

    ciot_iface_send_event_type(&base->iface, CIOT_EVENT_TYPE_STARTED);

    CIOT_LOGI(TAG, "Finding specified partition");
    const esp_partition_t *partition = esp_partition_find_first(self->partition_type, self->partition_subtype, NULL);
    if (partition == NULL)
    {
        CIOT_LOGE(TAG, "Specified partition not found.");
        base->status.error = CIOT_ERR_NOT_FOUND;
        ciot_ota_task_fatal_error(self);
        return;
    }

    CIOT_LOGI(TAG, "Partition found:");
    CIOT_LOGI(TAG, "label: %s", partition->label);
    CIOT_LOGI(TAG, "address: %x", (unsigned int)partition->address);
    CIOT_LOGI(TAG, "size: %x", (unsigned int)partition->size);

    CIOT_LOGI(TAG, "Erasing partition");
    base->status.state = CIOT_OTA_STATE_INIT;
    base->status.error = esp_partition_erase_range(partition, 0, partition->size);
    if (base->status.error != ESP_OK)
    {
        CIOT_LOGE(TAG, "Error erasing parititon: %s", esp_err_to_name(base->status.error));
        base->status.error = CIOT_ERR_ERASING;
        ciot_ota_task_fatal_error(self);
        return;
    }

    esp_http_client_config_t http_client_config = {
        .url = (const char *)base->cfg.url,
#ifdef CONFIG_MBEDTLS_CERTIFICATE_BUNDLE
        .crt_bundle_attach = esp_crt_bundle_attach
#endif
    };
    esp_http_client_handle_t client = esp_http_client_init(&http_client_config);

    if (client == NULL)
    {
        CIOT_LOGE(TAG, "Error initializing http client");
        base->status.error = CIOT_ERR_FAIL;
        ciot_ota_task_fatal_error(self);
        return;
    }

    base->status.error = esp_http_client_open(client, 0);
    if (base->status.error != ESP_OK)
    {
        CIOT_LOGE(TAG, "Error openning http client: %s", esp_err_to_name(base->status.error));
        base->status.error = CIOT_ERR_CONNECTION;
        ciot_ota_task_fatal_error(self);
        return;
    }

    esp_http_client_fetch_headers(client);
    base->status.image_written = 0;
    base->status.image_size = esp_http_client_get_content_length(client);
    if (base->status.image_size > partition->size)
    {
        CIOT_LOGE(TAG, "Incorrect partition size. Expected: %u Current: %u", (unsigned int)base->status.image_size, (unsigned int)partition->size);
        base->status.error = CIOT_ERR_INVALID_SIZE;
        ciot_ota_task_fatal_error(self);
    }

    char buf[256];
    base->status.state = CIOT_OTA_STATE_CONNECTED;
    ciot_iface_send_event_type(&base->iface, CIOT_EVENT_TYPE_INTERNAL);

    while (true)
    {
        int data_read = esp_http_client_read(client, buf, sizeof(buf));
        if (data_read < 0)
        {
            CIOT_LOGE(TAG, "Error reading data from client");
            base->status.error = CIOT_ERR_READING;
            ciot_ota_task_fatal_error(self);
        }
        else if (data_read > 0)
        {
            CIOT_LOGI(TAG, "Writing [%x]", (unsigned int)base->status.image_written);
            base->status.error = esp_partition_write(partition, base->status.image_written, buf, data_read);
            if (base->status.error != ESP_OK)
            {
                CIOT_LOGI(TAG, "Error writing data to partition: %s", esp_err_to_name(base->status.error));
                base->status.error = CIOT_ERR_WRITING;
                ciot_ota_task_fatal_error(self);
            }
            else
            {
                base->status.image_written += data_read;
                base->status.state = CIOT_OTA_STATE_FLASHING;
                ciot_iface_send_event_type(&base->iface, CIOT_EVENT_TYPE_INTERNAL);
            }
        }
        else if (data_read == 0)
        {
            CIOT_LOGI(TAG, "Connection closed, all data received");
            break;
        }

        CIOT_LOGI(TAG, "Total Write binary data length : %u", (unsigned int)base->status.image_written);
    }

    base->status.state = CIOT_OTA_STATE_STATE_DONE;
    ciot_iface_send_event_type(&base->iface, CIOT_EVENT_TYPE_INTERNAL);

    if (base->cfg.restart)
    {
        ESP_LOGI(TAG, "Prepare to restart system!");
        vTaskDelay(pdMS_TO_TICKS(1000));
        esp_restart();
    }

    (void)vTaskDelete(NULL);
}

static void __attribute__((noreturn)) ciot_ota_task_fatal_error(ciot_ota_t self)
{
    ESP_LOGE(TAG, "Exiting task due to fatal error...");

    ciot_ota_base_t *base = &self->base;

    base->status.state = CIOT_OTA_STATE_ERROR;
    ciot_iface_send_event_type(&base->iface, CIOT_EVENT_TYPE_ERROR);

    (void)vTaskDelete(NULL);

    while (1)
    {
        ;
    }
}

static void ciot_ota_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    ciot_ota_t self = (ciot_ota_t)arg;
    ciot_ota_base_t *base = &self->base;
    ciot_ota_status_t *status = &base->status;

    switch (event_id)
    {
    case ESP_HTTPS_OTA_START:
        ESP_LOGI(TAG, "ESP_HTTPS_OTA_START");
        status->state = CIOT_OTA_STATE_START;
        ciot_iface_send_event_type(&base->iface, CIOT_EVENT_TYPE_STARTED);
        break;
    case ESP_HTTPS_OTA_CONNECTED:
        ESP_LOGI(TAG, "ESP_HTTPS_OTA_CONNECTED");
        status->state = CIOT_OTA_STATE_CONNECTED;
        ciot_iface_send_event_type(&base->iface, CIOT_EVENT_TYPE_INTERNAL);
        break;
    case ESP_HTTPS_OTA_GET_IMG_DESC:
        ESP_LOGI(TAG, "ESP_HTTPS_OTA_GET_IMG_DESC");
        status->state = CIOT_OTA_STATE_CHECKING_DATA;
        ciot_iface_send_event_type(&base->iface, CIOT_EVENT_TYPE_INTERNAL);
        break;
    case ESP_HTTPS_OTA_VERIFY_CHIP_ID:
        ESP_LOGI(TAG, "ESP_HTTPS_OTA_VERIFY_CHIP_ID");
        status->state = CIOT_OTA_STATE_CHECKING_DATA;
        ciot_iface_send_event_type(&base->iface, CIOT_EVENT_TYPE_INTERNAL);
        break;
    case ESP_HTTPS_OTA_DECRYPT_CB:
        ESP_LOGD(TAG, "ESP_HTTPS_OTA_DECRYPT_CB");
        status->state = CIOT_OTA_STATE_DECRYPTING;
        break;
    case ESP_HTTPS_OTA_WRITE_FLASH:
        status->state = CIOT_OTA_STATE_FLASHING;
        status->image_size = esp_https_ota_get_image_size(self->handle);
        status->image_written = esp_https_ota_get_image_len_read(self->handle);
        ciot_iface_send_event_type(&base->iface, CIOT_EVENT_TYPE_INTERNAL);
        ESP_LOGD(TAG, "ESP_HTTPS_OTA_WRITE_FLASH %lu from %lu", status->image_written, status->image_size);
        break;
    case ESP_HTTPS_OTA_UPDATE_BOOT_PARTITION:
        ESP_LOGI(TAG, "ESP_HTTPS_OTA_UPDATE_BOOT_PARTITION");
        status->state = CIOT_OTA_STATE_UPDATE_BOOT_PARTITION;
        ciot_iface_send_event_type(&base->iface, CIOT_EVENT_TYPE_INTERNAL);
        break;
    case ESP_HTTPS_OTA_FINISH:
        ESP_LOGI(TAG, "ESP_HTTPS_OTA_FINISH");
        status->state = CIOT_OTA_STATE_STATE_DONE;
        ciot_iface_send_event_type(&base->iface, CIOT_EVENT_TYPE_DONE);
        break;
    case ESP_HTTPS_OTA_ABORT:
        ESP_LOGI(TAG, "ESP_HTTPS_OTA_ABORT");
        status->state = CIOT_OTA_STATE_ERROR;
        ciot_iface_send_event_type(&base->iface, CIOT_EVENT_TYPE_ERROR);
        break;
    default:
        break;
    }
}

#ifdef CONFIG_ESP_HTTPS_OTA_DECRYPT_CB
static esp_err_t ciot_ota_decrypt_cb(decrypt_cb_arg_t *args, void *user_ctx)
{
    if (args == NULL || user_ctx == NULL)
    {
        ESP_LOGE(TAG, "ciot_ota_decrypt_cb: Invalid argument");
        return ESP_ERR_INVALID_ARG;
    }
    esp_err_t err;
    pre_enc_decrypt_arg_t pargs = {};
    ciot_ota_t self = (ciot_ota_t)user_ctx;
    ciot_ota_base_t *base = &self->base;
    pargs.data_in = args->data_in;
    pargs.data_in_len = args->data_in_len;
    err = esp_encrypted_img_decrypt_data(self->decrypt_handle, &pargs);
    if (err != ESP_OK && err != ESP_ERR_NOT_FINISHED)
    {
        CIOT_LOGE(TAG, "Decrypt data failed: %s", esp_err_to_name(err));
        return err;
    }
    static bool is_image_verified = false;
    if (pargs.data_out_len > 0)
    {
        args->data_out = pargs.data_out;
        args->data_out_len = pargs.data_out_len;
        if (!is_image_verified)
        {
            is_image_verified = true;
            const int app_desc_offset = sizeof(esp_image_header_t) + sizeof(esp_image_segment_header_t);
            // It is unlikely to not have App Descriptor available in first iteration of decrypt callback.
            assert(args->data_out_len >= app_desc_offset + sizeof(esp_app_desc_t));
            esp_app_desc_t *app_info = (esp_app_desc_t *)&args->data_out[app_desc_offset];
            err = ciot_ota_validate_image_header(app_info, base->cfg.force);
            if (err != ESP_OK)
            {
                CIOT_LOGE(TAG, "Image header validation failed: %s", esp_err_to_name(err));
                free(pargs.data_out);
            }
            return err;
        }
    }
    else
    {
        args->data_out_len = 0;
    }

    return ESP_OK;
}
#endif

#ifdef CONFIG_ESP_HTTPS_OTA_DECRYPT_CB
static esp_err_t ciot_ota_validate_image_header(esp_app_desc_t *new_app_info, bool force)
{
    if (new_app_info == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }

    const esp_partition_t *running = esp_ota_get_running_partition();
    esp_app_desc_t running_app_info;
    if (esp_ota_get_partition_description(running, &running_app_info) == ESP_OK)
    {
        ESP_LOGI(TAG, "Running firmware version: %s", running_app_info.version);
    }

    if (memcmp(new_app_info->version, running_app_info.version, sizeof(new_app_info->version)) == 0 && force == false)
    {
        ESP_LOGW(TAG, "Current running version is the same as a new. We will not continue the update.");
        return ESP_ERR_INVALID_VERSION;
    }
    return ESP_OK;
}
#endif

#endif //! CIOT_CONFIG_FEATURE_OTA == 1