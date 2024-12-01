#ifndef INIT_H
#define INIT_H

#include "stm32l1xx_hal.h"

void SystemClock_Config(void);
void MX_GPIO_Init(void);
void MX_USART2_UART_Init(void);

#endif // INIT_H 