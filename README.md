# STM32 Blinky with Containerized Build Environment

This project demonstrates a simple STM32 blinky example with a twist: it uses a containerized cross-compilation build environment. This approach simplifies setup and ensures consistency across different development machines.

Project is based on [STM32L152RE Nucleo board](https://www.st.com/en/evaluation-tools/nucleo-l152re.html).

## Features

- STM32L152RE microcontroller target
- Containerized build environment using Docker
- gcc-arm-none-eabi toolchain for cross-compilation
- ccache for faster compilations with caching
- Make as the build tool (future plans for CMake-Ninja or Meson)
- CMSIS and HAL libraries included for easy start

## Why This Project?

This project was created to provide a simple, reproducible setup for STM32 development. It offers an alternative to IDEs like Keil or STM32CubeIDE, focusing on a more flexible, command-line based approach.

## Prerequisites

- Docker
- Git

## Usage

1. Clone the repository:
   ```
   git clone https://github.com/yourusername/stm32-blinky-docker.git
   cd stm32-blinky-docker
   ```

2. Build the Docker image:
   ```
   docker build -t stm32-build-env .
   ```

3. Run the Docker container:
   ```
   docker run -it --rm -v $(pwd):/work stm32-build-env
   ```

4. Inside the container, build the project:
   ```
   make
   ```

5. The compiled binary will be available in the `build` directory.

## Project Structure

- `src/`: Source files
- `include/`: Header files
- `lib/`: CMSIS and HAL libraries
- `Dockerfile`: Defines the build environment
- `Makefile`: Build configuration

## TODO

- Implement FreeRTOS variant
- Consider migrating to CMake-Ninja or Meson for more advanced build configurations

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## License

This project is open source and available under the [MIT License](LICENSE).