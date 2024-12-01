#!/bin/bash

ELF_FILE=$1
MAP_FILE=$2
# bootloader is 16KB
BOOTLOADER_SIZE=16384
FLASH_SIZE=$BOOTLOADER_SIZE
RAM_SIZE=32768     # 32KB for STM32L152RE

# Check if files exist
if [ ! -f "$ELF_FILE" ] || [ ! -f "$MAP_FILE" ]; then
    echo "Error: ELF or MAP file not found"
    exit 1
fi

echo "Memory Usage Analysis"
echo "===================="

# Flash usage calculation
read -r text data <<< "$(arm-none-eabi-size "$ELF_FILE" | awk '/\.elf/ {print $1, $2}')"
if [ -z "$text" ] || [ -z "$data" ]; then
    echo "Error: Unable to calculate flash usage."
    exit 1
fi

# Calculate free flash
FLASH_USED=$(($text + $data))
FREE_FLASH=$((FLASH_SIZE - FLASH_USED))
FREE_FLASH_KB=$(bc <<< "scale=2; $FREE_FLASH / 1024")
FREE_FLASH_PCT=$(bc <<< "scale=2; $FREE_FLASH * 100 / $FLASH_SIZE")
echo "Free FLASH: $FREE_FLASH_KB KB ($FREE_FLASH bytes, $FREE_FLASH_PCT%)"

# Detailed memory analysis
awk -v ram_size=$RAM_SIZE '
     /^\.text/ { 
         sub(/^0x/, "", $3); 
         text = sprintf("%d", "0x" $3); 
     } 
     /^\.data/ { 
         sub(/^0x/, "", $3); 
         data = sprintf("%d", "0x" $3); 
     } 
     /^\.bss/ { 
         sub(/^0x/, "", $3); 
         bss = sprintf("%d", "0x" $3); 
     } 
     /^\.stack/ { 
         sub(/^0x/, "", $3); 
         stack = sprintf("%d", "0x" $3); 
     } 
     END { 
         total_ram = data + bss + stack;
         free_ram = ram_size - total_ram;
         free_ram_kb = free_ram / 1024;
         free_ram_pct = (free_ram / ram_size) * 100;
         
         printf "Free RAM: %.2f KB (%d bytes, %.2f%%)\n\n", 
                free_ram_kb, free_ram, free_ram_pct;
         
         print "Detailed Memory Analysis";
         printf "Text (Flash) size: %d bytes\n", text + 0; 
         printf "Data size: %d bytes\n", data + 0; 
         printf "BSS size: %d bytes\n", bss + 0; 
         printf "Stack size: %d bytes\n", stack + 0; 
         printf "Total RAM usage: %d bytes\n", total_ram;
     }' "$MAP_FILE"