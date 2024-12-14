#include "flash.h"
#include "uart.h"
#include <string.h>

static const char hex_chars[] = "0123456789ABCDEF";

static void send_hex_byte(uint8_t byte)
{
    char hex[3];
    hex[0] = hex_chars[byte >> 4];
    hex[1] = hex_chars[byte & 0x0F];
    hex[2] = '\0';
    UART_Print(hex);
}

static uint8_t calculate_checksum(const uint8_t* data, uint8_t length, uint8_t sum)
{
    for(uint8_t i = 0; i < length; i++) {
        sum += data[i];
    }
    return ~sum + 1;
}

void Flash_DumpMemory(uint32_t start_addr, uint32_t length)
{
    uint8_t buffer[16];
    uint32_t addr;
    uint8_t checksum;
    
    // Send start of HEX file
    UART_Print(":020000040800F2\r\n");  // Extended Linear Address Record for 0x0800
    
    for(addr = start_addr; addr < start_addr + length; addr += 16)
    {
        uint8_t bytes_to_read = (addr + 16 <= start_addr + length) ? 16 : (start_addr + length - addr);
        
        // Copy data from flash
        memcpy(buffer, (void*)addr, bytes_to_read);
        
        // Start of record
        UART_Print(":");
        
        // Length
        send_hex_byte(bytes_to_read);
        
        // Address
        send_hex_byte((addr & 0xFF00) >> 8);
        send_hex_byte(addr & 0x00FF);
        
        // Record type (00 = data)
        UART_Print("00");
        
        // Calculate checksum
        checksum = bytes_to_read;
        checksum += (addr & 0xFF00) >> 8;
        checksum += addr & 0x00FF;
        
        // Data
        for(uint8_t i = 0; i < bytes_to_read; i++)
        {
            send_hex_byte(buffer[i]);
            checksum += buffer[i];
        }
        
        // Checksum
        send_hex_byte(~checksum + 1);
        
        // End of record
        UART_Print("\r\n");
    }
    
    // End of file record
    UART_Print(":00000001FF\r\n");
} 