#include "main.h"
#include "ssd1306.h"
#include "graphics.h"
#include "stm32l1xx_hal.h"
#include "stm32l1xx_hal_dma.h"
#include "stm32l1xx_hal_gpio.h"
#include "stm32l1xx_hal_i2c.h"
#include "stm32l1xx_hal_uart.h"
#include "stm32l1xx_hal_rcc.h"
#include "stm32l1xx_hal_pwr.h"
#include <string.h>
#include <stdio.h>
#include "at24c256.h"
#include "image.h"

UART_HandleTypeDef huart2;
I2C_HandleTypeDef hi2c1;

#define DISPLAY_WIDTH  128
#define DISPLAY_HEIGHT 64

static uint8_t image_buffer[IMAGE_SIZE];  // Buffer to hold the image data

// Function declarations
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static HAL_StatusTypeDef MX_I2C1_Init(void);
static void UART_Transmit(const char* msg);
static void print_buffer(const char* prefix, uint8_t* buffer, uint16_t size);
void WriteImageToEEPROM(const uint8_t* image_data);

#define SSD1306_I2C_ADDR 0x3C  // Define the I2C address of the OLED display

// GPIO definitions
#define LED_GPIO_PORT               GPIOA
#define LED_PIN                     GPIO_PIN_5
#define I2C_SCL_PORT               GPIOB
#define I2C_SCL_PIN                GPIO_PIN_8
#define I2C_SDA_PORT               GPIOB
#define I2C_SDA_PIN                GPIO_PIN_9

static char debug_str[32];  // Move from main() to global scope

// Add this function to convert from byte-per-pixel to packed format
static void ConvertToDisplayFormat(const uint8_t* source, uint8_t* dest) {
    for (int y = 0; y < DISPLAY_HEIGHT; y += 8) {
        for (int x = 0; x < DISPLAY_WIDTH; x++) {
            uint8_t packed_byte = 0;
            // Pack 8 vertical pixels into one byte
            for (int bit = 0; bit < 8; bit++) {
                if (source[x + (y + bit) * DISPLAY_WIDTH] > 0) {
                    packed_byte |= (1 << bit);
                }
            }
            dest[x + (y/8) * DISPLAY_WIDTH] = packed_byte;
        }
    }
}

int main(void) {
    HAL_StatusTypeDef status;
    uint8_t* display_buffer;  // Will get this from SSD1306 driver

    // Initialize HAL
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_USART2_UART_Init();
    
    // Initialize I2C
    status = MX_I2C1_Init();
    if (status != HAL_OK) {
        Error_Handler();
    }

    // Initialize EEPROM
    if (AT24C256_Init(&hi2c1) != AT24C256_OK) {
        Error_Handler();
    }

    // LED on - indicate starting EEPROM write
    HAL_GPIO_WritePin(LED_GPIO_PORT, LED_PIN, GPIO_PIN_SET);
    
    // Write the checkerboard pattern to EEPROM
    WriteImageToEEPROM(image_data);
    
    // LED off - EEPROM write complete
    HAL_GPIO_WritePin(LED_GPIO_PORT, LED_PIN, GPIO_PIN_RESET);

    // Initialize OLED display
    SSD1306_Init(&hi2c1, SSD1306_I2C_ADDR);
    display_buffer = SSD1306_GetBuffer();  // Get the display's buffer

    // Read image from EEPROM into temporary buffer
    if (AT24C256_Read(0x0000, image_buffer, IMAGE_SIZE) != AT24C256_OK) {
        Error_Handler();
    }

    // Convert from byte-per-pixel to OLED format directly into display buffer
    ConvertToDisplayFormat(image_buffer, display_buffer);

    // Send to display using the driver's function
    SSD1306_SendBufferToDisplay();
    
    // If we get here, everything worked!
    while (1) {
        HAL_GPIO_TogglePin(LED_GPIO_PORT, LED_PIN);  // Blink LED to show success
        HAL_Delay(500);
    }
}

void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLL_DIV3;

  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK) {
    Error_Handler();
  }

  // Enable UART2 and I2C1 clocks
  __HAL_RCC_USART2_CLK_ENABLE();
  __HAL_RCC_I2C1_CLK_ENABLE();
}

static void MX_GPIO_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  // Configure PA5 (LED)
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  // Configure PB8 and PB9 for I2C with pull-ups
  GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;  // Add pull-up resistors
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;  // Lower the speed
  GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  // Initialize LED to OFF state
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
}

static void MX_USART2_UART_Init(void) {
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;

  if (HAL_UART_Init(&huart2) != HAL_OK) {
    Error_Handler();
  }
}

static HAL_StatusTypeDef MX_I2C1_Init(void) {
    HAL_StatusTypeDef status;

    // 1. Enable peripherals and GPIO clocks
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_I2C1_CLK_ENABLE();

    // 2. Configure GPIO pins
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = I2C_SCL_PIN | I2C_SDA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;  // Back to very high speed
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(I2C_SCL_PORT, &GPIO_InitStruct);

    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 400000;  // Back to 400 kHz
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

    return HAL_I2C_Init(&hi2c1);
}

void Error_Handler(void) {
  // Keep interrupts enabled for blinking
  while (1) {
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);  // Blink the onboard LED
    HAL_Delay(100);  // Fast blink indicates error
  }
}

static void UART_Transmit(const char* msg) {
  HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
}

static void print_buffer(const char* prefix, uint8_t* buffer, uint16_t size) {
    static char debug_str[64];  // Make the buffer static
    sprintf(debug_str, "%s: ", prefix);
    UART_Transmit(debug_str);
    
    for(uint16_t i = 0; i < size; i++) {
        sprintf(debug_str, "%02X ", buffer[i]);
        UART_Transmit(debug_str);
    }
    UART_Transmit("\r\n");
}

// Add this utility function to write the image to EEPROM
void WriteImageToEEPROM(const uint8_t* image_data) {
    // Write the image data to EEPROM
    if (AT24C256_Write(0x0000, (uint8_t*)image_data, IMAGE_SIZE) != AT24C256_OK) {
        Error_Handler();
    }
    
    // Verify the write
    uint8_t verify_buffer[IMAGE_SIZE];
    if (AT24C256_Read(0x0000, verify_buffer, IMAGE_SIZE) != AT24C256_OK) {
        Error_Handler();
    }
    
    // Compare written data
    if (memcmp(image_data, verify_buffer, IMAGE_SIZE) != 0) {
        Error_Handler();
    }
}