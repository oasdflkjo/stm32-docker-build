# STM32L1 UART Bootloader

A minimal, reliable bootloader for STM32L1 series microcontrollers featuring a command-line interface over UART.

Most things are still in TODO state.

## Overview

This bootloader provides:
- UART communication at 115200 baud
- Interactive command shell
- Flash programming capabilities
- Simple and reliable design running at 16MHz HSI
- Command history and basic terminal features

## Hardware Requirements

- STM32L152RE or compatible
- UART connection (through ST-LINK or USB-UART converter)
- Connections:
  - PA2 (USART2_TX)
  - PA3 (USART2_RX)
  - PA5 (LED status indicator)

## Design Choices

- Uses 16MHz HSI for reliability
- No external crystal required
- Simple command parsing
- Robust error checking
- Minimal dependencies

## Safety Features

- Write protection checks
- Checksum verification
- Invalid command protection
- Flash operation verification


## License

GNU GPLv3

## Acknowledgments

- STM32 LL Libraries
