# STM32 EEPROM Image Storage Demo

This branch demonstrates storing and displaying images using AT24C256 EEPROM and SSD1306 OLED display.

![Demo Setup](assets/image.png)

## What it Does

1. Takes a PNG image and converts it to a C header file using a Python script
2. Writes the image data to AT24C256 EEPROM
3. Reads the image back from EEPROM
4. Displays it on the SSD1306 OLED screen

## Components Used

- STM32L152RE Nucleo board
- SSD1306 OLED Display (128x64)
- AT24C256 EEPROM
- Both connected via I2C1 (SCL: PB8, SDA: PB9)
