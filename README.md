[![Unit Tests](https://github.com/oasdflkjo/stm32-docker-build/actions/workflows/test.yml/badge.svg)](https://github.com/oasdflkjo/stm32-docker-build/actions/workflows/test.yml)
# STM32 Spinning Cube with Containerized Build Environment

![Blinky Demo](assets/demo.gif)

This project demonstrates a simple STM32 spinning cube example with a containerized cross-compilation build environment. This approach simplifies setup and ensures consistency across different development machines.

Project is based on [STM32L152RE Nucleo board](https://www.st.com/en/evaluation-tools/nucleo-l152re.html).

Pinout from mbed [STM32L152RE MBED](https://os.mbed.com/platforms/ST-Nucleo-L152RE/).

## Features

- STM32L152RE microcontroller target
- Containerized build environment using Docker
- gcc-arm-none-eabi toolchain for cross-compilation
- ccache for faster compilations with caching
- Make as the build tool (future plans for CMake-Ninja or Meson)
- CMSIS and HAL libraries included for easy start
- Minimalist driver for ssd1306
- Unit testing support with Unity framework
- Continuous Integration with GitHub Actions
- Memory usage info on builds
```
Memory Usage Analysis
====================
Free FLASH: 478.86 KB (490360 bytes, 93.52%)
Free RAM: 30.39 KB (31124 bytes, 94.98%)

Detailed Memory Analysis
Text (Flash) size: 32424 bytes
Data size: 12 bytes
BSS size: 1632 bytes
Stack size: 0 bytes
Total RAM usage: 1644 bytes
```

## Prerequisites

- Docker
- Git
- PowerShell :)
- ST-LINK Utility (for flashing)

## Usage

1. Clone the repository:
   ```
   git clone https://github.com/yourusername/stm32-blinky-docker.git
   cd stm32-blinky-docker
   ```

2. Initialize and update the submodules:
   ```
   git submodule init
   git submodule update --init --recursive
   ```
   This will clone the CMSIS and STM32CubeL1 libraries into the `lib/` directory.

3. TODO...

## License

This project is open source and available under the [MIT License](LICENSE).
