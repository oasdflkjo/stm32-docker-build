#ifndef SSD1306_H
#define SSD1306_H

#include "stm32l1xx_hal.h"
#include <stdint.h>

typedef struct {
    uint16_t width;
    uint16_t height;
    uint8_t* buffer;
} DisplayConfig;

void SSD1306_Init(I2C_HandleTypeDef *hi2c, uint8_t address);
void SSD1306_TurnOnDisplay(void);
void SSD1306_TurnOffDisplay(void);
DisplayConfig* SSD1306_GetDisplayConfig(void);
void SSD1306_SendBufferToDisplay(void);

#endif // SSD1306_H