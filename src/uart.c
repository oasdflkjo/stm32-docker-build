#include "uart.h"

void UART_Transmit(const uint8_t* data, uint16_t len)
{
    for(uint16_t i = 0; i < len; i++)
    {
        while (!LL_USART_IsActiveFlag_TXE(USART2));
        LL_USART_TransmitData8(USART2, data[i]);
    }
    while (!LL_USART_IsActiveFlag_TC(USART2));
}

void UART_Print(const char* str)
{
    while(*str)
    {
        while (!LL_USART_IsActiveFlag_TXE(USART2));
        LL_USART_TransmitData8(USART2, *str++);
    }
    while (!LL_USART_IsActiveFlag_TC(USART2));
} 