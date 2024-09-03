CC=arm-none-eabi-gcc
OBJCOPY=arm-none-eabi-objcopy

CFLAGS=-mcpu=cortex-m3 -mthumb -O0 -g \
       -Ilib/CMSIS/CMSIS/Core/Include \
       -Ilib/STM32CubeL1/Drivers/STM32L1xx_HAL_Driver/Inc \
       -Ilib/STM32CubeL1/Drivers/CMSIS/Device/ST/STM32L1xx/Include \
       -Iinclude \
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
           $(HAL_SRC_DIR)/stm32l1xx_hal_rcc.c

OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))
HAL_OBJS = $(patsubst $(HAL_SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(HAL_SRCS))

all: $(BUILD_DIR)/main.elf

$(BUILD_DIR)/main.elf: $(OBJS) $(HAL_OBJS) $(BUILD_DIR)/startup_stm32l152xe.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	$(OBJCOPY) -O ihex $@ $(BUILD_DIR)/main.hex
	$(OBJCOPY) -O binary $@ $(BUILD_DIR)/main.bin

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(HAL_SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/startup_stm32l152xe.o: $(SRC_DIR)/startup_stm32l152xe.s | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $@

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean