# stm32-build-env
FROM ubuntu:24.04

# Add repository for ST tools
RUN apt-get update && apt-get install -y wget
RUN wget -q https://apt.kitware.com/kitware-archive.sh && \
    bash kitware-archive.sh && \
    rm kitware-archive.sh

RUN apt-get update && apt-get install -y \
    gcc-arm-none-eabi \
    make \
    ccache \
    gdb-multiarch \
    openocd \
    udev \
    stlink-tools \
    libusb-1.0-0 \
    usbutils \
    linux-tools-generic \
    hwdata \
    git \
    stlink-tools \
    stlink-gui \
    gdb-arm-none-eabi \
    bc \
    gcc \
    libc6-dev \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Create alternatives for usbip
RUN update-alternatives --install /usr/local/bin/usbip usbip /usr/lib/linux-tools/*-generic/usbip 20


WORKDIR /workspaces

CMD ["bash"]