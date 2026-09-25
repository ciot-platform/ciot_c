/**
 * @file ciot_http_server.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-12-11
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "ciot_config.h"

#if CIOT_CONFIG_FEATURE_HTTP_SERVER == 1 && defined(CIOT_PLATFORM_ESP32)

#include "ciot_http_server.h"
#include "esp_http_server.h"
#include "freertos/event_groups.h"
#include "lwip/sockets.h"
#include "lwip/inet.h"

#define CIOT_HTTP_SERVER_RESP_READY_BIT BIT0

#ifndef CIOT_CONFIG_HTTP_SERVER_TIMEOUT_MS
#define CIOT_CONFIG_HTTP_SERVER_TIMEOUT_MS 30000
#endif

#ifndef CIOT_CONFIG_HTTP_SERVER_MAX_RESP_SIZE
#define CIOT_CONFIG_HTTP_SERVER_MAX_RESP_SIZE 512
#endif

struct ciot_http_server
{
    ciot_http_server_base_t base;
    httpd_handle_t handle;
    httpd_req_t *req;
    uint8_t resp[CIOT_CONFIG_HTTP_SERVER_MAX_RESP_SIZE];
    uint16_t resp_size;
    bool resp_ready;
    uint16_t resp_status_code;
    char resp_content_type[48];
    ciot_http_server_header_t resp_headers[CIOT_HTTP_SERVER_MAX_RESPONSE_HEADERS];
    char resp_header_names[CIOT_HTTP_SERVER_MAX_RESPONSE_HEADERS][48];
    char resp_header_values[CIOT_HTTP_SERVER_MAX_RESPONSE_HEADERS][96];
    size_t resp_headers_count;
    EventGroupHandle_t event_group;
};

static const char *TAG = "ciot_http_server";

static ciot_err_t ciot_https_register_routes(ciot_http_server_t self);
static esp_err_t ciot_http_server_api_handler(httpd_req_t *req);
static esp_err_t ciot_http_server_file_handler(httpd_req_t *req);
static const char *get_mime_type(const char *filename);
static esp_err_t ciot_http_server_custom_api_handler(httpd_req_t *req);
static esp_err_t ciot_http_server_upload_handler(httpd_req_t *req);
static httpd_method_t http_method_from_str(const char *method);
static const char *http_method_to_str(httpd_method_t method);
static const char *http_status_to_str(uint16_t status_code);
static bool ciot_http_server_get_header(const ciot_http_server_request_t *request, const char *name, char *value, size_t value_size);
static void ciot_http_server_prepare_request(httpd_req_t *req, ciot_http_server_request_t *request);
static bool ciot_http_server_intercept(httpd_req_t *req, ciot_http_server_t self);
static esp_err_t ciot_http_server_write_response(httpd_req_t *req, const ciot_http_server_response_t *response);
static esp_err_t ciot_http_server_options_handler(httpd_req_t *req);
static int ciot_http_server_read_body(httpd_req_t *req, uint8_t *data, size_t capacity);
static esp_err_t ciot_http_server_send_payload_too_large(httpd_req_t *req);

ciot_http_server_t ciot_http_server_new(void *handle)
{
    ciot_http_server_t self = calloc(1, sizeof(struct ciot_http_server));
    self->event_group = xEventGroupCreate();
    ciot_http_server_init(self);
    return self;
}

ciot_err_t ciot_http_server_start(ciot_http_server_t self, ciot_http_server_cfg_t *cfg)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(cfg);

    ciot_http_server_base_t *base = &self->base;

    base->cfg = *cfg;

    httpd_config_t httpd_config = HTTPD_DEFAULT_CONFIG();
    httpd_config.server_port = cfg->port;
    httpd_config.max_uri_handlers = 5;
    httpd_config.uri_match_fn = httpd_uri_match_wildcard;
    httpd_config.stack_size = 8192;

    esp_err_t err_code = httpd_start(&self->handle, &httpd_config);
    if (err_code == ESP_OK)
    {
        ESP_LOGI(TAG, "Server Started on port %lu", (unsigned long)cfg->port);
        base->cfg = *cfg;
        ciot_https_register_routes(self);
        ciot_iface_send_event_type(&base->iface, CIOT_EVENT_TYPE_STARTED);
    }

    return err_code;
}

ciot_err_t ciot_http_server_stop(ciot_http_server_t self)
{
    CIOT_ERR_NULL_CHECK(self);
    return httpd_stop(self->handle);
}

ciot_err_t ciot_http_server_send_bytes(ciot_http_server_t self, uint8_t *data, int size)
{
    ciot_http_server_response_t response = {
        .status_code = 200,
        .content_type = "application/octet-stream",
        .body = data,
        .body_size = size,
    };
    return ciot_http_server_send_response(self, &response);
}

ciot_err_t ciot_http_server_send_response(ciot_http_server_t self, const ciot_http_server_response_t *response)
{
    CIOT_ERR_NULL_CHECK(self);
    CIOT_ERR_NULL_CHECK(response);
    if(response->body_size > sizeof(self->resp))
    {
        CIOT_LOGW(TAG, "Response size %d exceeds buffer size %d, truncating", (int)response->body_size, (int)sizeof(self->resp));
        return CIOT_ERR_OVERFLOW;
    }
    if (response->body != NULL && response->body_size > 0) memcpy(self->resp, response->body, response->body_size);
    self->resp_size = response->body_size;
    self->resp_ready = true;
    self->resp_status_code = response->status_code == 0 ? 200 : response->status_code;
    snprintf(self->resp_content_type, sizeof(self->resp_content_type), "%s", response->content_type != NULL ? response->content_type : "application/octet-stream");
    self->resp_headers_count = response->headers_count < CIOT_HTTP_SERVER_MAX_RESPONSE_HEADERS ? response->headers_count : CIOT_HTTP_SERVER_MAX_RESPONSE_HEADERS;
    for (size_t i = 0; i < self->resp_headers_count; i++)
    {
        snprintf(self->resp_header_names[i], sizeof(self->resp_header_names[i]), "%s", response->headers[i].name != NULL ? response->headers[i].name : "");
        snprintf(self->resp_header_values[i], sizeof(self->resp_header_values[i]), "%s", response->headers[i].value != NULL ? response->headers[i].value : "");
        self->resp_headers[i].name = self->resp_header_names[i];
        self->resp_headers[i].value = self->resp_header_values[i];
    }
    xEventGroupSetBits(self->event_group, CIOT_HTTP_SERVER_RESP_READY_BIT);
    return CIOT_ERR_OK;
}

static ciot_err_t ciot_https_register_routes(ciot_http_server_t self)
{
    CIOT_LOGI(TAG, "Registering route: %s", self->base.cfg.route);
    httpd_uri_t post_uri = {
        .uri = self->base.cfg.route,
        .handler = ciot_http_server_api_handler,
        .method = HTTP_POST,
        .user_ctx = self,
    };
    esp_err_t err = httpd_register_uri_handler(self->handle, &post_uri);
    if (err)
    {
        CIOT_LOGE(TAG, "Register uri error: %s", esp_err_to_name(err));
        return CIOT_ERR_FAIL;
    }

    if (self->base.custom_api.enabled && self->base.custom_api.handler != NULL)
    {
        CIOT_LOGI(TAG, "Registering route: %s", self->base.custom_api.uri);
        httpd_uri_t post_uri = {
            .uri = self->base.custom_api.uri,
            .handler = ciot_http_server_custom_api_handler,
            .method = HTTP_ANY,
            .user_ctx = self,
        };
        esp_err_t err = httpd_register_uri_handler(self->handle, &post_uri);
        if (err)
        {
            CIOT_LOGE(TAG, "Register uri error: %s", esp_err_to_name(err));
            return CIOT_ERR_FAIL;
        }
    }

    if (self->base.upload_api.enabled && self->base.upload_api.handler != NULL)
    {
        CIOT_LOGI(TAG, "Registering route: %s", self->base.upload_api.uri);
        httpd_uri_t upload_uri = {
            .uri = self->base.upload_api.uri,
            .handler = ciot_http_server_upload_handler,
            .method = http_method_from_str(self->base.upload_api.method),
            .user_ctx = self,
        };
        esp_err_t err = httpd_register_uri_handler(self->handle, &upload_uri);
        if (err)
        {
            CIOT_LOGE(TAG, "Register uri error: %s", esp_err_to_name(err));
            return CIOT_ERR_FAIL;
        }
    }

    CIOT_LOGI(TAG, "Registering route: /*");
    httpd_uri_t file_uri = {
        .uri = "/*",
        .method = HTTP_GET,
        .handler = ciot_http_server_file_handler,
        .user_ctx = self};
    err = httpd_register_uri_handler(self->handle, &file_uri);
    if (err)
    {
        CIOT_LOGE(TAG, "Register uri error: %s", esp_err_to_name(err));
        return CIOT_ERR_FAIL;
    }

    httpd_uri_t options_uri = {
        .uri = "/*",
        .method = HTTP_OPTIONS,
        .handler = ciot_http_server_options_handler,
        .user_ctx = self};
    err = httpd_register_uri_handler(self->handle, &options_uri);
    if (err)
    {
        CIOT_LOGE(TAG, "Register OPTIONS error: %s", esp_err_to_name(err));
        return CIOT_ERR_FAIL;
    }
    return CIOT_ERR_OK;
}

static esp_err_t ciot_http_server_api_handler(httpd_req_t *req)
{
    ciot_http_server_t self = (ciot_http_server_t)req->user_ctx;
    uint8_t req_data[CIOT_CONFIG_MSG_SIZE] = {0};

    if (self == NULL)
    {
        CIOT_LOGE(TAG, "Null context");
        return ESP_FAIL;
    }

    if (ciot_http_server_intercept(req, self)) return ESP_OK;

    self->resp_ready = false;
    self->resp_size = 0;
    xEventGroupClearBits(self->event_group, CIOT_HTTP_SERVER_RESP_READY_BIT);

    int recv_size = req->content_len;
    if (recv_size > (int)sizeof(req_data))
    {
        CIOT_LOGW(TAG, "HTTP payload rejected: %d > %d", recv_size, (int)sizeof(req_data));
        return ciot_http_server_send_payload_too_large(req);
    }

    int bytes_read = ciot_http_server_read_body(req, req_data, sizeof(req_data));
    if (bytes_read < 0)
    {
        CIOT_LOGE(TAG, "Failed to read request body: %d", bytes_read);
        return bytes_read;
    }

    ciot_iface_send_event_data(&self->base.iface, CIOT_EVENT_TYPE_MSG, req_data, bytes_read);

    if (!self->resp_ready)
    {
        xEventGroupWaitBits(
            self->event_group,
            CIOT_HTTP_SERVER_RESP_READY_BIT,
            pdTRUE,
            pdFALSE,
            pdMS_TO_TICKS(CIOT_CONFIG_HTTP_SERVER_TIMEOUT_MS));
    }

    if (self->resp_ready)
    {
        CIOT_LOGI(TAG, "Resp OK");
        ciot_http_server_response_t response = {
            .status_code = self->resp_status_code,
            .content_type = self->resp_content_type,
            .body = self->resp,
            .body_size = self->resp_size,
            .headers_count = self->resp_headers_count,
        };
        memcpy(response.headers, self->resp_headers, response.headers_count * sizeof(response.headers[0]));
        ciot_http_server_write_response(req, &response);
        self->resp_size = 0;
        self->resp_ready = false;
    }
    else
    {
        CIOT_LOGW(TAG, "Timeout waiting for response");
        httpd_resp_send_408(req);
    }

    return CIOT_ERR_OK;
}

static esp_err_t ciot_http_server_file_handler(httpd_req_t *req)
{
    ciot_http_server_t self = (ciot_http_server_t)req->user_ctx;

#ifdef CIOT_CONFIG_HTTP_SERVER_ALLOW_ORIGIN
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", CIOT_CONFIG_HTTP_SERVER_ALLOW_ORIGIN);
#endif

    /* req->uri includes the query string (e.g. "/?username=wesley"), unlike
     * the mongoose backend's hm->uri - strip it before any path matching. */
    char uri_path[36];
    const char *query = strchr(req->uri, '?');
    size_t uri_len = query ? (size_t)(query - req->uri) : strlen(req->uri);
    if (uri_len >= sizeof(uri_path))
        uri_len = sizeof(uri_path) - 1;
    memcpy(uri_path, req->uri, uri_len);
    uri_path[uri_len] = '\0';

    if (((strcmp(uri_path, "/") == 0) || (strcmp(uri_path, "/index.html") == 0)) && self->base.homepage.size > 0)
    {
        if (self->base.homepage.gz)
        {
            CIOT_LOGI(TAG, "Serving embed gzip");
            httpd_resp_set_hdr(req, "Content-Encoding", "gzip");
        }
        else
        {
            CIOT_LOGI(TAG, "Serving embed html");
        }
        httpd_resp_send(req, (const char *)self->base.homepage.data, self->base.homepage.size - 1);
        return ESP_OK;
    }

    char filepath[36];
    snprintf(filepath, sizeof(filepath), "/fs%.*s", (int)(sizeof(filepath) - 4), uri_path);

    // Verificar se a URI é "/", servir "index.html"
    if (strcmp(uri_path, "/") == 0)
    {
        CIOT_LOGI(TAG, "Serving fs html");
        strcpy(filepath, "/fs/index.html");
    }

    // Buffer para armazenar o valor do cabeçalho "Accept-Encoding"
    char accept_encoding[16] = {0};
    httpd_req_get_hdr_value_str(req, "Accept-Encoding", accept_encoding, sizeof(accept_encoding));

    // Verificar suporte a gzip
    bool supports_gzip = strstr(accept_encoding, "gzip") != NULL;

    // Modificar o caminho para tentar servir o arquivo gzip, se suportado
    if (supports_gzip)
    {
        char gz_filepath[39];
        snprintf(gz_filepath, sizeof(gz_filepath), "%s.gz", filepath);
        FILE *gz_file = fopen(gz_filepath, "r");
        if (gz_file)
        {
            ESP_LOGI(TAG, "Serving gzip file: %s", gz_filepath);
            httpd_resp_set_hdr(req, "Content-Encoding", "gzip");
            strcpy(filepath, gz_filepath); // Atualizar para o arquivo gzip
            fclose(gz_file);               // Fechar para reabrir na leitura abaixo
        }
    }

    // Abrir o arquivo solicitado
    FILE *file = fopen(filepath, "r");
    if (!file)
    {
        ESP_LOGE(TAG, "Failed to open file: %s", filepath);
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }

    // Configurar o cabeçalho com o tipo de conteúdo adequado
    httpd_resp_set_type(req, get_mime_type(filepath));

    // Ler o arquivo e enviar seu conteúdo
    char buffer[128];
    size_t read_bytes;
    while ((read_bytes = fread(buffer, 1, sizeof(buffer), file)) > 0)
    {
        if (httpd_resp_send_chunk(req, buffer, read_bytes) != ESP_OK)
        {
            fclose(file);
            return ESP_FAIL;
        }
    }

    // Enviar a resposta final
    httpd_resp_send_chunk(req, NULL, 0);

    fclose(file);
    return ESP_OK;
}

static const char *get_mime_type(const char *filename)
{
    if (strstr(filename, ".html"))
        return "text/html";
    if (strstr(filename, ".css"))
        return "text/css";
    if (strstr(filename, ".js"))
        return "application/javascript";
    if (strstr(filename, ".png"))
        return "image/png";
    if (strstr(filename, ".jpg"))
        return "image/jpeg";
    if (strstr(filename, ".ico"))
        return "image/x-icon";
    if (strstr(filename, ".svg"))
        return "image/svg+xml";
    return "text/plain";
}

static esp_err_t ciot_http_server_custom_api_handler(httpd_req_t *req)
{
    ciot_http_server_t self = (ciot_http_server_t)req->user_ctx;
    uint8_t req_data[CIOT_CONFIG_MSG_SIZE] = {0};
    int recv_size = req->content_len;

    if (self == NULL)
    {
        CIOT_LOGE(TAG, "Null context");
        return ESP_FAIL;
    }

    if (ciot_http_server_intercept(req, self)) return ESP_OK;

    self->resp_ready = false;
    self->resp_size = 0;
    xEventGroupClearBits(self->event_group, CIOT_HTTP_SERVER_RESP_READY_BIT);

    if (recv_size > (int)sizeof(req_data))
    {
        CIOT_LOGW(TAG, "Custom API payload rejected: %d > %d", recv_size, (int)sizeof(req_data));
        return ciot_http_server_send_payload_too_large(req);
    }

    int bytes_read = ciot_http_server_read_body(req, req_data, sizeof(req_data));
    if (bytes_read < 0)
    {
        CIOT_LOGE(TAG, "Failed to read custom API body: %d", bytes_read);
        return bytes_read;
    }

    switch (req->method)
    {
    case HTTP_DELETE:
        self->base.custom_api.handler(self, req->uri, strlen(req->uri), "DELETE", req_data, bytes_read, self->base.custom_api.args);
        break;
    case HTTP_GET:
        self->base.custom_api.handler(self, req->uri, strlen(req->uri), "GET", req_data, bytes_read, self->base.custom_api.args);
        break;
    case HTTP_HEAD:
        self->base.custom_api.handler(self, req->uri, strlen(req->uri), "HEAD", req_data, bytes_read, self->base.custom_api.args);
        break;
    case HTTP_POST:
        self->base.custom_api.handler(self, req->uri, strlen(req->uri), "POST", req_data, bytes_read, self->base.custom_api.args);
        break;
    case HTTP_PUT:
        self->base.custom_api.handler(self, req->uri, strlen(req->uri), "PUT", req_data, bytes_read, self->base.custom_api.args);
        break;
    case HTTP_CONNECT:
        self->base.custom_api.handler(self, req->uri, strlen(req->uri), "CONNECT", req_data, bytes_read, self->base.custom_api.args);
        break;
    default:
        self->base.custom_api.handler(self, req->uri, strlen(req->uri), "UNKNOWN", req_data, bytes_read, self->base.custom_api.args);
        break;
    }

    if (!self->resp_ready)
    {
        xEventGroupWaitBits(
            self->event_group,
            CIOT_HTTP_SERVER_RESP_READY_BIT,
            pdTRUE,
            pdFALSE,
            pdMS_TO_TICKS(CIOT_CONFIG_HTTP_SERVER_TIMEOUT_MS));
    }

    if (self->resp_ready)
    {
        CIOT_LOGI(TAG, "Resp OK");
        ciot_http_server_response_t response = {
            .status_code = self->resp_status_code,
            .content_type = self->resp_content_type,
            .body = self->resp,
            .body_size = self->resp_size,
            .headers_count = self->resp_headers_count,
        };
        memcpy(response.headers, self->resp_headers, response.headers_count * sizeof(response.headers[0]));
        ciot_http_server_write_response(req, &response);
        self->resp_size = 0;
        self->resp_ready = false;
    }
    else
    {
        CIOT_LOGW(TAG, "Timeout waiting for response");
        httpd_resp_send_500(req);
    }

    return CIOT_ERR_OK;
}

static httpd_method_t http_method_from_str(const char *method)
{
    if (method == NULL)
    {
        return HTTP_POST;
    }
    if (strcmp(method, "GET") == 0) return HTTP_GET;
    if (strcmp(method, "PUT") == 0) return HTTP_PUT;
    if (strcmp(method, "DELETE") == 0) return HTTP_DELETE;
    if (strcmp(method, "PATCH") == 0) return HTTP_PATCH;
    if (strcmp(method, "HEAD") == 0) return HTTP_HEAD;
    if (strcmp(method, "OPTIONS") == 0) return HTTP_OPTIONS;
    return HTTP_POST;
}

static esp_err_t ciot_http_server_upload_handler(httpd_req_t *req)
{
    ciot_http_server_t self = (ciot_http_server_t)req->user_ctx;

    if (self == NULL)
    {
        CIOT_LOGE(TAG, "Null context");
        return ESP_FAIL;
    }

    if (ciot_http_server_intercept(req, self)) return ESP_OK;

    ciot_http_server_upload_api_t *upload_api = &self->base.upload_api;

    size_t remaining = req->content_len;
    if (remaining == 0 || remaining > upload_api->max_size)
    {
        CIOT_LOGW(TAG, "Upload rejected: content_len %d, max %d", (int)remaining, (int)upload_api->max_size);
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid content length");
        return ESP_OK;
    }

    uint8_t *buf = malloc(remaining);
    if (buf == NULL)
    {
        CIOT_LOGE(TAG, "Failed to allocate %d bytes for upload", (int)remaining);
        httpd_resp_send_500(req);
        return ESP_OK;
    }

    size_t received = 0;
    while (received < remaining)
    {
        int ret = httpd_req_recv(req, (char *)(buf + received), remaining - received);
        if (ret <= 0)
        {
            CIOT_LOGE(TAG, "Failed to read upload body: %d", ret);
            free(buf);
            if (ret == HTTPD_SOCK_ERR_TIMEOUT)
            {
                httpd_resp_send_408(req);
            }
            else
            {
                httpd_resp_send_500(req);
            }
            return ESP_OK;
        }
        received += ret;
    }

    ciot_err_t err = upload_api->handler(upload_api->args, buf, received);
    free(buf);

    if (err == CIOT_ERR_OK)
    {
        httpd_resp_set_status(req, HTTPD_200);
        httpd_resp_send(req, NULL, 0);
    }
    else
    {
        CIOT_LOGW(TAG, "Upload handler rejected payload: %s", ciot_err_to_message(err));
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, ciot_err_to_message(err));
    }

    return ESP_OK;
}

static const char *http_method_to_str(httpd_method_t method)
{
    switch (method)
    {
    case HTTP_GET: return "GET";
    case HTTP_POST: return "POST";
    case HTTP_PUT: return "PUT";
    case HTTP_DELETE: return "DELETE";
    case HTTP_PATCH: return "PATCH";
    case HTTP_HEAD: return "HEAD";
    case HTTP_OPTIONS: return "OPTIONS";
    default: return "UNKNOWN";
    }
}

static const char *http_status_to_str(uint16_t status_code)
{
    switch (status_code)
    {
    case 200: return "200 OK";
    case 201: return "201 Created";
    case 204: return "204 No Content";
    case 400: return "400 Bad Request";
    case 401: return "401 Unauthorized";
    case 403: return "403 Forbidden";
    case 404: return "404 Not Found";
    case 409: return "409 Conflict";
    case 428: return "428 Precondition Required";
    case 429: return "429 Too Many Requests";
    default: return "500 Internal Server Error";
    }
}

static bool ciot_http_server_get_header(const ciot_http_server_request_t *request, const char *name, char *value, size_t value_size)
{
    if (request == NULL || name == NULL || value == NULL || value_size == 0) return false;
    httpd_req_t *req = request->context;
    return httpd_req_get_hdr_value_str(req, name, value, value_size) == ESP_OK;
}

static void ciot_http_server_prepare_request(httpd_req_t *req, ciot_http_server_request_t *request)
{
    memset(request, 0, sizeof(*request));
    request->method = http_method_to_str(req->method);
    request->method_len = strlen(request->method);
    request->uri = req->uri;
    const char *query = strchr(req->uri, '?');
    request->uri_len = query == NULL ? strlen(req->uri) : (size_t)(query - req->uri);
    request->get_header = ciot_http_server_get_header;
    request->context = req;

    int socket_fd = httpd_req_to_sockfd(req);
    struct sockaddr_storage address;
    socklen_t address_len = sizeof(address);
    if (getsockname(socket_fd, (struct sockaddr *)&address, &address_len) == 0 && address.ss_family == AF_INET)
    {
        inet_ntoa_r(((struct sockaddr_in *)&address)->sin_addr, request->local_address, sizeof(request->local_address));
    }
    address_len = sizeof(address);
    if (getpeername(socket_fd, (struct sockaddr *)&address, &address_len) == 0 && address.ss_family == AF_INET)
    {
        inet_ntoa_r(((struct sockaddr_in *)&address)->sin_addr, request->remote_address, sizeof(request->remote_address));
    }
}

static bool ciot_http_server_intercept(httpd_req_t *req, ciot_http_server_t self)
{
    if (self->base.request_interceptor == NULL) return false;
    ciot_http_server_request_t request;
    ciot_http_server_response_t response = {0};
    ciot_http_server_prepare_request(req, &request);
    if (!self->base.request_interceptor(self, &request, &response, self->base.request_interceptor_args)) return false;
    ciot_http_server_write_response(req, &response);
    return true;
}

static esp_err_t ciot_http_server_write_response(httpd_req_t *req, const ciot_http_server_response_t *response)
{
    httpd_resp_set_status(req, http_status_to_str(response->status_code == 0 ? 200 : response->status_code));
    httpd_resp_set_type(req, response->content_type != NULL ? response->content_type : HTTPD_TYPE_OCTET);
#ifdef CIOT_CONFIG_HTTP_SERVER_ALLOW_ORIGIN
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", CIOT_CONFIG_HTTP_SERVER_ALLOW_ORIGIN);
#endif
    for (size_t i = 0; i < response->headers_count && i < CIOT_HTTP_SERVER_MAX_RESPONSE_HEADERS; i++)
    {
        if (response->headers[i].name != NULL && response->headers[i].value != NULL)
            httpd_resp_set_hdr(req, response->headers[i].name, response->headers[i].value);
    }
    return httpd_resp_send(req, (const char *)response->body, response->body_size);
}

static esp_err_t ciot_http_server_options_handler(httpd_req_t *req)
{
    ciot_http_server_t self = (ciot_http_server_t)req->user_ctx;
    if (ciot_http_server_intercept(req, self)) return ESP_OK;
#ifdef CIOT_CONFIG_HTTP_SERVER_ALLOW_ORIGIN
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", CIOT_CONFIG_HTTP_SERVER_ALLOW_ORIGIN);
#endif
    httpd_resp_set_hdr(req, "Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Headers", "Authorization, Content-Type");
    return httpd_resp_send(req, NULL, 0);
}

static int ciot_http_server_read_body(httpd_req_t *req, uint8_t *data, size_t capacity)
{
    if (req->content_len < 0 || (size_t)req->content_len > capacity) return -1;
    size_t received = 0;
    while (received < (size_t)req->content_len)
    {
        int ret = httpd_req_recv(req, (char *)(data + received), req->content_len - received);
        if (ret <= 0) return ret;
        received += (size_t)ret;
    }
    return (int)received;
}

static esp_err_t ciot_http_server_send_payload_too_large(httpd_req_t *req)
{
    httpd_resp_set_status(req, "413 Payload Too Large");
    httpd_resp_set_type(req, "text/plain");
    return httpd_resp_sendstr(req, "Payload too large");
}

#endif //! CIOT_CONFIG_FEATURE_HTTP_SERVER == 1