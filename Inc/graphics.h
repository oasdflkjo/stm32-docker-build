#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdint.h>
#include "ssd1306.h"  // For DisplayConfig

// bind the display configuration to the graphics driver
void Graphics_Init(DisplayConfig* display);
// update the buffer
void Graphics_Update(void);

#endif // GRAPHICS_H