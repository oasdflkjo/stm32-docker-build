CC=ccache arm-none-eabi-gcc
OBJCOPY=arm-none-eabi-objcopy

CFLAGS=-mcpu=cortex-m3 -mthumb -O3 -g \
       -Ilib/STM32CubeL1/Drivers/CMSIS/Core/Include \
       -Ilib/STM32CubeL1/Drivers/STM32L1xx_HAL_Driver/Inc \
       -Ilib/STM32CubeL1/Drivers/CMSIS/Device/ST/STM32L1xx/Include \
       -Ilib/STM32CubeL1/Drivers/BSP/STM32L1xx_Nucleo \
       -Iinc \
       -DSTM32L152xE -DUSE_HAL_DRIVER

LDFLAGS=-T lib/STM32CubeL1/Projects/NUCLEO-L152RE/Templates/SW4STM32/STM32L152RE_NUCLEO/STM32L152RETx_FLASH.ld \
        -Wl,--gc-sections

SRC_DIR = src
HAL_SRC_DIR = lib/STM32CubeL1/Drivers/STM32L1xx_HAL_Driver/Src
BUILD_DIR = build

SRCS = $(wildcard $(SRC_DIR)/*.c)
HAL_SRCS = $(HAL_SRC_DIR)/stm32l1xx_hal.c \
           $(HAL_SRC_DIR)/stm32l1xx_hal_cortex.c \
           $(HAL_SRC_DIR)/stm32l1xx_hal_gpio.c \
           $(HAL_SRC_DIR)/stm32l1xx_hal_rcc.c \
           $(HAL_SRC_DIR)/stm32l1xx_hal_uart.c \
           $(HAL_SRC_DIR)/stm32l1xx_hal_dma.c \
           $(HAL_SRC_DIR)/stm32l1xx_hal_flash.c \
           $(HAL_SRC_DIR)/stm32l1xx_hal_flash_ex.c \
           $(HAL_SRC_DIR)/stm32l1xx_hal_flash_ramfunc.c \
           $(HAL_SRC_DIR)/stm32l1xx_hal_pwr.c \
           $(HAL_SRC_DIR)/stm32l1xx_hal_pwr_ex.c \
           $(HAL_SRC_DIR)/stm32l1xx_hal_rcc_ex.c \
           $(HAL_SRC_DIR)/stm32l1xx_hal_rcc_ex.c \


OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))
HAL_OBJS = $(patsubst $(HAL_SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(HAL_SRCS))

FLASH_SIZE = 524288  # 512 KB in bytes

all: $(BUILD_DIR)/main.elf size_info

$(BUILD_DIR)/main.elf: $(OBJS) $(HAL_OBJS) $(BUILD_DIR)/startup_stm32l152xe.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	$(OBJCOPY) -O ihex $@ $(BUILD_DIR)/main.hex
	$(OBJCOPY) -O binary $@ $(BUILD_DIR)/main.bin

size_info: $(BUILD_DIR)/main.bin
	@echo "Binary size:"
	@ls -l $(BUILD_DIR)/main.bin | awk '{print $$5 " bytes"}'
	@echo "Flash usage:"
	@ls -l $(BUILD_DIR)/main.bin | awk '{printf "%.2f%%\n", ($$5 / $(FLASH_SIZE)) * 100}'

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(HAL_SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/startup_stm32l152xe.o: SW4STM32/startup_stm32l152xe.s | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $@

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean size_info
