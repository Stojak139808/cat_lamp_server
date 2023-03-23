#include "include/rgb.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "driver/pwm.h"
#include <string.h>

TaskHandle_t xRGB_shifter = NULL;

#define RGB_STEP 10
static const char *TAG="RGB";

// pwm pin number
const uint32_t pin_num[3] = {
    RED_GPIO,
    GREEN_GPIO,
    BLUE_GPIO
};

// duties table, real_duty = duties[x]/PERIOD
uint32_t rgb_duties[3] = {
    500, 500, 500
};

// phase table, delay = (phase[x]/360)*PERIOD
float rgb_phases[3] = {
    0, 0, 0
};

void vRGB_shifter(void* pvParam){

    const TickType_t xDelay = 10 / portTICK_PERIOD_MS;

    for(;;){
        if(rgb_duties[RED_ID] > 0 && rgb_duties[BLUE_ID] == 0){
            rgb_duties[RED_ID] -= RGB_STEP;
            rgb_duties[GREEN_ID] += RGB_STEP;
        }
        if(rgb_duties[GREEN_ID] > 0 && rgb_duties[RED_ID] == 0){
            rgb_duties[GREEN_ID] -= RGB_STEP;
            rgb_duties[BLUE_ID] += RGB_STEP;
        }
        if(rgb_duties[BLUE_ID] > 0 && rgb_duties[GREEN_ID] == 0){
            rgb_duties[RED_ID] += RGB_STEP;
            rgb_duties[BLUE_ID] -= RGB_STEP;
        }
        pwm_set_duties(rgb_duties);
        pwm_start();
        vTaskDelay( xDelay );
    }
}

void rbg_mode_on(){
    ESP_LOGI(TAG, "turning on rgb mode");
    if( xRGB_shifter == NULL ){
        rgb_duties[RED_ID] = 1000;
        rgb_duties[GREEN_ID] = 0;
        rgb_duties[BLUE_ID] = 0;
        pwm_set_duties(rgb_duties);
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

void init_pwm(){
    pwm_init(PWM_PERIOD, rgb_duties, 3, pin_num);
    pwm_set_phases(rgb_phases);
    pwm_start();
    ESP_LOGI(TAG, "PWM: ON");
}

void rgb_set_color(uint32_t RGB[3]){

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
