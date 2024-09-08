#ifndef SSD1306_H
#define SSD1306_H

#include "stm32l1xx_hal.h"
#include <stdint.h>

// Configuration
#define SSD1306_WIDTH    128
#define SSD1306_HEIGHT   64
#define SSD1306_BUFFER_SIZE (SSD1306_WIDTH * SSD1306_HEIGHT / 8)

// Function prototypes
void SSD1306_Init(I2C_HandleTypeDef *hi2c, uint8_t address);
void SSD1306_DisplayOn(void);
void SSD1306_DisplayOff(void);
uint8_t* SSD1306_GetBuffer(void);
void SSD1306_SendBufferToDisplay(void);  // Remove the I2C_HandleTypeDef parameter

#endif // SSD1306_H