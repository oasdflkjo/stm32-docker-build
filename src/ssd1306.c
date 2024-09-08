#include "ssd1306.h"
#include <string.h>

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

static void SSD1306_WriteCommand(uint8_t command) {
    uint8_t data[2] = {0x00, command};
    HAL_I2C_Master_Transmit(ssd1306_i2c, ssd1306_i2c_addr, data, 2, HAL_MAX_DELAY);
}

static void SSD1306_WriteData(uint8_t* data, uint16_t size) {
    uint8_t buffer[65];  // Max 64 bytes of data + 1 byte for data control
    if (size > 64) {
        // Handle error: data too large
        return;
    }
    buffer[0] = 0x40;  // 0x40 for data
    memcpy(buffer + 1, data, size);
    HAL_I2C_Master_Transmit(ssd1306_i2c, ssd1306_i2c_addr, buffer, size + 1, HAL_MAX_DELAY);
}

void SSD1306_Init(I2C_HandleTypeDef *hi2c, uint8_t address) {
    ssd1306_i2c = hi2c;
    ssd1306_i2c_addr = address << 1;  // Convert 7-bit address to 8-bit
    HAL_Delay(100);  // Wait for the display to power up

    for (uint8_t i = 0; i < sizeof(ssd1306_init_sequence); i++) {
        SSD1306_WriteCommand(ssd1306_init_sequence[i]);
    }

    memset(ssd1306_buffer, 0, SSD1306_BUFFER_SIZE);
    SSD1306_SendBufferToDisplay();
}

void SSD1306_DisplayOn(void) {
    SSD1306_WriteCommand(0xAF);
}

void SSD1306_DisplayOff(void) {
    SSD1306_WriteCommand(0xAE);
}

uint8_t* SSD1306_GetBuffer(void) {
    return ssd1306_buffer;
}

void SSD1306_SendBufferToDisplay(void) {
    SSD1306_WriteCommand(0x20);  // Set memory addressing mode
    SSD1306_WriteCommand(0x00);  // Horizontal addressing mode
    SSD1306_WriteCommand(0x21);  // Set column address
    SSD1306_WriteCommand(0);     // Start at 0
    SSD1306_WriteCommand(127);   // End at 127
    SSD1306_WriteCommand(0x22);  // Set page address
    SSD1306_WriteCommand(0);     // Start at 0
    SSD1306_WriteCommand(7);     // End at 7

    for (uint16_t i = 0; i < SSD1306_BUFFER_SIZE; i += 64) {
        SSD1306_WriteData(&ssd1306_buffer[i], 64);
    }
}