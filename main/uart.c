//
// Created by Caincil on 29/05/2025.
//
#include "driver/uart.h"

#define UART_NUM            UART_NUM_0
#define UART_BUF_SIZE       128

void uart_init(void)
{
    const uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    // Настраиваем UART0
    ESP_ERROR_CHECK(uart_param_config(UART_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM, UART_PIN_NO_CHANGE,
                                UART_PIN_NO_CHANGE,
                                UART_PIN_NO_CHANGE,
                                UART_PIN_NO_CHANGE));
    // Буфер для RX, без пула TX
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM, UART_BUF_SIZE * 2, 0, 0, NULL, 0));
}