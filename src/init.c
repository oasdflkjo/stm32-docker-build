#include "init.h"
#include "main.h"

void SystemClock_Config(void)
{
    /* Enable HSI and wait for ready */
    LL_RCC_HSI_Enable();
    while(LL_RCC_HSI_IsReady() != 1) {}

    /* Set HSI as system clock */
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI);
    while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSI) {}

    /* Enable USART2 clock */
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);
}

void MX_GPIO_Init(void)
{
    /* Enable GPIOA clock */
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);

    /* Minimal LED config */
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_5, LL_GPIO_MODE_OUTPUT);

    /* Minimal UART pins config */
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_2, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_3, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetAFPin_0_7(GPIOA, LL_GPIO_PIN_2, LL_GPIO_AF_7);
    LL_GPIO_SetAFPin_0_7(GPIOA, LL_GPIO_PIN_3, LL_GPIO_AF_7);
}

void MX_USART2_UART_Init(void)
{
    LL_USART_SetTransferDirection(USART2, LL_USART_DIRECTION_TX_RX);
    LL_USART_ConfigCharacter(USART2, LL_USART_DATAWIDTH_8B, LL_USART_PARITY_NONE, LL_USART_STOPBITS_1);
    LL_USART_SetBaudRate(USART2, 16000000, LL_USART_OVERSAMPLING_16, 115200);
    LL_USART_Enable(USART2);
} 