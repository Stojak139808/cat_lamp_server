/* Simple HTTP Server Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <sys/param.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "include/rgb.h"
#include "include/http.h"

static const char *TAG="MAIN";

void app_main(){

    ESP_LOGI(TAG, "Initializing main.");
    init_pwm();
    init_server();
    ESP_LOGI(TAG, "Initialization done.");

}
