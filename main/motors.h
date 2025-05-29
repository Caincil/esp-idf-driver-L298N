// motors.h
#ifndef MOTORS_H
#define MOTORS_H

#include "driver/gpio.h"

// Одна «голова» мотора: PWM + два DIR
typedef struct {
    gpio_num_t pin_en;    // PWM → ENA/ENB
    gpio_num_t pin_dir1;  // IN1/IN3
    gpio_num_t pin_dir2;  // IN2/IN4
} one_motor_t;

// Конфиг из двух моторов
typedef struct {
    one_motor_t left;
    one_motor_t right;
} motor_config_t;

void mcpwm_initialize(const motor_config_t *cfg);
void mcpwm_set_direction(float speed, int16_t direction,
                         const motor_config_t *cfg);
void mcpwm_motor_stop(void);

#endif // MOTORS_H