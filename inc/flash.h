#ifndef FLASH_H
#define FLASH_H

#include <stdint.h>

// STM32L152RE has 512KB flash
#define BOOTLOADER_FLASH_START    0x08000000
#define BOOTLOADER_FLASH_SIZE    (512 * 1024)
#define BOOTLOADER_PAGE_SIZE     256

void Flash_DumpMemory(uint32_t start_addr, uint32_t length);

#endif // FLASH_H 