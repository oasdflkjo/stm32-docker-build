#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "ssd1306.h"
#include <stdint.h>

#define SSD1306_BUFFER_SIZE (SSD1306_WIDTH * SSD1306_HEIGHT / 8)

void Graphics_Init(uint8_t* buffer);
void Graphics_Run(void);

#endif // GRAPHICS_H