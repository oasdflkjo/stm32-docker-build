#include "main.h"
#include "init.h"
#include "uart_commands.h"

UART_HandleTypeDef huart2;
I2C_HandleTypeDef hi2c1;

int main(void) {
    // Initialize HAL and Clock
    HAL_Init();
    SystemClock_Config();
    
    // Initialize peripherals
    MX_GPIO_Init();
    MX_USART2_UART_Init();
    
    // Initialize UART command interface
    UARTCmd_Init(&huart2);
    
    // LED on - indicate system is ready
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
    
    // Main loop
    while (1) {
        UARTCmd_Process();
    }
}

void Error_Handler(void) {
    while (1) {
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
        HAL_Delay(100);
    }
}