//
// Created by th3et3rnalz on 29/01/2021.
//
#include "driver/mcpwm.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "/home/caincil/esp-idf/tools/tools/xtensa-esp-elf/esp-14.2.0_20240906/xtensa-esp-elf/xtensa-esp-elf/include/math.h"
#include "esp_mac.h"

static const char *TAG = "MOTOR_DRIVER";
const int pwm_frequency = 1000;  // Hz

typedef struct MotorConfig{
    gpio_num_t pin_m1; // [boolean value] forward or backward
    gpio_num_t pin_e1; // [PWM] speed
    gpio_num_t pin_m2;
    gpio_num_t pin_e2;
} motor_config_t;

float duty_cycle_motor_left;
float duty_cycle_motor_right;
int polarity_motor_left;
int polarity_motor_right;

void mcpwm_initialize(motor_config_t config)
{
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
    ESP_LOGI(TAG, "Initializing the PWM \n");

    // General config for both devices
    mcpwm_config_t pwm_config = {
            .frequency = pwm_frequency,
            .cmpr_a = 0,
            .cmpr_b = 0,
            .counter_mode = MCPWM_UP_COUNTER,
            .duty_mode = MCPWM_DUTY_MODE_0
    };
    // First pwm -- left motor
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, config.pin_e1);
    // Second pwm -- right motor
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0B, config.pin_e2);

    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);
    gpio_set_direction(config.pin_m1, GPIO_MODE_OUTPUT);
    gpio_set_pull_mode(config.pin_m1, GPIO_PULLDOWN_ONLY);
    gpio_set_direction(config.pin_m2, GPIO_MODE_OUTPUT);
    gpio_set_pull_mode(config.pin_m2, GPIO_PULLDOWN_ONLY);
}

void mcpwm_set_direction(float speed, int16_t direction, motor_config_t config)
{
    // INPUT SANITATION
    if (speed > 100){
        speed = 100;
    } else if (speed < 0){
        speed = 0;
    }
    if (direction > 360 || direction < 0){
        direction = direction % 360;
    }

    // INPUT MAPPING
    if (direction < 90){
        duty_cycle_motor_left = speed;
        duty_cycle_motor_right = (float) cos(2*direction*M_PI/ 180) * speed;

    } else if (direction < 180){
        duty_cycle_motor_left = (float) -cos(2*direction*M_PI / 180) * speed;
        duty_cycle_motor_right = -speed;

    } else if (direction < 270){
        duty_cycle_motor_left = -speed;
        duty_cycle_motor_right = (float) -cos(2*direction*M_PI/180) * speed;

    } else {
        duty_cycle_motor_left = (float) cos(2*direction * M_PI / 180) * speed;
        duty_cycle_motor_right = speed;
    }

    // DEBUGGING STUFF
    ESP_LOGI(TAG, "Left motor: %f, right motor: %f", duty_cycle_motor_left, duty_cycle_motor_right);

    // OUTPUT SANITATION
    if (duty_cycle_motor_left < 0){
        duty_cycle_motor_left *= -1; // absolute value
        polarity_motor_left = 0; // running backwards
    } else {
        polarity_motor_left = 1;
    }

    if (duty_cycle_motor_right < 0){
        duty_cycle_motor_right *= -1; // absolute value
        polarity_motor_right = 1; // running backwards
    } else {
        polarity_motor_right = 0;
    }

    // SETTING THE MOTOR VALUES
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, duty_cycle_motor_left);
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, duty_cycle_motor_right);
    mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, MCPWM_DUTY_MODE_0);
    mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, MCPWM_DUTY_MODE_0);

    gpio_set_level(config.pin_m1, polarity_motor_left); // Don't really care about this, but we have to set a value
    gpio_set_level(config.pin_m2, polarity_motor_right);
}

void mcpwm_motor_stop()
{
    printf("STOPPING ALL MOTORS");
//    mcpwm_set_signal_low();
//    mcpwm_set_signal_low();
}

