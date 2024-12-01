#ifndef AT24C256_H
#define AT24C256_H

#include "stm32l1xx_hal.h"

// Device parameters
#define AT24C256_BASE_ADDR     0x50    // 7-bit address
#define AT24C256_PAGE_SIZE     64      // bytes
#define AT24C256_TOTAL_SIZE    32768   // 256 kbit = 32 KBytes
#define AT24C256_WRITE_TIME    5       // ms, max time to write a page

// Status codes
typedef enum {
    AT24C256_OK = 0,
    AT24C256_ERROR
} AT24C256_Status;

// Initialize the EEPROM
AT24C256_Status AT24C256_Init(I2C_HandleTypeDef *hi2c);

// Write data to EEPROM
AT24C256_Status AT24C256_Write(uint16_t address, uint8_t *data, uint16_t size);

// Read data from EEPROM
AT24C256_Status AT24C256_Read(uint16_t address, uint8_t *data, uint16_t size);

// Write a single byte
AT24C256_Status AT24C256_WriteByte(uint16_t address, uint8_t data);

// Read a single byte
AT24C256_Status AT24C256_ReadByte(uint16_t address, uint8_t *data);

#endif // AT24C256_H 