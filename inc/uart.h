#ifndef UART_H
#define UART_H

#include "main.h"
#include <stdint.h>

void UART_Transmit(const uint8_t* data, uint16_t len);
void UART_Print(const char* str);

#endif // UART_H 