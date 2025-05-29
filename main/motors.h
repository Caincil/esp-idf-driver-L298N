

#ifndef MCPWM_BRUSHED_DC_CONTROL_MOTORS_H
#define MCPWM_BRUSHED_DC_CONTROL_MOTORS_H
#include "driver/gpio.h"

typedef struct MotorConfig{
    gpio_num_t pin_m1; // [boolean value] forward or backward
    gpio_num_t pin_e1; // [PWM] speed
    gpio_num_t pin_m2;
    gpio_num_t pin_e2;
} motor_config_t;


void mcpwm_initialize(motor_config_t config);

void mcpwm_set_direction(float speed, int16_t direction, motor_config_t config);

#endif //MCPWM_BRUSHED_DC_CONTROL_MOTORS_H
