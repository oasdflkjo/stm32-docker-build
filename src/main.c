#include "main.h"
#include "init.h"
#include "uart.h"
#include "shell.h"

static inline void delay(uint32_t count)
{
    while(count--) { __NOP(); }
}

int main(void)
{
    /* Configure System Clock */
    SystemClock_Config();
    
    /* Initialize GPIO */
    MX_GPIO_Init();
    
    /* Initialize UART */
    MX_USART2_UART_Init();
    
    /* LED on - indicate system is ready */
    LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_5);
    
    /* Initialize shell */
    Shell_Init();
    
    /* Main loop */
    while (1)
    {
        Shell_Process();
        // LL_GPIO_TogglePin(GPIOA, LL_GPIO_PIN_5);
        // delay(500000);  // Reduced delay for better responsiveness
    }
}

void Error_Handler(void)
{
    UART_Print("ERROR: System halted\r\n");
    while (1)
    {
        LL_GPIO_TogglePin(GPIOA, LL_GPIO_PIN_5);
        delay(100000); // Fast blink delay
    }
}