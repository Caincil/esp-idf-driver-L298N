// wifi.h
#ifndef WIFI_H_
#define WIFI_H_

#include "motors.h"

// Your network credentials:
#define WIFI_SSID       "DIR-842-3C05"
#define WIFI_PASS       "12345678"

// TCP port that ESP32 will listen on:
#define TCP_SERVER_PORT 5000

/**
 * @brief Initialize Wi-Fi in station mode, 
 *        wait until connected and got IP.
 */
void wifi_init_sta(void);

/**
 * @brief FreeRTOS task: simple TCP server.
 *        Expects pvParameters = (motor_config_t*)pointer.
 *        Reads lines "S:<speed>,D:<direction>\n"
 *        and calls mcpwm_set_direction(...).
 */
void tcp_server_task(void *pvParameters);

#endif // WIFI_H_
