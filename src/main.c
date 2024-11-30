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

UART_HandleTypeDef huart2;
I2C_HandleTypeDef hi2c1;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static HAL_StatusTypeDef MX_I2C1_Init(void);

#define SSD1306_I2C_ADDR 0x3C  // Define the I2C address of the OLED display

// GPIO definitions
#define LED_GPIO_PORT               GPIOA
#define LED_PIN                     GPIO_PIN_5
#define I2C_SCL_PORT               GPIOB
#define I2C_SCL_PIN                GPIO_PIN_8
#define I2C_SDA_PORT               GPIOB
#define I2C_SDA_PIN                GPIO_PIN_9

int main(void) {
    HAL_StatusTypeDef status;

    // Initialize HAL
    HAL_Init();
    SystemClock_Config();

    // Initialize GPIO for LED
    MX_GPIO_Init();
    
    // Initialize LED to OFF
    HAL_GPIO_WritePin(LED_GPIO_PORT, LED_PIN, GPIO_PIN_RESET);

    // Initialize I2C
    status = MX_I2C1_Init();
    if (status != HAL_OK) {
        Error_Handler();  // LED will blink to indicate error
    }

    if (HAL_I2C_IsDeviceReady(&hi2c1, SSD1306_I2C_ADDR << 1, 1, 100) != HAL_OK) {
        Error_Handler();
    }

    // Initialize display
    SSD1306_Init(&hi2c1, SSD1306_I2C_ADDR);
    Graphics_Init(SSD1306_GetDisplayConfig());

    while (1) {
        Graphics_Update();
        SSD1306_SendBufferToDisplay();
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