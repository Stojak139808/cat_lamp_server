#include <sys/param.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_event.h"
#include <esp_http_server.h>
#include "driver/pwm.h"

#include "include/rgb.h"
#include "include/wifi.h"

/* HTTP pages */
#include "color_picker.h"

static const char *TAG="HTTP";

static httpd_handle_t server = NULL;

esp_err_t index_page_handler(httpd_req_t *req);

httpd_uri_t index_page = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = index_page_handler,
    .user_ctx  = color_picker_h
};

esp_err_t change_colors_from_uri(char* color){

    /* expecting color = "%23RRGGBB" */
    uint8_t i;
    char tmp[] = {'0','0','\0'};
    uint32_t rgb[] = {0, 0, 0};

    for( i = 0; i < 3; i++ ){
        memcpy(tmp, color + 3 + i*2, 2);
        rgb[i] = hex_to_int(tmp) * (PWM_PERIOD/256u);
    }
    rgb_set_color(rgb);
    return ESP_OK;
}

esp_err_t index_page_handler(httpd_req_t *req){

    char*  buf;
    size_t buf_len;
    char param[32];

    /* Read URL query string length and allocate memory for length + 1,
    * extra byte for null termination */
    buf_len = httpd_req_get_url_query_len(req) + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found URL query => %s", buf);
            /* Get value of expected key from query string */

            /* set single color */
            if (httpd_query_key_value(buf, "colorlamp", param, sizeof(param)) == ESP_OK) {
                ESP_LOGI(TAG, "Found URL query parameter => colorlamp=%s", param);
                change_colors_from_uri(param);
            }

            /* go gamer rgb mode */
            if (httpd_query_key_value(buf, "rgbmode", param, sizeof(param)) == ESP_OK) {
                ESP_LOGI(TAG, "Found URL query parameter => rgbmode=%s", param);
                if(*param == '0' ){
                    rbg_mode_off();
                }
                else if(*param == '1' ){
                    rbg_mode_on();
                }
            }
        }
        free(buf);
    }

    /* Send response with custom headers and body set as the
     * string passed in user context*/
    const char* resp_str = (const char*) req->user_ctx;
    httpd_resp_send(req, resp_str, strlen(resp_str));

    return ESP_OK;
}

httpd_handle_t start_webserver(void){
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &index_page);
        return server;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}

void stop_webserver(httpd_handle_t server){
    // Stop the httpd server
    httpd_stop(server);
}

static void disconnect_handler(void* arg, esp_event_base_t event_base, 
                               int32_t event_id, void* event_data){
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server) {
        ESP_LOGI(TAG, "Stopping webserver");
        stop_webserver(*server);
        *server = NULL;
    }
}

static void connect_handler(void* arg, esp_event_base_t event_base, 
                            int32_t event_id, void* event_data){
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server == NULL) {
        ESP_LOGI(TAG, "Starting webserver");
        *server = start_webserver();
    }
}

void init_server(){
    ESP_ERROR_CHECK(wifi_init_sta());
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &connect_handler, &server));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &disconnect_handler, &server));
    server = start_webserver();
}