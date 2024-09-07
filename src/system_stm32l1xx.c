#include "stm32l1xx.h"
#include <stdint.h>

#if !defined(HSE_VALUE)
#define HSE_VALUE ((uint32_t)8000000U)
#endif

#if !defined(HSI_VALUE)
#define HSI_VALUE ((uint32_t)8000000U)
#endif

#define VECT_TAB_OFFSET 0x00U

uint32_t SystemCoreClock = 32000000;
const uint8_t PLLMulTable[9] = {3U, 4U, 6U, 8U, 12U, 16U, 24U, 32U, 48U};
const uint8_t AHBPrescTable[16] = {0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 1U, 2U, 3U, 4U, 6U, 7U, 8U, 9U};
const uint8_t APBPrescTable[8] = {0U, 0U, 0U, 0U, 1U, 2U, 3U, 4U};

#if defined(STM32L151xD) || defined(STM32L152xD) || defined(STM32L162xD)
#ifdef DATA_IN_ExtSRAM
static void SystemInit_ExtMemCtl(void);
#endif
#endif

void SystemInit(void)
{
  RCC->CR |= (uint32_t)0x00000100;

  RCC->CFGR &= (uint32_t)0x88FFC00C;

  RCC->CR &= (uint32_t)0xEEFEFFFE;

  RCC->CR &= (uint32_t)0xFFFBFFFF;

  RCC->CFGR &= (uint32_t)0xFF02FFFF;

  RCC->CIR = 0x00000000;

#ifdef DATA_IN_ExtSRAM
  SystemInit_ExtMemCtl();
#endif

#ifdef VECT_TAB_SRAM
  SCB->VTOR = SRAM_BASE | VECT_TAB_OFFSET;
#else
  SCB->VTOR = FLASH_BASE | VECT_TAB_OFFSET;
#endif
}

void SystemCoreClockUpdate(void)
{
  uint32_t tmp = 0, pllmul = 0, plldiv = 0, pllsource = 0, msirange = 0;

  tmp = RCC->CFGR & RCC_CFGR_SWS;

  switch (tmp)
  {
  case 0x00:
    msirange = (RCC->ICSCR & RCC_ICSCR_MSIRANGE) >> 13;
    SystemCoreClock = (32768 * (1 << (msirange + 1)));
    break;
  case 0x04:
    SystemCoreClock = HSI_VALUE;
    break;
  case 0x08:
    SystemCoreClock = HSE_VALUE;
    break;
  case 0x0C:
    pllmul = RCC->CFGR & RCC_CFGR_PLLMUL;
    plldiv = RCC->CFGR & RCC_CFGR_PLLDIV;
    pllmul = PLLMulTable[(pllmul >> 18)];
    plldiv = (plldiv >> 22) + 1;

    pllsource = RCC->CFGR & RCC_CFGR_PLLSRC;

    if (pllsource == 0x00)
    {
      SystemCoreClock = (((HSI_VALUE)*pllmul) / plldiv);
    }
    else
    {
      SystemCoreClock = (((HSE_VALUE)*pllmul) / plldiv);
    }
    break;
  default:
    msirange = (RCC->ICSCR & RCC_ICSCR_MSIRANGE) >> 13;
    SystemCoreClock = (32768 * (1 << (msirange + 1)));
    break;
  }
  tmp = AHBPrescTable[((RCC->CFGR & RCC_CFGR_HPRE) >> 4)];
  SystemCoreClock >>= tmp;
}
