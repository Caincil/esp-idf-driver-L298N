//
// Created by th3et3rnalz on 29/01/2021.
//
#include "driver/mcpwm.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "/home/caincil/esp-idf/tools/tools/xtensa-esp-elf/esp-14.2.0_20240906/xtensa-esp-elf/xtensa-esp-elf/include/math.h"
#include "esp_mac.h"
#include "motors.h"

static const char *TAG = "MOTOR_DRIVER";
const int pwm_frequency = 1000;  // Hz
/*
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
*/
void mcpwm_initialize(const motor_config_t *cfg)
{
    ESP_LOGI(TAG, "MCPWM init");

    // Настраиваем оба PWM-канала
    mcpwm_config_t pconf = {
        .frequency = pwm_frequency,
        .cmpr_a = 0,
        .cmpr_b = 0,
        .counter_mode = MCPWM_UP_COUNTER,
        .duty_mode = MCPWM_DUTY_MODE_0
    };
    // Левый мотор → канал A
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, cfg->left.pin_en);
    // Правый мотор → канал B
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0B, cfg->right.pin_en);
    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pconf);

    // DIR-пины в GPIO OUTPUT
    gpio_set_direction(cfg->left.pin_dir1, GPIO_MODE_OUTPUT);
    gpio_set_direction(cfg->left.pin_dir2, GPIO_MODE_OUTPUT);
    gpio_set_direction(cfg->right.pin_dir1, GPIO_MODE_OUTPUT);
    gpio_set_direction(cfg->right.pin_dir2, GPIO_MODE_OUTPUT);
}

void mcpwm_set_direction(float speed, int16_t direction,
                         const motor_config_t *cfg)
{
    // 1) Санитизация
    speed = fminf(fmaxf(speed, 0.0f), 100.0f);
    direction = (direction % 360 + 360) % 360;

    // 2) Вычисляем raw-скважности
    float dL, dR;
    float rad2 = 2 * direction * M_PI / 180.0f;
    if (direction < 90) {
        dL =  speed;
        dR =  cosf(rad2) * speed;
    } else if (direction < 180) {
        dL = -cosf(rad2) * speed;
        dR = -speed;
    } else if (direction < 270) {
        dL = -speed;
        dR = -cosf(rad2) * speed;
    } else {
        dL =  cosf(rad2) * speed;
        dR =  speed;
    }

    // 3) Определяем полярность и abs()
    bool fwdL = (dL >= 0), fwdR = (dR >= 0);
    float dutyL = fabsf(dL), dutyR = fabsf(dR);

    // 4) Устанавливаем PWM
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, dutyL);
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, dutyR);
    mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0,
                        MCPWM_OPR_A, MCPWM_DUTY_MODE_0);
    mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0,
                        MCPWM_OPR_B, MCPWM_DUTY_MODE_0);

    // 5) Выставляем DIR-линии
    // Левый мотор
    gpio_set_level(cfg->left.pin_dir1, fwdL ? 1 : 0);
    gpio_set_level(cfg->left.pin_dir2, fwdL ? 0 : 1);
    // Правый мотор
    gpio_set_level(cfg->right.pin_dir1, fwdR ? 1 : 0);
    gpio_set_level(cfg->right.pin_dir2, fwdR ? 0 : 1);
}

void mcpwm_motor_stop(void)
{
    // Обнуляем PWM
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, 0);
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, 0);
}

