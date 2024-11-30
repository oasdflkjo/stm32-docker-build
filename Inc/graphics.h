#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdint.h>
#include "ssd1306.h"

// Initialize graphics with display configuration
void Graphics_Init(DisplayConfig* display);

// Update graphics frame
void Graphics_Update(void);

#endif // GRAPHICS_H