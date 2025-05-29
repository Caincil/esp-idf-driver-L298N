/* Motor Driver Testing
 * Created by Joe Verbist on January 29th 2021.
*/

#include <stdio.h>

#include "/home/caincil/esp-idf/components/freertos/FreeRTOS-Kernel/include/freertos/FreeRTOS.h"
#include "/home/caincil/esp-idf/components/freertos/FreeRTOS-Kernel/include/freertos/task.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "esp_mac.h"

#include "motors.h"

static const char *TAG = "MAIN";

void mcpwm_loop() {
    //1. mcpwm gpio initialization
    motor_config_t motor_config = {
        .pin_e1 = GPIO_NUM_27,
        .pin_m1 = GPIO_NUM_33,
        .pin_e2 = GPIO_NUM_15,
        .pin_m2 = GPIO_NUM_32
    };

    mcpwm_initialize(motor_config);
    float speed = 100;
    int16_t direction;


  while (1) {
        for (direction = 0; direction < 360; direction += 5){
            ESP_LOGI(TAG, "\n\nDIRECTION %d", direction);
          mcpwm_set_direction(speed, direction, motor_config);
            vTaskDelay(100/portTICK_PERIOD_MS);
        }
    }
   
}

void app_main(void)
{
    esp_log_level_set("*", ESP_LOG_DEBUG);
    xTaskCreate(mcpwm_loop, "MotorControlLoop", 4096, NULL, 2, NULL);
}
