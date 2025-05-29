#ifndef UART_H
#define UART_H

#include "driver/uart.h"

// Номер UART и размер буфера вы можете вынести сюда, если хотите
#define UART_NUM      UART_NUM_0
#define UART_BUF_SIZE 128

// Прототип функции
void uart_init(void);

#endif // UART_H