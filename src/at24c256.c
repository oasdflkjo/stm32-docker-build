#include "at24c256.h"
#include <stdlib.h>
#include <string.h>

static I2C_HandleTypeDef *eeprom_i2c;

AT24C256_Status AT24C256_Init(I2C_HandleTypeDef *hi2c) {
    eeprom_i2c = hi2c;
    
    // Check if device is ready
    if (HAL_I2C_IsDeviceReady(eeprom_i2c, AT24C256_BASE_ADDR << 1, 3, 100) != HAL_OK) {
        return AT24C256_ERROR;
    }
    
    return AT24C256_OK;
}

AT24C256_Status AT24C256_WriteByte(uint16_t address, uint8_t data) {
    uint8_t buffer[3];
    
    // Prepare buffer: address (2 bytes) + data (1 byte)
    buffer[0] = (address >> 8) & 0xFF;    // High byte of address
    buffer[1] = address & 0xFF;           // Low byte of address
    buffer[2] = data;
    
    if (HAL_I2C_Master_Transmit(eeprom_i2c, AT24C256_BASE_ADDR << 1, buffer, 3, HAL_MAX_DELAY) != HAL_OK) {
        return AT24C256_ERROR;
    }
    
    // Wait for write cycle to complete
    HAL_Delay(AT24C256_WRITE_TIME);
    return AT24C256_OK;
}

AT24C256_Status AT24C256_ReadByte(uint16_t address, uint8_t *data) {
    uint8_t addr_buffer[2];
    
    // Send address
    addr_buffer[0] = (address >> 8) & 0xFF;
    addr_buffer[1] = address & 0xFF;
    
    if (HAL_I2C_Master_Transmit(eeprom_i2c, AT24C256_BASE_ADDR << 1, addr_buffer, 2, HAL_MAX_DELAY) != HAL_OK) {
        return AT24C256_ERROR;
    }
    
    // Read data
    if (HAL_I2C_Master_Receive(eeprom_i2c, AT24C256_BASE_ADDR << 1, data, 1, HAL_MAX_DELAY) != HAL_OK) {
        return AT24C256_ERROR;
    }
    
    return AT24C256_OK;
}

AT24C256_Status AT24C256_Write(uint16_t address, uint8_t *data, uint16_t size) {
    uint16_t bytes_written = 0;
    uint16_t current_addr = address;
    
    // Static buffer for address + maximum page size
    static uint8_t buffer[2 + AT24C256_PAGE_SIZE];
    
    while (bytes_written < size) {
        // Calculate remaining bytes in current page
        uint16_t page_remain = AT24C256_PAGE_SIZE - (current_addr % AT24C256_PAGE_SIZE);
        uint16_t to_write = (size - bytes_written) < page_remain ? 
                           (size - bytes_written) : page_remain;
        
        // Prepare buffer: address (2 bytes) + data
        buffer[0] = (current_addr >> 8) & 0xFF;
        buffer[1] = current_addr & 0xFF;
        memcpy(&buffer[2], &data[bytes_written], to_write);
        
        // Write the page
        if (HAL_I2C_Master_Transmit(eeprom_i2c, AT24C256_BASE_ADDR << 1, 
                                   buffer, to_write + 2, HAL_MAX_DELAY) != HAL_OK) {
            return AT24C256_ERROR;
        }
        
        // Wait for write cycle to complete
        HAL_Delay(AT24C256_WRITE_TIME);
        
        bytes_written += to_write;
        current_addr += to_write;
    }
    
    return AT24C256_OK;
}

AT24C256_Status AT24C256_Read(uint16_t address, uint8_t *data, uint16_t size) {
    uint8_t addr_buffer[2];
    
    // Send address
    addr_buffer[0] = (address >> 8) & 0xFF;
    addr_buffer[1] = address & 0xFF;
    
    if (HAL_I2C_Master_Transmit(eeprom_i2c, AT24C256_BASE_ADDR << 1, 
                               addr_buffer, 2, HAL_MAX_DELAY) != HAL_OK) {
        return AT24C256_ERROR;
    }
    
    // Read data
    if (HAL_I2C_Master_Receive(eeprom_i2c, AT24C256_BASE_ADDR << 1, 
                              data, size, HAL_MAX_DELAY) != HAL_OK) {
        return AT24C256_ERROR;
    }
    
    return AT24C256_OK;
} 