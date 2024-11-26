CC=ccache arm-none-eabi-gcc
OBJCOPY=arm-none-eabi-objcopy
SIZE=arm-none-eabi-size

# Define common flags
CFLAGS=-mcpu=cortex-m3 -mthumb \
       -Ilib/STM32CubeL1/Drivers/CMSIS/Core/Include \
       -Ilib/STM32CubeL1/Drivers/STM32L1xx_HAL_Driver/Inc \
       -Ilib/STM32CubeL1/Drivers/CMSIS/Device/ST/STM32L1xx/Include \
       -Ilib/STM32CubeL1/Drivers/BSP/STM32L1xx_Nucleo \
       -Iinc \
       -DSTM32L152xE -DUSE_HAL_DRIVER

LDFLAGS=-T lib/STM32CubeL1/Projects/NUCLEO-L152RE/Templates/SW4STM32/STM32L152RE_NUCLEO/STM32L152RETx_FLASH.ld \
        -Wl,--gc-sections \
        -Wl,-Map=$(BUILD_DIR)/main.map

SRC_DIR = src
HAL_SRC_DIR = lib/STM32CubeL1/Drivers/STM32L1xx_HAL_Driver/Src
BUILD_DIR = build

SRCS = $(wildcard $(SRC_DIR)/*.c)
# Exclude the template file from HAL sources
HAL_SRCS = $(filter-out $(HAL_SRC_DIR)/stm32l1xx_hal_timebase_tim_template.c, $(wildcard $(HAL_SRC_DIR)/*.c))

OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))
HAL_OBJS = $(patsubst $(HAL_SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(HAL_SRCS))

# Set MAKEFLAGS to use 16 threads by default
MAKEFLAGS += -j16

# Default target
all: fast

# Fast build
fast: CFLAGS += -O3
fast: $(BUILD_DIR)/main.elf

# Debug build
debug: CFLAGS += -g -O0
debug: $(BUILD_DIR)/main.elf

# Small build
small: CFLAGS += -Os
small: $(BUILD_DIR)/main.elf

$(BUILD_DIR)/main.elf: $(OBJS) $(HAL_OBJS) $(BUILD_DIR)/startup_stm32l152xe.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	$(OBJCOPY) -O ihex $@ $(BUILD_DIR)/main.hex
	$(OBJCOPY) -O binary $@ $(BUILD_DIR)/main.bin
	$(SIZE) $@
	@./scripts/analyze_memory.sh $(BUILD_DIR)/main.elf $(BUILD_DIR)/main.map



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

.PHONY: all clean fast debug small

# Note: MAKEFLAGS is set to use 16 threads by default for Ryzen 7 5700X3D.
# Adjust the number of jobs (-j) according to your CPU's capabilities if needed.