/* Motor Driver Testing
 * Created by Joe Verbist on January 29th 2021.
*/

#include <stdio.h>

#include "/home/caincil/esp-idf/components/freertos/FreeRTOS-Kernel/include/freertos/FreeRTOS.h"
#include "/home/caincil/esp-idf/components/freertos/FreeRTOS-Kernel/include/freertos/task.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "esp_mac.h"

#include "nvs_flash.h"
#include "esp_event.h"
#include "wifi.h"
#include "motors.h"
static const char *TAG = "MAIN";

// Your actual wiring:
//  ENA: GPIO26, IN1: GPIO33, IN2: GPIO27
//  ENB: GPIO15, IN3: GPIO32, IN4: GPIO25
static motor_config_t motor_cfg = {
    .left  = { .pin_en   = GPIO_NUM_26,
               .pin_dir1 = GPIO_NUM_33,
               .pin_dir2 = GPIO_NUM_27 },
    .right = { .pin_en   = GPIO_NUM_15,
               .pin_dir1 = GPIO_NUM_32,
               .pin_dir2 = GPIO_NUM_25 }
};

void app_main(void)
{
    esp_log_level_set("*", ESP_LOG_INFO);
    ESP_LOGI(TAG, "Starting motor & Wi-Fi init");

    // 1) initialize PWM / GPIO for motors
    mcpwm_initialize(&motor_cfg);

    // 2) initialize Wi-Fi and wait for connection
    wifi_init_sta();

    // 3) start the TCP-server task, passing &motor_cfg
    xTaskCreate(
        tcp_server_task,
        "tcp_server",
        4096,
        (void*)&motor_cfg,
        5,
        NULL
    );
}