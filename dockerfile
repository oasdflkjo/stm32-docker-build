# Use the latest Ubuntu LTS base image
FROM ubuntu:24.04

# Install the ARM GCC toolchain and make
RUN apt-get update && apt-get install -y \
    gcc-arm-none-eabi \
    make \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Set the working directory inside the container
WORKDIR /work

# Default command to run if no command is specified
CMD ["bash"]
