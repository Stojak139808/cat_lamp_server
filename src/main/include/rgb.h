#ifndef RGB_H
#define RGB_H

#include <sys/param.h>
#include "esp_system.h"

#define PWM_PERIOD    (1024u)

#define RED_GPIO    13
#define GREEN_GPIO  12
#define BLUE_GPIO   15

#define RED_ID    0
#define GREEN_ID  1
#define BLUE_ID   2

// pwm pin number
const uint32_t pin_num[3];

// duties table, real_duty = duties[x]/PERIOD
uint32_t rgb_duties[3];

// phase table, delay = (phase[x]/360)*PERIOD
float rgb_phases[3];

void init_pwm(void);
void rbg_mode_on(void);
void rbg_mode_off(void);
void rgb_set_color(uint32_t RGB[3]);

/* Convert hex string to uint32 value */
uint32_t hex_to_int(char *hex);

#endif //RGB_H
