# Use the latest Ubuntu LTS base image
FROM ubuntu:24.04

# Install the ARM GCC toolchain, make, and ccache
RUN apt-get update && apt-get install -y \
    gcc-arm-none-eabi \
    make \
    ccache \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Set the working directory inside the container
WORKDIR /work

# Configure ccache to use a specific cache size (optional)
RUN ccache --max-size=10G

# Default command to run if no command is specified
CMD ["bash"]