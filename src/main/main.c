/* Simple HTTP Server Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <sys/param.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "protocol_examples_common.h"
#include "nvs.h"
#include "nvs_flash.h"

#include <esp_http_server.h>
#include "driver/pwm.h"

#include "color_picker.h"

static const char *TAG="APP";

#define PWM_PERIOD    (1024u)

// pwm pin number
const uint32_t pin_num[3] = {
    13, //R
    12, //G
    15  //B
};

// duties table, real_duty = duties[x]/PERIOD
uint32_t duties[3] = {
    500, 500, 500
};

// phase table, delay = (phase[x]/360)*PERIOD
float phase[3] = {
    0, 0, 0
};

void init_pwm(){
    pwm_init(PWM_PERIOD, duties, 3, pin_num);
    pwm_set_phases(phase);
    pwm_start();
    ESP_LOGI(TAG, "PWM: ON");

}

uint32_t hex_to_int(char *hex){

    int8_t i, mul = 1;
    uint32_t out = 0;

    for( i = strlen(hex) - 1; i >= 0; i-- ){
        if( hex[i] >= '0' && hex[i] <= '9'){
            out += (hex[i] - '0') * mul;
        } 
        else if( hex[i] >= 'a' && hex[i] <= 'f'){
            out += (hex[i] - 'a' + 10) * mul;
        }
        else if( hex[i] >= 'A' && hex[i] <= 'F'){
            out += (hex[i] - 'A' + 10) * mul;
        }
        else{
            /* got some hot garbage instead of hex */
            return 0;
        }
        mul *= 16u;
    }
    ESP_LOGI(TAG, "GOT HEX: %d", out);
    return out;
}

TaskHandle_t xRGB_shifter = NULL;
#define RGB_STEP 10

void vRGB_shifter(){

    const TickType_t xDelay = 10 / portTICK_PERIOD_MS;
    for(;;){
        if(duties[0] > 0 && duties[2] == 0){
            duties[0] -= RGB_STEP;
            duties[1] += RGB_STEP;
        }
        if(duties[1] > 0 && duties[0] == 0){
            duties[1] -= RGB_STEP;
            duties[2] += RGB_STEP;
        }
        if(duties[2] > 0 && duties[1] == 0){
            duties[0] += RGB_STEP;
            duties[2] -= RGB_STEP;
        }
        pwm_set_duties(duties);
        pwm_start();
        vTaskDelay( xDelay );
    }
}

void rbg_mode_on(){
    ESP_LOGI(TAG, "turning on rgb mode");
    if( xRGB_shifter == NULL ){
        duties[0] = 1000;
        duties[1] = 0;
        duties[2] = 0;
        pwm_set_duties(duties);
        pwm_start();
        xTaskCreate(vRGB_shifter,
                    "RGB_shifter",
                    configMINIMAL_STACK_SIZE + 64,
                    (void*)1u,
                    20,
                    &xRGB_shifter
                    );
    }
}

void rbg_mode_off(){
    ESP_LOGI(TAG, "turning off rgb mode");
    vTaskDelete( xRGB_shifter );
    xRGB_shifter = NULL;
}

esp_err_t change_colors_from_uri(char* color){
    /* expecting color = "%23RRGGBB" */
    uint8_t i;
    char tmp[] = {'0','0','\0'};

    for( i = 0; i < 3; i++ ){
        memcpy(tmp, color + 3 + i*2, 2);
        duties[i] = hex_to_int(tmp) * (PWM_PERIOD/256u);
    }
    pwm_set_duties(duties);
    pwm_start();
    ESP_LOGI(TAG, "setting pwm: %d %d %d", duties[0], duties[1], duties[2]);
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

httpd_uri_t index_page = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = index_page_handler,
    .user_ctx  = color_picker_h
};

httpd_handle_t start_webserver(void)
{
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

void stop_webserver(httpd_handle_t server)
{
    // Stop the httpd server
    httpd_stop(server);
}

static httpd_handle_t server = NULL;

static void disconnect_handler(void* arg, esp_event_base_t event_base, 
                               int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server) {
        ESP_LOGI(TAG, "Stopping webserver");
        stop_webserver(*server);
        *server = NULL;
    }
}

static void connect_handler(void* arg, esp_event_base_t event_base, 
                            int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server == NULL) {
        ESP_LOGI(TAG, "Starting webserver");
        *server = start_webserver();
    }
}

void app_main()
{
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());


    ESP_ERROR_CHECK(example_connect());

    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &connect_handler, &server));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &disconnect_handler, &server));

    init_pwm();

    server = start_webserver();
}
