/* Motor Driver Testing
 * Created by Joe Verbist on January 29th 2021.
*/

#include <stdio.h>

#include "/home/caincil/esp-idf/components/freertos/FreeRTOS-Kernel/include/freertos/FreeRTOS.h"
#include "/home/caincil/esp-idf/components/freertos/FreeRTOS-Kernel/include/freertos/task.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "/home/caincil/esp-idf/components/hal/include/hal/uart_types.h"

#include "uart.h"
#include "motors.h"

static const char *TAG = "MAIN";
#define UART_NUM            UART_NUM_0
#define UART_BUF_SIZE       128

void mcpwm_loop(void *arg)
{
    // Конфигурация пинов по вашей распайке:
    motor_config_t cfg = {
        .left  = {
            .pin_en   = GPIO_NUM_26,  // ENA ← D26
            .pin_dir1 = GPIO_NUM_33,  // IN1 ← D33
            .pin_dir2 = GPIO_NUM_27   // IN2 ← D27
        },
        .right = {
            .pin_en   = GPIO_NUM_15,  // ENB ← D15
            .pin_dir1 = GPIO_NUM_32,  // IN3 ← D32
            .pin_dir2 = GPIO_NUM_25   // IN4 ← D25
        }
    };

    mcpwm_initialize(&cfg);
    uart_init();

    uint8_t buf[UART_BUF_SIZE + 1];
    int len;
    float speed = 0.0f;
    int16_t dir = 0;

    ESP_LOGI(TAG, "Entering main loop, waiting for UART commands L:<spd>,D:<dir>");

  while (1) {
        int len = uart_read_bytes(UART_NUM, buf, UART_BUF_SIZE, pdMS_TO_TICKS(100));
        if (len > 0) {
            buf[len] = '\0';
            if (sscanf((char*)buf, "S:%f,D:%hd", &speed, &dir) == 2) {
                mcpwm_set_direction(speed, dir, &cfg);
            } else {
                ESP_LOGW(TAG, "Bad format");
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
   
}

void app_main(void)
{
    esp_log_level_set("*", ESP_LOG_DEBUG);
    xTaskCreate(mcpwm_loop, "MotorControlLoop", 4096, NULL, 2, NULL);
}
