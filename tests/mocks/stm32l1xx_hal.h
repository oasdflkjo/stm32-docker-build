#ifndef MOCK_STM32L1XX_HAL_H
#define MOCK_STM32L1XX_HAL_H

#include <stdint.h>

// Common HAL types we need
typedef enum {
    HAL_OK       = 0x00U,
    HAL_ERROR    = 0x01U,
    HAL_BUSY     = 0x02U,
    HAL_TIMEOUT  = 0x03U
} HAL_StatusTypeDef;

// I2C Handle Structure (simplified for testing)
typedef struct {
    void* Instance;
    uint32_t State;
} I2C_HandleTypeDef;

// Add any other HAL definitions needed by your tests

#endif // MOCK_STM32L1XX_HAL_H 