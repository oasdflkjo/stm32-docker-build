#include "stm32l1xx.h"

uint32_t SystemCoreClock = 32000000; // Default to 32 MHz

void SystemInit(void)
{
    // Reset the RCC clock configuration to the default reset state
    // Set MSION bit
    RCC->CR |= (uint32_t)0x00000100;

    // Reset SW, HPRE, PPRE1, PPRE2, MCOSEL and MCOPRE bits
    RCC->CFGR &= (uint32_t)0x88FFC00C;

    // Reset HSION, HSEON, CSSON and PLLON bits
    RCC->CR &= (uint32_t)0xEEFEFFFE;

    // Reset HSEBYP bit
    RCC->CR &= (uint32_t)0xFFFBFFFF;

    // Reset PLLSRC, PLLMUL and PLLDIV bits
    RCC->CFGR &= (uint32_t)0xFF02FFFF;

    // Disable all interrupts
    RCC->CIR = 0x00000000;
}

void SystemCoreClockUpdate(void)
{
    // Update SystemCoreClock based on clock register values
    // For simplicity, we'll assume it's still 32 MHz
    SystemCoreClock = 32000000;
}