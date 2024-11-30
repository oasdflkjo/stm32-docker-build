#include "ssd1306.h"
#include <string.h>

// Display configuration constants
#define SSD1306_WIDTH    128
#define SSD1306_HEIGHT   64
#define SSD1306_BUFFER_SIZE (SSD1306_WIDTH * SSD1306_HEIGHT / 8)

static uint8_t ssd1306_buffer[SSD1306_BUFFER_SIZE];
static I2C_HandleTypeDef *ssd1306_i2c;
static uint8_t ssd1306_i2c_addr;

static const uint8_t ssd1306_init_sequence[] = {
    0xAE,        // Display off
    0xD5, 0x80,  // Set display clock divide ratio/oscillator frequency
    0xA8, 0x3F,  // Set multiplex ratio (1 to 64)
    0xD3, 0x00,  // Set display offset
    0x40,        // Set start line address
    0x8D, 0x14,  // Enable charge pump regulator
    0x20, 0x00,  // Set memory addressing mode (horizontal)
    0xA1,        // Set segment re-map
    0xC8,        // Set COM output scan direction
    0xDA, 0x12,  // Set COM pins hardware configuration
    0x81, 0xCF,  // Set contrast control
    0xD9, 0xF1,  // Set pre-charge period
    0xDB, 0x40,  // Set VCOMH deselect level
    0xA4,        // Disable entire display on
    0xA6,        // Set normal display
    0xAF         // Display on
};

static inline HAL_StatusTypeDef SSD1306_WriteCommand(uint8_t command) {
    uint8_t data[2] = {0x00, command};
    return HAL_I2C_Master_Transmit(ssd1306_i2c, ssd1306_i2c_addr, data, 2, HAL_MAX_DELAY);
}

static inline HAL_StatusTypeDef SSD1306_WriteData(uint8_t* data, uint16_t size) {
    uint8_t buffer[65];  // Max 64 bytes of data + 1 byte for data control
    if (size > 64) {
        return HAL_ERROR;
    }
    buffer[0] = 0x40;  // 0x40 for data
    memcpy(buffer + 1, data, size);
    return HAL_I2C_Master_Transmit(ssd1306_i2c, ssd1306_i2c_addr, buffer, size + 1, HAL_MAX_DELAY);
}

static inline HAL_StatusTypeDef SSD1306_SetAddressRange(uint8_t col_start, uint8_t col_end, 
                                                       uint8_t page_start, uint8_t page_end) {
    HAL_StatusTypeDef status;
    
    status = SSD1306_WriteCommand(0x21);  // Set column address
    if (status != HAL_OK) return status;
    
    status = SSD1306_WriteCommand(col_start);
    if (status != HAL_OK) return status;
    
    status = SSD1306_WriteCommand(col_end);
    if (status != HAL_OK) return status;
    
    status = SSD1306_WriteCommand(0x22);  // Set page address
    if (status != HAL_OK) return status;
    
    status = SSD1306_WriteCommand(page_start);
    if (status != HAL_OK) return status;
    
    return SSD1306_WriteCommand(page_end);
}

// Add display configuration struct
static DisplayConfig display_config = {
    .width = SSD1306_WIDTH,
    .height = SSD1306_HEIGHT,
    .buffer = NULL
};

void SSD1306_Init(I2C_HandleTypeDef *hi2c, uint8_t address) {
    HAL_StatusTypeDef status;
    
    ssd1306_i2c = hi2c;
    ssd1306_i2c_addr = address << 1;  // Convert 7-bit address to 8-bit
    display_config.buffer = ssd1306_buffer;
    
    HAL_Delay(100);  // Wait for display power-up

    // Send init sequence
    for (uint8_t i = 0; i < sizeof(ssd1306_init_sequence); i++) {
        status = SSD1306_WriteCommand(ssd1306_init_sequence[i]);
        if (status != HAL_OK) {
            Error_Handler();  // Handle I2C error
        }
    }

    // Clear display
    memset(ssd1306_buffer, 0, SSD1306_BUFFER_SIZE);
    SSD1306_SendBufferToDisplay();
}

void SSD1306_TurnOnDisplay(void) {
    SSD1306_WriteCommand(0xAF);
}

void SSD1306_TurnOffDisplay(void) {
    SSD1306_WriteCommand(0xAE);
}

uint8_t* SSD1306_GetBuffer(void) {
    return ssd1306_buffer;
}

void SSD1306_SendBufferToDisplay(void) {
    SSD1306_SetAddressRange(0, 127, 0, 7);

    for (uint16_t i = 0; i < SSD1306_BUFFER_SIZE; i += 64) {
        SSD1306_WriteData(&ssd1306_buffer[i], 64);
    }
}

DisplayConfig* SSD1306_GetDisplayConfig(void) {
    return &display_config;
}