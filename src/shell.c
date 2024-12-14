#include "shell.h"
#include "uart.h"
#include "flash.h"
#include <stdio.h>
#include <string.h>

#define HISTORY_SIZE 8
#define VT100_UP    "\x1B[A"
#define VT100_DOWN  "\x1B[B"
#define VT100_RIGHT "\x1B[C"
#define VT100_LEFT  "\x1B[D"

static char cmd_buffer[SHELL_MAX_CMD_LEN];
static uint8_t cmd_index = 0;

// Command history
static char history[HISTORY_SIZE][SHELL_MAX_CMD_LEN];
static uint8_t history_count = 0;
static uint8_t history_index = 0;
static uint8_t history_current = 0;

// ANSI escape sequence processing
static uint8_t escape_seq[4];
static uint8_t escape_index = 0;

// Function prototypes
static void process_escape_sequence(void);
static void add_to_history(const char* cmd);
static void restore_from_history(uint8_t index);
static void clear_line(void);
static void handle_tab(void);
static const char* find_completion(const char* partial, const char* const* options, uint8_t num_options, uint8_t* matches);
static void show_possibilities(const char* partial, const char* const* options, uint8_t num_options);

typedef void (*cmd_handler_t)(char *args);

typedef struct {
    const char *cmd;
    cmd_handler_t handler;
    const char *help;
} shell_cmd_t;

// Command handlers
static void cmd_help(char *args);
static void cmd_info(char *args);
static void cmd_reset(char *args);
static void cmd_led(char *args);
static void cmd_dump(char *args);
static void cmd_jump(char *args);

// Command table
static const shell_cmd_t commands[] = {
    {"help", cmd_help, "Show this help message"},
    {"info", cmd_info, "Show bootloader info"},
    {"reset", cmd_reset, "Reset the device"},
    {"led", cmd_led, "Control LED (on/off/toggle)"},
    {"dump", cmd_dump, "Dump flash memory in Intel HEX format"},
    {"jump", cmd_jump, "Jump to application"},
    {NULL, NULL, NULL}
};

// Add these arrays for tab completion
static const char* led_options[] = {"on", "off", "toggle"};
#define LED_OPTIONS_COUNT (sizeof(led_options)/sizeof(led_options[0]))

void Shell_Init(void)
{
    UART_Print("\r\n=== Bootloader Shell ===\r\n");
    UART_Print(SHELL_PROMPT);
}

void Shell_Process(void)
{
    uint8_t c;
    
    if (!LL_USART_IsActiveFlag_RXNE(USART2)) {
        return;
    }
    
    c = LL_USART_ReceiveData8(USART2);
    
    // Handle escape sequences
    if (escape_index > 0 || c == '\x1B') {
        escape_seq[escape_index++] = c;
        
        if (escape_index == 1 && c != '\x1B') {
            escape_index = 0;  // Invalid escape sequence
        }
        else if (escape_index == 3) {
            process_escape_sequence();
            escape_index = 0;
            return;
        }
        return;
    }
    
    // Echo printable characters
    if (c >= ' ' && c <= '~') {
        LL_USART_TransmitData8(USART2, c);
    }
    
    // Handle backspace
    if (c == '\b' || c == 0x7F) {
        if (cmd_index > 0) {
            cmd_index--;
            UART_Print("\b \b");
        }
        return;
    }
    
    // Handle tab
    if (c == '\t') {
        handle_tab();
        return;
    }
    
    // Handle enter key
    if (c == '\r' || c == '\n') {
        UART_Print("\r\n");
        
        if (cmd_index > 0) {
            cmd_buffer[cmd_index] = '\0';
            add_to_history(cmd_buffer);
            
            // Parse command
            char *cmd = cmd_buffer;
            char *args = strchr(cmd_buffer, ' ');
            if (args) {
                *args = '\0';
                args++;
            }
            
            // Find and execute command
            const shell_cmd_t *cmd_entry = commands;
            while (cmd_entry->cmd) {
                if (strcmp(cmd, cmd_entry->cmd) == 0) {
                    cmd_entry->handler(args);
                    break;
                }
                cmd_entry++;
            }
            
            if (!cmd_entry->cmd) {
                UART_Print("Unknown command: ");
                UART_Print(cmd);
                UART_Print("\r\n");
            }
        }
        
        history_current = history_count;
        cmd_index = 0;
        UART_Print(SHELL_PROMPT);
        return;
    }
    
    // Store character if buffer not full
    if (cmd_index < SHELL_MAX_CMD_LEN - 1 && c >= ' ' && c <= '~') {
        cmd_buffer[cmd_index++] = c;
    }
}

static void process_escape_sequence(void)
{
    if (escape_seq[1] != '[') return;
    
    switch(escape_seq[2]) {
        case 'A': // Up arrow
            if (history_current > 0) {
                history_current--;
                clear_line();
                restore_from_history(history_current);
            }
            break;
            
        case 'B': // Down arrow
            if (history_current < history_count) {
                history_current++;
                clear_line();
                if (history_current < history_count) {
                    restore_from_history(history_current);
                } else {
                    cmd_index = 0;
                    cmd_buffer[0] = '\0';
                }
            }
            break;
    }
}

static void add_to_history(const char* cmd)
{
    if (strlen(cmd) == 0) return;
    
    // Don't add if it's the same as the last command
    if (history_count > 0 && strcmp(history[history_count-1], cmd) == 0) {
        return;
    }
    
    if (history_count < HISTORY_SIZE) {
        strcpy(history[history_count++], cmd);
    } else {
        // Shift history
        for (int i = 0; i < HISTORY_SIZE-1; i++) {
            strcpy(history[i], history[i+1]);
        }
        strcpy(history[HISTORY_SIZE-1], cmd);
    }
    history_current = history_count;
}

static void restore_from_history(uint8_t index)
{
    strcpy(cmd_buffer, history[index]);
    cmd_index = strlen(cmd_buffer);
    UART_Print(SHELL_PROMPT);
    UART_Print(cmd_buffer);
}

static void clear_line(void)
{
    // Clear current line
    UART_Print("\r");
    for (uint8_t i = 0; i < SHELL_MAX_CMD_LEN + strlen(SHELL_PROMPT); i++) {
        UART_Print(" ");
    }
    UART_Print("\r");
}

static void cmd_help(char *args)
{
    (void)args;
    UART_Print("Available commands:\r\n");
    
    const shell_cmd_t *cmd = commands;
    while (cmd->cmd) {
        UART_Print("  ");
        UART_Print(cmd->cmd);
        UART_Print(" - ");
        UART_Print(cmd->help);
        UART_Print("\r\n");
        cmd++;
    }
}

static void cmd_info(char *args)
{
    (void)args;
    UART_Print("Bootloader v0.1\r\n");
    UART_Print("Target: STM32L152RE\r\n");
}

static void cmd_reset(char *args)
{
    (void)args;
    UART_Print("Resetting...\r\n");
    NVIC_SystemReset();
}

static void cmd_led(char *args)
{
    if (!args) {
        UART_Print("Usage: led <on|off|toggle>\r\n");
        return;
    }

    if (strcmp(args, "on") == 0) {
        LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_5);
        UART_Print("LED turned on\r\n");
    }
    else if (strcmp(args, "off") == 0) {
        LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_5);
        UART_Print("LED turned off\r\n");
    }
    else if (strcmp(args, "toggle") == 0) {
        LL_GPIO_TogglePin(GPIOA, LL_GPIO_PIN_5);
        UART_Print("LED toggled\r\n");
    }
    else {
        UART_Print("Unknown LED command. Use: on, off, or toggle\r\n");
    }
}

static void cmd_dump(char *args) {
    uint32_t start_addr = FLASH_BASE;  // 0x08000000
    uint32_t length = 16384;           // 16KB - bootloader size
    char hex_buffer[10];               // Buffer for hex conversion
    
    UART_Print("Dumping flash contents:\r\n");
    
    uint8_t *ptr = (uint8_t *)start_addr;
    for (uint32_t i = 0; i < length; i += 16) {
        // Print address
        UART_Print("0x");
        for (int j = 28; j >= 0; j -= 4) {
            hex_buffer[0] = "0123456789ABCDEF"[(((uint32_t)(ptr + i)) >> j) & 0xF];
            hex_buffer[1] = '\0';
            UART_Print(hex_buffer);
        }
        UART_Print(": ");
        
        // Print hex values
        for (uint32_t j = 0; j < 16; j++) {
            hex_buffer[0] = "0123456789ABCDEF"[ptr[i + j] >> 4];
            hex_buffer[1] = "0123456789ABCDEF"[ptr[i + j] & 0xF];
            hex_buffer[2] = ' ';
            hex_buffer[3] = '\0';
            UART_Print(hex_buffer);
        }
        
        // Print ASCII
        UART_Print(" |");
        for (uint32_t j = 0; j < 16; j++) {
            char c = ptr[i + j];
            hex_buffer[0] = (c >= 32 && c <= 126) ? c : '.';
            hex_buffer[1] = '\0';
            UART_Print(hex_buffer);
        }
        UART_Print("|\r\n");
        
        // Small delay to not overwhelm UART
        for(volatile int d = 0; d < 1000; d++);
    }
    
    UART_Print("\r\nDump complete.\r\n");
}

static void cmd_jump(char *args) {
    uint32_t app_address = 0x08004000;  // Application start address
    
    UART_Print("Jumping to application...\r\n");
    
    // Get the application stack pointer and reset handler
    uint32_t *app_vector_table = (uint32_t*)app_address;
    uint32_t app_stack_pointer = app_vector_table[0];
    uint32_t app_reset_handler = app_vector_table[1];
    
    // Basic validation
    if ((app_stack_pointer & 0x2FFE0000) != 0x20000000) {
        UART_Print("Error: Invalid stack pointer\r\n");
        return;
    }
    
    // Disable interrupts
    __disable_irq();
    
    // Reset peripherals (optional)
    HAL_DeInit();
    
    // Set vector table offset
    SCB->VTOR = app_address;
    
    // Set stack pointer
    __set_MSP(app_stack_pointer);
    
    // Jump to application
    void (*app_reset_handler_ptr)(void) = (void*)app_reset_handler;
    app_reset_handler_ptr();
}

static void handle_tab(void)
{
    cmd_buffer[cmd_index] = '\0';
    
    // Find space to separate command from arguments
    char* space = strchr(cmd_buffer, ' ');
    
    if (space == NULL) {
        // Complete command name
        uint8_t matches = 0;
        const char* completion = NULL;
        const char* cmd_options[16];  // Max commands
        uint8_t cmd_count = 0;
        
        // Build array of command names
        const shell_cmd_t* cmd = commands;
        while (cmd->cmd) {
            cmd_options[cmd_count++] = cmd->cmd;
            cmd++;
        }
        
        completion = find_completion(cmd_buffer, cmd_options, cmd_count, &matches);
        
        if (matches == 1 && completion) {
            // Single match - complete it
            clear_line();
            UART_Print(SHELL_PROMPT);
            strcpy(cmd_buffer, completion);
            strcat(cmd_buffer, " ");
            cmd_index = strlen(cmd_buffer);
            UART_Print(cmd_buffer);
        }
        else if (matches > 1) {
            // Show possibilities
            UART_Print("\r\n");
            show_possibilities(cmd_buffer, cmd_options, cmd_count);
            UART_Print(SHELL_PROMPT);
            UART_Print(cmd_buffer);
        }
    }
    else {
        // Complete arguments
        *space = '\0';
        char* arg = space + 1;
        
        // Currently only LED command has completable arguments
        if (strcmp(cmd_buffer, "led") == 0) {
            uint8_t matches = 0;
            const char* completion = find_completion(arg, led_options, LED_OPTIONS_COUNT, &matches);
            
            if (matches == 1 && completion) {
                // Single match - complete it
                clear_line();
                UART_Print(SHELL_PROMPT);
                *space = ' ';
                strcpy(arg, completion);
                cmd_index = strlen(cmd_buffer);
                UART_Print(cmd_buffer);
            }
            else if (matches > 1) {
                // Show possibilities
                UART_Print("\r\n");
                show_possibilities(arg, led_options, LED_OPTIONS_COUNT);
                UART_Print(SHELL_PROMPT);
                *space = ' ';
                UART_Print(cmd_buffer);
            }
        }
        else {
            *space = ' ';  // Restore space
        }
    }
}

static const char* find_completion(const char* partial, const char* const* options, uint8_t num_options, uint8_t* matches)
{
    const char* completion = NULL;
    size_t len = strlen(partial);
    *matches = 0;
    
    for (uint8_t i = 0; i < num_options; i++) {
        if (strncmp(partial, options[i], len) == 0) {
            (*matches)++;
            completion = options[i];
        }
    }
    
    return (*matches == 1) ? completion : NULL;
}

static void show_possibilities(const char* partial, const char* const* options, uint8_t num_options)
{
    size_t len = strlen(partial);
    
    for (uint8_t i = 0; i < num_options; i++) {
        if (strncmp(partial, options[i], len) == 0) {
            UART_Print("  ");
            UART_Print(options[i]);
            UART_Print("\r\n");
        }
    }
} 