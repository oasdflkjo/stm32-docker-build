#include "main.h"
#include <string.h>
#include "stm32l1xx_hal.h"
#include "stm32l1xx_hal_dma.h"
#include "stm32l1xx_hal_gpio.h"
#include "stm32l1xx_hal_uart.h"
#include "stm32l1xx_hal_i2c.h"
#include <stdlib.h>

UART_HandleTypeDef huart2;
I2C_HandleTypeDef hi2c1;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void UART_Transmit(const char* msg);
static void MX_I2C1_Init(void);
static void I2C_Transmit(uint16_t DevAddress, uint8_t *pData, uint16_t Size);
static uint8_t SSD1306_IsConnected(void);
static void SSD1306_Init(void);
static void SSD1306_WriteCommand(uint8_t command);
static void SSD1306_WriteData(uint8_t* data, uint16_t size);
static void SSD1306_DrawHalfScreen(void);

int main(void)
{
  HAL_Init();
  SystemClock_Config();

  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_I2C1_Init();

  char* uart_msg_found = "SSD1306 found and initialized!\r\n";
  char* uart_msg_not_found = "SSD1306 not found!\r\n";

  // Check if SSD1306 is connected
  if (SSD1306_IsConnected())
  {
    UART_Transmit(uart_msg_found);
    SSD1306_Init();
    SSD1306_DrawHalfScreen();
  }
  else
  {
    UART_Transmit(uart_msg_not_found);
  }

  while (1)
  {
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
    HAL_Delay(1000);
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

static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();  // Enable GPIOB clock

  // Configure PA5 (LED)
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  // Configure PB8 and PB9 for I2C
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
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

static void MX_I2C1_Init(void)
{
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;  // 400 kHz (Fast mode)
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
}

void Error_Handler(void) {
  __disable_irq();
  while (1) {}
}

static void UART_Transmit(const char* msg) {
  HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
}

static void I2C_Transmit(uint16_t DevAddress, uint8_t *pData, uint16_t Size)
{
  HAL_I2C_Master_Transmit(&hi2c1, DevAddress, pData, Size, HAL_MAX_DELAY);
}

static uint8_t SSD1306_IsConnected(void)
{
    uint16_t address = 0x3C << 1;  // SSD1306 address (0x3C) shifted left by 1
    if (HAL_I2C_IsDeviceReady(&hi2c1, address, 2, 10) == HAL_OK)
    {
        return 1;  // Device found
    }
    return 0;  // Device not found
}

#ifdef USE_FULL_ASSERT

void assert_failed(uint8_t* file, uint32_t line) {}
#endif

#define SSD1306_ADDR (0x3C << 1)

static void SSD1306_Init(void)
{
    uint8_t init_sequence[] = {
        0xAE,   // Display off
        0xD5,   // Set display clock divide ratio/oscillator frequency
        0x80,   // Set divide ratio
        0xA8,   // Set multiplex ratio
        0x3F,   // 64 MUX
        0xD3,   // Set display offset
        0x00,   // No offset
        0x40,   // Set display start line
        0x8D,   // Set charge pump
        0x14,   // Enable charge pump
        0x20,   // Set memory addressing mode
        0x00,   // Horizontal addressing mode
        0xA1,   // Set segment re-map
        0xC8,   // Set COM output scan direction
        0xDA,   // Set COM pins hardware configuration
        0x12,
        0x81,   // Set contrast control
        0xCF,
        0xD9,   // Set pre-charge period
        0xF1,
        0xDB,   // Set VCOMH deselect level
        0x40,
        0xA4,   // Entire display ON
        0xA6,   // Set normal display
        0xAF    // Display on
    };

    for (uint8_t i = 0; i < sizeof(init_sequence); i++) {
        SSD1306_WriteCommand(init_sequence[i]);
    }
}

static void SSD1306_WriteCommand(uint8_t command)
{
    uint8_t data[2] = {0x00, command};  // 0x00 for command
    HAL_I2C_Master_Transmit(&hi2c1, SSD1306_ADDR, data, 2, HAL_MAX_DELAY);
}

static void SSD1306_WriteData(uint8_t* data, uint16_t size)
{
    uint8_t buffer[65];  // Max 64 bytes of data + 1 byte for command
    if (size > 64) {
        Error_Handler();  // Handle error if data is too large
    }
    buffer[0] = 0x40;  // 0x40 for data
    memcpy(buffer + 1, data, size);
    HAL_I2C_Master_Transmit(&hi2c1, SSD1306_ADDR, buffer, size + 1, HAL_MAX_DELAY);
}

static void SSD1306_DrawHalfScreen(void)
{
    uint8_t data[64];  // One page of display data
    memset(data, 0xFF, 64);  // Fill with white pixels

    for (uint8_t page = 0; page < 4; page++) {  // Half the screen (4 pages)
        SSD1306_WriteCommand(0xB0 | page);  // Set page address
        SSD1306_WriteCommand(0x00);         // Set lower column address
        SSD1306_WriteCommand(0x10);         // Set higher column address
        SSD1306_WriteData(data, 64);
    }
}